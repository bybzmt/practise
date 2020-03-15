extern crate libc;

use std::default::Default;
use std::mem;
use std::ptr;
use std::str;
// use std::process;
use std::u32::MAX as U32_MAX;
use std::u16::MAX as U16_MAX;
use std::cmp::Ordering;
use std::hash::Hash;
use std::hash::Hasher;
use std::rc::Rc;
use std::cell::RefCell;
use std::path::Path;
use std::fs::{self, File, OpenOptions};
use std::io;
use std::collections::HashSet;
// use std::io::prelude::*;

use fnv::FnvHasher;
use memmap::{MmapMut, MmapOptions};

// mod db;
// pub use db::DB;

const MAGIC_NUM : u32 = 12349872;

struct Db {
    meta:Rc<RefCell<Meta>>,
}

impl Db {
    pub fn open<P: AsRef<Path>>(path: P) -> io::Result<Db> {
        let need_init = match fs::metadata(&path) {
            Ok(_) => false,
            Err(_) => true,
        };

        let file = OpenOptions::new()
            .read(true).write(true).create(true).open(path)?;

        Db::load(file, need_init)
    }

    fn load(file: File, need_init:bool) -> io::Result<Db> {
        let mmap = Rc::new(Mmap::load(&file, need_init)?);

        let meta_id = mmap.get_meta_id();
        let meta = mmap.load_meta(meta_id).clone();

        let mut meta = Meta{
            file: Rc::new(file),
            meta_id: meta_id,
            low : meta,
            free_pages : Vec::new(),
            hold_pages : Vec::new(),
            cache_pages: HashSet::new(),
            mmap: mmap.clone(),
        };

        meta.init();

        let db = Db{
            meta: Rc::new(RefCell::new(meta)),
        };

        Ok(db)
    }

    fn getTxReader(&self) -> TxReader {
        let meta = self.meta.borrow().clone();
        let meta = Rc::new(RefCell::new(meta));

        TxReader{
            meta:meta,
        }
    }

    fn getTxWriter(&self) -> TxWriter {
        let meta = self.meta.borrow().clone();
        let meta = Rc::new(RefCell::new(meta));

        TxWriter{
            meta:meta,
        }
    }
}

struct TxReader {
    meta:Rc<RefCell<Meta>>,
}

impl TxReader {
    fn get(&self, key:&[u8]) -> Option<&[u8]> {
        let raw = meta_get(self.meta.clone(), key)?;

        let out = raw.0 as *const _ as *const [u8; U32_MAX as usize];
        unsafe {
            Some(&(*out)[..raw.1])
        }
    }
}

struct TxWriter {
    meta:Rc<RefCell<Meta>>,
}

impl TxWriter {
    fn get(&self, key:&[u8]) -> Option<&[u8]> {
        let raw = meta_get(self.meta.clone(), key)?;
        if raw.1 == 0 {
            return Some(&[]);
        }

        let out = raw.0 as *const _ as *const [u8; U32_MAX as usize];
        unsafe {
            Some(&(*out)[..raw.1])
        }
    }

    fn set(&self, key:&[u8], val:&[u8]) {
        let mut rtx = self.meta.borrow().as_mut();

        let root = rtx.low.root_page;

        let ids = if root == 0 {
            let tmp:LowLeafNode = Default::default();
            tmp.set_to(self.meta.clone(), key, val)
        } else {
            let meta = self.meta.borrow();
            let res = meta.mmap.node_type(root);
            if let NodeType::Node = res {
                let node = meta.mmap.load_node(root);
                node.set_to(self.meta.clone(), key, val)
            } else {
                let node = meta.mmap.load_leaf(root);
                node.set_to(self.meta.clone(), key, val)
            }
        };

        let ids = if ids.len() > 1 {

            let node = rtx.mmap.load_node(ids[0]);

            let key = &node.item(1).key;
            let key = String::from_utf8_lossy(key);
            println!("node2 {:?}", key);

            let pid = rtx.page_alloc(1);
            let node = rtx.mmap.load_node(pid);
            node.init(0);
            for id in ids {
                node.add_child(self.meta.clone(), id);
            }
            node.key_depth_check(self.meta.clone());

            vec![pid]
        } else {
            ids
        };

        rtx.low.root_page = ids[0];
        return;
    }

    fn del(&self, key:&[u8]) -> bool {
        let mut meta = self.meta.borrow().as_mut();
        let root = meta.low.root_page;
        if root == 0 {
            return false;
        }

        let res = meta.mmap.node_type(root);

        let res = if let NodeType::Node = res {
            let node = meta.mmap.load_node(root);
            node.del_to(self.meta.clone(), key)
        } else {
            let node = meta.mmap.load_leaf(root);
            node.del_to(self.meta.clone(), key)
        };

        match res {
            Some(pid) => {
                meta.low.root_page = pid;
                true
            },
            None => false,
        }
    }

    fn commit(&self) {
        let mut meta = self.meta.borrow().as_mut();
        let root = meta.low.root_page;

        let node = meta.mmap.load_node(root);

        let mut items = Vec::with_capacity(node.num as usize);

        for i in 0..node.num as usize {
            items.push(node.item(i as u16).clone());
        }

        // println!("items: {:?}", items);
    }
}

struct Mmap {
    page_size: u32,
    page_num: u32,
    meta_num : u16,
    data: MmapMut,
}

impl Mmap {
    fn load(fh:&File, init:bool) -> io::Result<Mmap> {
        let mut len = 0;

        let meta_num = 3;
        let mut page_size:u32 = 4096;

        if init {
            page_size = unsafe { libc::sysconf(libc::_SC_PAGESIZE) } as u32;
            len = page_size as u64 * (meta_num+1) as u64;

            fh.set_len(len);
            fh.sync_all()?;
        } else {
            len = fh.metadata()?.len();
        }

        let mut data = unsafe {
            MmapOptions::new().map_mut(fh)?
        };

        let header = LowHeader::load(&data);

        if init {
            header.init(page_size as u32, meta_num);
        } else if header.check() {
            return Err(io::Error::new(io::ErrorKind::Other, "Invalid file type"));
        }

        let page_num = ((len / header.page_size as u64) + (len % header.page_size as u64 > 0) as u64) as u32;

        let mut mmap = Mmap {
            page_num: page_num,
            page_size: header.page_size,
            meta_num: header.meta_num,
            data: data,
        };

        Ok(mmap)
    }

    fn get_meta_id(&self) -> u16 {
        let mut idx = 0;
        let mut tx_id = 0;

        for i in 0..self.meta_num {
            let m2 = self.load_meta(i);
            if m2.check() {
                if m2.tx_id > tx_id {
                    tx_id = m2.tx_id;
                    idx = i;
                }
            } else {
                m2.sum = 0;
            }
        }

        idx
    }

    fn load_page(&self, page_id: u32) -> *const u8 {
        let f1 = (page_id * self.page_size) as usize;
        &self.data[f1] as *const u8
    }

    fn load_meta<'a>(&self, meta_id: u16) -> &'a mut LowMeta {
        let raw = self.load_page(meta_id as u32 + 1);
        let raw = raw as *mut LowMeta;
        unsafe { &mut (*raw) }
    }

    fn load_node(&self, page_id: u32) -> &mut LowNode {
        let raw = self.load_page(page_id);
        let node = raw as *mut LowNode;
        unsafe { &mut (*node) }
    }

    fn load_leaf(&self, page_id: u32) -> &mut LowLeafNode {
        let raw = self.load_page(page_id);
        let node = raw as *mut LowLeafNode;
        unsafe { &mut (*node) }
    }

    fn load_data(&self, page_id:u32, len:u32) -> &mut [u8] {
        let raw = self.load_page(page_id);
        let data = raw as *mut [u8; U32_MAX as usize];
        unsafe { &mut (*data)[..len as usize] }
    }

    fn node_type(&self, page_id: u32) -> NodeType {
        let raw = self.load_page(page_id);
        unsafe {
            mem::transmute::<_, NodeType>(*raw)
        }
    }

    fn set_data(&self, page_id: u32, data:&[u8]) {
        let raw = self.load_page(page_id);

        let src = &data[0] as *const _ as *const [u8; U32_MAX as usize];

        let dst = raw as *mut [u8; U32_MAX as usize];
        let len = data.len();

        unsafe {
            ptr::copy(src, dst, len);
        }
    }
}

#[derive(Clone)]
struct Meta {
    file: Rc<File>,
    meta_id : u16,
    low : LowMeta,
    free_pages : Vec<LowRange>,
    hold_pages : Vec<LowRange>,
    cache_pages: HashSet<u32>,
    mmap: Rc<Mmap>,
}

impl Meta {
    fn as_mut<'a>(&self) -> &'a mut Meta{
        let t = self as *const _ as *mut Meta;
        unsafe{ &mut (*t) }
    }

    fn init(&mut self) {
        let raw = self.mmap.load_page(self.low.free_page);
        let pages = raw as *const [LowRange; U32_MAX as usize];
        let pages = unsafe{ &(*pages)[..self.low.free_num as usize] };

        for page in pages.iter() {
            self.free_pages.push(page.clone());
        }

        let raw = self.mmap.load_page(self.low.hold_page);
        let pages = raw as *const [LowRange; U32_MAX as usize];
        let pages = unsafe{ &(*pages)[..self.low.hold_num as usize] };

        for page in pages.iter() {
            self.hold_pages.push(page.clone());
        }
    }

    fn flush(&self) {
        self.mmap.data.flush().unwrap();
    }

    fn page_free(&mut self, pid:u32, len:u32) {
        for i in 0..len {
            let has = self.cache_pages.remove(&(pid + i));
            if has {
                self._page_free(pid+i, 1);
            } else {
                self._hold_free(pid+i, 1);
            }
        }

        self._page_free(pid, len);
    }

    fn _page_free(&mut self, pid:u32, len:u32) {
        let page = LowRange{
            from_id : pid,
            end_id : pid + len,
        };

        for i in 0..self.free_pages.len() {
            if page.from_id == self.free_pages[i].end_id {
                self.free_pages[i].end_id = page.end_id;
                return;
            } else if page.end_id == self.free_pages[i].from_id {
                self.free_pages[i].from_id = page.from_id;
                return;
            } else if page.end_id < self.free_pages[i].from_id {
                self.free_pages.insert(i, page);
                return;
            }
        }

        self.free_pages.push(page);
    }

    fn _hold_free(&mut self, pid:u32, len:u32) {
        let page = LowRange{
            from_id : pid,
            end_id : pid + len,
        };

        for i in 0..self.hold_pages.len() {
            if page.from_id == self.hold_pages[i].end_id {
                self.hold_pages[i].end_id = page.end_id;
                return;
            } else if page.end_id == self.hold_pages[i].from_id {
                self.hold_pages[i].from_id = page.from_id;
                return;
            } else if page.end_id < self.hold_pages[i].from_id {
                self.hold_pages.insert(i, page);
                return;
            }
        }

        self.hold_pages.push(page);
    }

    fn page_alloc(&mut self, num:u32) -> u32 {
        let pid = self._page_alloc(num);

        for i in 0..num {
            self.cache_pages.insert(pid + i);
        }

        pid
    }

    fn _page_alloc(&mut self, num:u32) -> u32 {
        let mut i = 0;
        while i < self.free_pages.len() {
            let has = self.free_pages[i].end_id - self.free_pages[i].from_id;

            if has > num {
                let out = self.free_pages[i].from_id;
                self.free_pages[i].from_id += num;
                return out;
            } else if has == num {
                let out = self.free_pages[i].from_id;
                self.free_pages.remove(i);
                return out;
            }

            i+=1;
        }

        let (old, end) = self.resize(num);

        self.free_pages.push(LowRange{
            from_id: old + num,
            end_id: end,
        });

        old
    }

    fn resize(&mut self, num:u32) -> (u32, u32) {
        let s1gb:u32 = 1024 * 1024 * 1024;

        let mmap = self.mmap.clone();

        let mut new = mmap.page_num;

        while new - mmap.page_num < num+1 {
            if new as u64 * (mmap.page_size as u64) < s1gb as u64 {
                new *= 2;
            } else {
                new += s1gb / mmap.page_size;
            }
        }

        self.remmap(new);

        (mmap.page_num, new)
    }

    fn remmap(&mut self, num:u32) {
        let mmap = self.mmap.clone();

        let size = mmap.page_size as u64 * num as u64;

        //file resize
        // let fh = OpenOptions::new().read(true).write(true).open(&self.file).unwrap();
        // fh.set_len(size).unwrap();
        // fh.sync_all().unwrap();

        self.file.set_len(size).unwrap();

        //mmap重建
        self.mmap = Rc::new(Mmap::load(&self.file, false).unwrap());
    }
}

#[repr(C)]
#[derive(Hash, Default, Clone, Debug)]
struct LowHeader {
    magic: u32,
    version: u32,
    page_size: u32,
    meta_num: u16,
    sum: u64,
}

impl LowHeader {
    fn load(raw:&[u8]) -> &mut LowHeader {
        let h = &raw[0] as *const _ as *mut LowHeader;
        unsafe { &mut (*h) }
    }

    fn sum_fnv(&self) -> u64 {
        let mut d = self.clone();
        d.sum = 0;

        let mut h : FnvHasher = Default::default();
        d.hash(&mut h);
        h.finish()
    }

    fn check(&self) -> bool {
        if self.sum == 0 || self.magic != MAGIC_NUM || self.version != 1 {
            return true;
        }

        self.sum_fnv() != self.sum
    }

    fn init(&mut self, page_size: u32, meta_num:u16) {
        self.magic = MAGIC_NUM;
        self.version = 1;
        self.page_size = page_size;
        self.meta_num = meta_num;
        self.sum = self.sum_fnv();
    }
}

#[derive(Default, Clone, Debug)]
struct LowLeafItem {
    //key的结束偏移位置，启始点上一个Item的key_offset_end
    key_offset_end: u16,
    //data的结束偏移位置，启始点为key_offset_end
    data_offset_end: u32,
}

#[repr(C)]
#[derive(Default, Clone, Debug)]
struct LowLeafNode{
    node_type: NodeType,
    num: u16,
    items: [LowLeafItem;1],
    //越界访问
    //data:[u8;1]
}

fn leaf_node_size() -> usize {
    let size1 = mem::size_of::<LowLeafNode>();
    let size2 = mem::size_of::<LowLeafItem>();
    size1 - size2
}

impl LowLeafNode {
    fn items_size(&self) -> usize {
        let size = mem::size_of::<LowLeafItem>();
        size * self.num as usize
    }

    fn size(&self) -> usize {
        let size = mem::size_of::<LowLeafNode>();
        let size2 = mem::size_of::<LowLeafItem>();
        let size = size + size2 * self.num as usize;
        let size = size - size2;
        let size = size + self.low_item(self.num-1).data_offset_end as usize;
        size
    }

    fn item_size(&self, i:u16) -> usize {
        let size = self.low_item(i).data_offset_end as usize - self.item_offset(i);
        let size2 = mem::size_of::<LowLeafItem>();
        size + size2
    }

    fn low_item(&self, i:u16) -> &mut LowLeafItem {
        let item = &self.items[0] as *const _ as *mut LowLeafItem;

        unsafe {
            let item = item.offset(i as isize);
            &mut(*item)
        }
    }

    fn item_offset(&self, i:u16) -> usize {
        if i > 0 {
            self.low_item(i-1).data_offset_end as usize
        } else {
            self.items_size()
        }
    }

    fn item_key(&self, i:u16) -> &[u8] {
        let offset = self.item_offset(i);
        let end = self.low_item(i).key_offset_end as usize;

        let out = &self.items[0] as *const _ as *const [u8; U16_MAX as usize];

        unsafe { &(*out)[offset..end] }
    }

    fn item_data(&self, i:u16) -> &[u8] {
        let low = self.low_item(i);

        let n1 = low.key_offset_end as usize;
        let n2 = low.data_offset_end as usize;

        let out = &self.items[0] as *const _ as *const [u8; U32_MAX as usize];

        unsafe { &(*out)[n1..n2] }
    }

    fn seek(&self, key:&[u8]) -> Result<usize,usize> {
        let f = |i| {
            let key2 = self.item_key(i as u16);
            str_cmp(key2, key)
        };

        sorted_search_by(self.num as usize, f)
    }

    fn get(&self, key:&[u8]) -> Option<(*const u8, usize)> {
        println!("leaf get {:?}", key);
        match self.seek(key) {
            Ok(i) => {
                let d = self.item_data(i as u16);
                println!("test {:?}", d);
                if d.len() == 0 {
                    return Some((0 as *const u8, 0));
                }

                let ptr = &d[0] as *const u8;

                Some((ptr, d.len()))
            },
            Err(_) => None,
        }
    }

    fn del_to(&self, tx:Rc<RefCell<Meta>>, key:&[u8]) -> Option<u32> {
        match self.seek(key) {
            Ok(i) => {
                //己空请上级删除自己
                if self.num == 1 {
                    return Some(0);
                }

                let mut rtx = tx.borrow_mut();

                let new_pid = rtx.page_alloc(1);

                let leaf = rtx.mmap.load_leaf(new_pid);
                leaf.node_type = NodeType::LeafNode;
                leaf.num = self.num - 1;

                let sizeof_item = mem::size_of::<LowLeafItem>();
                let len1 = sizeof_item * i;

                let src2 = sizeof_item * (i+1);
                let dst2 = len1;
                let len2 = self.item_offset(i as u16) - src2;

                let src3 = self.low_item(i as u16).data_offset_end as usize;
                let dst3 = len1 + len2;
                let len3 = self.low_item(self.num-1).data_offset_end as usize - src3;

                if len1 > 0 {
                    let src = &self.items[0] as *const _ as *const u8;
                    let dst = &leaf.items[0] as *const _ as *mut u8;
                    unsafe { ptr::copy_nonoverlapping(src, dst, len1); }
                }

                {
                    let src = &self.items[src2] as *const _ as *const u8;
                    let dst = &leaf.items[dst2] as *const _ as *mut u8;
                    unsafe { ptr::copy_nonoverlapping(src, dst, len2); }
                }

                if len3 > 0 {
                    let src = &self.items[src3] as *const _ as *const u8;
                    let dst = &leaf.items[dst3] as *const _ as *mut u8;
                    unsafe { ptr::copy_nonoverlapping(src, dst, len3); }
                }

                Some(new_pid)
            },
            Err(_) => None,
        }
    }

    fn set_to(&self, tx:Rc<RefCell<Meta>>, key:&[u8], val:&[u8]) -> Vec<u32> {
        let new_i_len = key.len() + val.len();
        let sizeof_leaf = mem::size_of::<LowLeafNode>();
        let sizeof_item = mem::size_of::<LowLeafItem>();
        let seek = self.seek(key);

        let mut rtx = tx.borrow().as_mut();
        //强制持有老内存，防止过早回收
        let mmap = rtx.mmap.clone();

        let page_size = mmap.page_size;

        let mut pages: Vec<(usize, Vec<isize>)> = vec![(0, Vec::new())];

        let (idx, find) = match seek {
            Ok(i) => (i, true),
            Err(i) => (i, false),
        };

        let mut fx = |i:isize, s:usize| {
            if pages.last().unwrap().0 + sizeof_item + s > page_size as usize {
                pages.push((sizeof_leaf - sizeof_item, Vec::new()));
            }

            let mut page = pages.last_mut().unwrap();
            page.0 += sizeof_item + s;
            page.1.push(i);
        };

        for i in 0..self.num {
            if i as usize == idx {
                fx(-1, new_i_len);

                if find {
                    continue;
                }
            }

            let len = self.low_item(i).data_offset_end as usize - self.item_offset(i);
            fx(i as isize, len);
        }

        if idx == self.num as usize {
            fx(-1, new_i_len);
        }

        if pages.len() > 1 {
            let half_size = page_size as usize / pages.len();

            let prev = pages.len() - 2;
            let last = pages.len() - 1;

            while pages[last].0 < half_size {
                let size = {
                    let i = pages[prev].1.last().unwrap();
                    let size = if *i == -1 {
                        new_i_len
                    } else {
                         self.low_item(*i as u16).data_offset_end as usize - self.item_offset(*i as u16)
                    };
                    let size = size + sizeof_item;
                    size
                };

                if pages[last].0 + size > half_size {
                    break;
                } else {
                    let i = pages[prev].1.pop().unwrap();
                    pages[prev].0 -= size;
                    pages[last].0 += size;
                    pages[last].1.insert(0, i);
                }
            }
        }

        let mut new_pids : Vec<u32> = Vec::new();

        for page in pages.iter() {
            let num = {
                let page = (page.0 / page_size as usize) + (page.0 % page_size as usize > 0) as usize;
                let page = if page < 1 { 1 } else { page };
                page
            };

            let pid = rtx.page_alloc(num as u32);
            new_pids.push(pid);

            let node = rtx.mmap.load_leaf(pid);
            node.node_type = NodeType::LeafNode;
            node.num = page.1.len() as u16;

            let mut new_id = 0;

            for id in page.1.iter() {
                let (src_k, src_v) = if *id == -1 {
                    (key, val)
                } else {
                    ( self.item_key(*id as u16), self.item_data(*id as u16) )
                };

                let key_len = src_k.len();
                let val_len = src_v.len();

                let mut new = node.low_item(new_id);
                new.key_offset_end = (node.item_offset(new_id) + key_len) as u16;
                new.data_offset_end = new.key_offset_end as u32 + val_len as u32;

                let dst_k = node.item_key(new_id);
                let src_k = &src_k[0] as *const u8;
                let dst_k = &dst_k[0] as *const _ as *mut u8;

                unsafe { ptr::copy_nonoverlapping(src_k, dst_k, key_len); }

                if val_len > 0 {
                    let dst_v = node.item_data(new_id);
                    let src_v = &src_v[0] as *const u8;
                    let dst_v = &dst_v[0] as *const _ as *mut u8;

                    unsafe { ptr::copy_nonoverlapping(src_v, dst_v, val_len); }
                }

                new_id += 1;
            }
        }

        new_pids
    }
}

#[derive(Hash, Default, Copy, Clone, Debug)]
struct LowMeta {
    tx_id: u64,
    root_page: u32,
    free_page: u32,
    free_num: u32,
    hold_page: u32,
    hold_num:u32,
    auto_id: u64,
    sum: u64,
}

impl LowMeta {
    fn sum_fnv(&self) -> u64 {
        let mut d = self.clone();
        d.sum = 0;

        let mut h : FnvHasher = Default::default();
        d.hash(&mut h);
        h.finish()
    }

    fn check(&self) -> bool {
        if self.sum == 0 {
            return false;
        }

        let res = self.sum_fnv();

        res == self.sum
    }
}

#[derive(Default, Clone, Debug)]
struct LowRange{
    from_id: u32,
    end_id: u32,
}

#[repr(C)]
#[derive(Default, Clone, Debug)]
struct LowFreePage{
    num: u32,
    pages: [LowRange;1],
}

type Subkey = [u8; 8];

#[derive(Default, Clone, Debug)]
struct LowItem {
    key: Subkey,
    child_id: u32,
}

#[repr(C)]
#[derive(Default, Clone, Debug)]
struct LowNode{
    node_type: NodeType,
    key_depth: u8,
    num: u16,
    items: [LowItem; 1],
}

#[derive(Clone, Debug)]
enum NodeType {
    Node = 1,
    LeafNode = 2,
}

impl Default for NodeType {
    fn default() -> Self { NodeType::Node }
}

impl LowNode {
    fn init(&mut self, depth:u8) {
        self.node_type = NodeType::Node;
        self.key_depth = depth;
        self.num = 0;
    }

    fn item(&self, i: u16) -> &mut LowItem {
        let item = &self.items[0] as *const _ as *mut LowItem;

        unsafe {
            let item = item.offset(i as isize);
            &mut (*item)
        }
    }

    fn seek(&self, key:&[u8]) -> Result<usize,usize> {
        let key2 = subkey(key, self.key_depth);

        let f = |i:usize| {
            let key3 = self.item(i as u16).key;
            str_cmp(&key3, &key2)
        };

        sorted_search_by(self.num as usize, f)
    }

    fn key_depth_check(&mut self, tx:Rc<RefCell<Meta>>) {
        let mut items = Vec::with_capacity(self.num as usize);
        for i in 0..self.num as usize {
            items.push(self.item(i as u16).clone());
        }

        key_depth_check(tx, &mut items, self.key_depth);

        self.num = 0;
        for i in 0..items.len() {
            self.add_item(items[i].clone());
        }
    }

    fn seek_leaf(&self, tx:Rc<RefCell<Meta>>, key:&[u8]) -> Option<u32> {
        let res = self.seek(key);

        println!("seek_leaf depth:{:?} {:?} key:{:?}", self.key_depth, res, subkey(key, self.key_depth));

        let mut items = Vec::with_capacity(self.num as usize);
        for i in 0..self.num as usize {
            items.push(self.item(i as u16).clone());
        }
        // println!("seek_leaf items {:?}", items);

        match res {
            Ok(i) => {
                let rtx = tx.borrow();

                let pid = self.item(i as u16).child_id;

                if let NodeType::Node = rtx.mmap.node_type(pid) {
                    let node = rtx.mmap.load_node(pid);
                    node.seek_leaf(tx.clone(), key)
                } else {
                    Some(pid)
                }
            },
            Err(_) => None,
        }
    }

    fn has(&self, tx:Rc<RefCell<Meta>>, key:&[u8]) -> bool {
        match self.seek_leaf(tx.clone(), key) {
            Some(pid) => {
                let rtx = tx.borrow();

                let leaf = rtx.mmap.load_leaf(pid);
                let res = leaf.get(key);

                if let Some(_) = res {
                    true
                } else {
                    false
                }
            },
            None => false,
        }
    }

    fn get(&self, tx:Rc<RefCell<Meta>>, key:&[u8]) -> Option<(*const u8, usize)> {
        let pid = self.seek_leaf(tx.clone(), key)?;

        let rtx = tx.borrow();
        let leaf = rtx.mmap.load_leaf(pid);
        leaf.get(key)
    }

    fn del_to(&self, tx:Rc<RefCell<Meta>>, key:&[u8]) -> Option<u32> {
        match self.seek(key) {
            Ok(i) => {
                let mut rtx = tx.borrow_mut();

                let child_id = self.item(i as u16).child_id;
                let mut page_num = 1;

                let res = if let NodeType::Node = rtx.mmap.node_type(child_id) {
                    let node = rtx.mmap.load_node(child_id);
                    node.del_to(tx.clone(), key)
                } else {
                    let leaf = rtx.mmap.load_leaf(child_id);
                    let size = leaf.size() as u32;
                    page_num = (size / rtx.mmap.page_size) + (size % rtx.mmap.page_size > 0) as u32;

                    leaf.del_to(tx.clone(), key)
                };

                let new_child_id = match res {
                    Some(pid) => pid,
                    None => return None,
                };

                if new_child_id == 0 {
                    //子节点己为空，删除它
                    rtx.page_free(child_id, page_num);

                    //自己也空了，请上级删除自己
                    if self.num == 1 {
                        return Some(0);
                    }
                }

                let new_pid = rtx.page_alloc(1);
                let node = rtx.mmap.load_node(new_pid);
                node.init(self.key_depth);
                for i in 0..self.num {
                    let mut item = self.item(i).clone();
                    if item.child_id == child_id {
                        if new_child_id == 0 {
                            continue;
                        }

                        item.child_id = new_child_id;
                    }

                    node.add_item(item);
                }

                Some(new_pid)
            },
            Err(_) => None,
        }
    }

    fn set_to(&self, tx:Rc<RefCell<Meta>>, key:&[u8], val:&[u8]) -> Vec<u32> {
        let (idx, find) = match self.seek(key) {
            Ok(i) => (i, true),
            Err(i) => (i, false),
        };

        let mut rtx = tx.borrow().as_mut();
        let mmap = rtx.mmap.clone();

        let mut items:Vec<LowItem> = Vec::with_capacity(self.num as usize + 1);

        for i in 0..self.num as usize {
            items.push(self.item(i as u16).clone());
        }

        let pids = if find {
            let pid = self.item(idx as u16).child_id;
            items.remove(idx);

            if let NodeType::Node = rtx.mmap.node_type(pid) {
                let node = rtx.mmap.load_node(pid);
                node.set_to(tx.clone(), key, val)
            } else {
                let leaf = rtx.mmap.load_leaf(pid);
                leaf.set_to(tx.clone(), key, val)
            }
        } else {
            let leaf:LowLeafNode = Default::default();
            leaf.set_to(tx.clone(), key, val)
        };

        let mut items2 = Vec::new();
        for pid in pids {
            items2.push(LowItem{
                key: minkey(tx.clone(), pid, self.key_depth),
                child_id: pid,
            });
        }

        //进行冲突检测
        key_depth_check(tx.clone(), &mut items2, self.key_depth);

        let mut items3 = items.split_off(idx);
        items.append(&mut items2);
        items.append(&mut items3);

        //将items写入到新页面
        let size = mem::size_of::<LowItem>();
        let size = size * items.len();
        let size = size as u32;

        let p_num = (size / mmap.page_size) + (size % mmap.page_size > 0) as u32;
        let i_num = items.len() / p_num as usize;

        let mut new_ids: Vec<u32> = Vec::new();
        let mut i = 0;

        for _ in 0..p_num {
            let pid = rtx.page_alloc(1);
            new_ids.push(pid);
            let node = rtx.mmap.load_node(pid);
            node.init(self.key_depth);

            for _ in 0..i_num {
                node.add_item(items[i].clone());
                i+=1;
            }
        }

        while i < items.len() {
            let pid = new_ids.last().unwrap();
            let node = rtx.mmap.load_node(*pid);
            node.add_item(items[i].clone());
            i+=1;
        }

        new_ids
    }

    fn add_item(&mut self, item:LowItem) {
        let mut t = self.item(self.num);
        t.key = item.key;
        t.child_id = item.child_id;
        self.num += 1;
    }

    fn add_child(&mut self, tx:Rc<RefCell<Meta>>, child:u32) {
        self.add_item(LowItem{
            key: minkey(tx, child, self.key_depth),
            child_id: child,
        });
    }

}

fn minkey(tx:Rc<RefCell<Meta>>, pid:u32, depth:u8) -> Subkey {
    let rtx = tx.borrow();

    let out = if let NodeType::Node = rtx.mmap.node_type(pid) {
        let node = rtx.mmap.load_node(pid);
        let item = node.item(0);

        if depth == node.key_depth {
            item.key
        } else {
            minkey(tx.clone(), item.child_id, depth)
        }
    } else {
        let key = rtx.mmap.load_leaf(pid).item_key(0);
        subkey(key, depth)
    };

    out
}

fn subkey<'a>(key:&'a [u8], depth:u8) -> Subkey {
    let size = mem::size_of::<Subkey>();
    let oft = depth as usize * size;

    // let mut t:Subkey = Default::default();
    let mut t:Subkey = [0u8; 8];

    let mut i = 0;
    while i < t.len() && i+oft < key.len() {
        t[i] = key[i+oft];
        i+=1;
    }

    t
}

fn key_depth_check(tx:Rc<RefCell<Meta>>, items:&mut Vec<LowItem>, depth:u8) {
    let mut rtx = tx.borrow().as_mut();

    //进行冲突检测
    let mut i = 0;
    while i < items.len() - 1 {
        let key1 = items[i].key;
        let key2 = items[i+1].key;

        if str_cmp(&key1, &key2) != Ordering::Equal {
            i+=1;
            continue;
        }

        println!("find conflict {:?}", items[i].key);

        let pid = rtx.page_alloc(1);
        let node = rtx.mmap.load_node(pid);
        node.init(depth+1);
        node.add_child(tx.clone(), items[i].child_id);

        items[i].child_id = pid;

        let key1 = items[i].key;

        i+=1;
        loop {
            let pid = items[i].child_id;
            node.add_child(tx.clone(), pid);

            items.remove(i);

            if i >= items.len() || str_cmp(&items[i].key, &key1) != Ordering::Equal {
                break;
            }
        }

        node.key_depth_check(tx.clone());
        break;
    }
}

fn sorted_search_by<F>(num:usize, f:F) -> Result<usize,usize>
    where F: Fn(usize)-> Ordering
{
    let mut size = num;
    if size == 0 {
        return Err(0);
    }

    let mut base = 0usize;
    while size > 1 {
        let half = size / 2;
        let mid = base + half;
        // mid is always in [0, size), that means mid is >= 0 and < size.
        // mid >= 0: by definition
        // mid < size: mid = size / 2 + size / 4 + size / 8 ...
        let cmp = f(mid);
        if cmp == Ordering::Equal {
            return Ok(mid);
        }

        base = if cmp == Ordering::Greater { base } else { mid };
        size -= half;
    }

    // base is always in [0, size) because base <= mid.
    let cmp = f(base);
    if cmp == Ordering::Equal {
        Ok(base)
    } else {
        Err(base + (cmp == Ordering::Less) as usize)
    }
}

fn str_cmp(a:&[u8], b:&[u8]) -> Ordering {
    return a.cmp(b);

    let s1 = &a[0] as *const _ as *const libc::c_void;
    let s2 = &b[0] as *const _ as *const libc::c_void;

    let cmp = a.len().cmp(&b.len());
    let len = if Ordering::Less == cmp { a.len() } else { b.len() };

    let rs = unsafe {
        libc::memcmp(s1, s2, len)
    };

    if rs < 0 {
        Ordering::Less
    } else if rs == 0 {
        cmp
    } else {
        Ordering::Greater
    }
}


fn meta_get(tx:Rc<RefCell<Meta>>, key:&[u8]) -> Option<(*const u8, usize)> {
    let meta = tx.borrow();
    let root = meta.low.root_page;
    if root == 0 {
        return None;
    }

    let res = meta.mmap.node_type(root);

    if let NodeType::Node = res {
        let node = meta.mmap.load_node(root);
        node.get(tx.clone(), key)
    } else {
        let node = meta.mmap.load_leaf(root);
        node.get(key)
    }
}

#[cfg(test)]
mod tests {
    use std::ptr;
    use super::*;
    use tempfile;

    #[test]
    fn leaf_set() {
        let file = tempfile::tempfile().unwrap();
        let db = Db::load(file, true).unwrap();

        let tx = db.getTxWriter();

        for i in 0..300 {
            let key = format!("{:->5}", i);
            let key = key.as_bytes();

            tx.set(key, key);
        }

        tx.commit();

        let key = format!("{:->5}", 1);
        let key = key.as_bytes();

        println!("key {:?}", key);

        let ret = tx.get(key);
        assert_eq!(ret, Some(key));

        // let key = format!("{:->10}", 99);
        // let key = key.as_bytes();

        // let ret = tx.get(key);
        // assert_eq!(ret, Some(key));
    }

    // #[test]
    // fn leaf_set_to3() {
        // let val = "12345678901234567890".as_bytes();

        // let page_size = 4096;
        // let mut raws : Vec<Vec<u8>> = Vec::new();
        // let mut raw:Vec<u8> = Vec::with_capacity(page_size);
        // raw.resize(page_size, 0);
        // raws.push(raw);

        // for i in 0..99 {
            // let mut pid = 1;

            // let leaf = &raws[0][0] as *const _ as *mut LowLeafNode;
            // let leaf = unsafe{ &(*leaf) };

            // let key = format!("{:->10}", i);
            // let key = key.as_bytes();

            // leaf.set_to(key, val, &mut |x| {
                // if raws.len() < pid+1 {
                    // let mut raw:Vec<u8> = Vec::with_capacity(page_size * x);
                    // raw.resize(page_size*x, 0);
                    // raws.push(raw);
                // }

                // let p = &raws[pid][0] as *const u8;
                // pid += 1;
                // p
            // }, 4096);

            // let src = &raws[1][0] as *const u8;
            // let dst = &raws[0][0] as *const _ as *mut u8;
            // unsafe {
                // ptr::copy_nonoverlapping(src, dst, 4096);
            // }

            // if pid > 2 {
                // break;
            // }
        // }

        // {
            // let leaf = &raws[0][0] as *const _ as *mut LowLeafNode;
            // let leaf = unsafe{ &(*leaf) };

            // let key = format!("{:->10}", 80);
            // let key = key.as_bytes();

            // let mut val:Vec<u8> = Vec::with_capacity(page_size + 100);
            // val.resize(page_size + 100, 0);

            // let mut pid = 1;
            // leaf.set_to(key, &val, &mut |x| {
                // if raws.len() < pid+1 {
                    // let mut raw:Vec<u8> = Vec::with_capacity(page_size * x);
                    // raw.resize(page_size*x, 0);
                    // raws.push(raw);
                // }

                // let p = &raws[pid][0] as *const u8;
                // pid += 1;
                // p
            // }, 4096);
        // }

        // let key = format!("{:->10}", 1);
        // let key = key.as_bytes();
        // let new = &raws[1][0] as *const _ as *mut LowLeafNode;
        // let new = unsafe{ &(*new) };
        // let ret = new.find(key);
        // assert_eq!(ret, Some(val));

        // let key = format!("{:->10}", 98);
        // let key = key.as_bytes();
        // let new = &raws[3][0] as *const _ as *mut LowLeafNode;
        // let new = unsafe{ &(*new) };
        // let ret = new.find(key);
        // assert_eq!(ret, Some(val));
    // }

}
