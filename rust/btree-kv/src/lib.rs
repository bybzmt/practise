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
use std::collections::HashMap;
// use std::io::prelude::*;

use fnv::FnvHasher;
use memmap::{MmapMut, MmapOptions};

// mod db;
// pub use db::DB;

const MAGIC_NUM : u32 = 12349872;

struct Db {
    meta:Rc<Meta>,
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
            mmap: mmap.clone(),
        };

        meta.init();

        let db = Db{
            meta: Rc::new(meta),
        };

        Ok(db)
    }

    fn getTxReader(&self) -> TxReader {
        let meta = self.meta.as_ref().clone();
        let meta = Rc::new(meta);

        let node = load_inode(meta.clone(), meta.low.root_page);

        TxReader{
            meta:meta,
            root: node,
        }
    }

    fn getTxWriter(&self) -> TxWriter {
        let mut meta = self.meta.as_ref().clone();
        let meta = Rc::new(meta);

        let node = load_inode(meta.clone(), meta.low.root_page);

        TxWriter{
            meta:meta,
            root: node,
        }
    }
}

struct TxReader {
    meta:Rc<Meta>,
    root: Rc<dyn Inode>,
}

impl TxReader {
    fn get(&self, key:&[u8]) -> Option<Rc<Vec<u8>>> {
        self.root.get(key)
    }
}

struct TxWriter {
    meta:Rc<Meta>,
    root: Rc<dyn Inode>,
}

impl TxWriter {
    fn get(&self, key:&[u8]) -> Option<Rc<Vec<u8>>> {
        self.root.get(key)
    }

    fn set(&self, key:&[u8], val:&[u8]) {
        self.root.set(key, val)
    }

    fn del(&self, key:&[u8]) -> bool {
        self.root.del(key)
    }

    fn balance(&mut self) {
        let root = balance(self.meta.clone(), self.root.clone());

        self.root = if root.len() == 1 {
            root[0].clone()
        } else {
            Rc::new(Node::new(self.meta.clone(), root))
        };

        self.meta.as_mut().low.root_page = self.root.pid();
    }

    fn commit(&mut self) {
        self.balance();

        self.root.sync();

        //写入元信息
        self.meta.sync();
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

    fn load_range(&self, pid:u32, num:u32) -> &mut [LowRange] {
        let raw = self.load_page(pid);
        let pages = raw as *mut [LowRange; U32_MAX as usize];
        unsafe{ &mut (*pages)[..num as usize] }
    }

    fn load_page(&self, page_id: u32) -> *const u8 {
        let f1 = (page_id * self.page_size) as usize;
        &self.data[f1] as *const u8
    }

    fn load_meta(&self, meta_id: u16) -> &mut LowMeta {
        let raw = self.load_page(meta_id as u32 + 1);
        let raw = raw as *mut LowMeta;
        unsafe { &mut (*raw) }
    }

    fn load_node(&self, page_id: u32) -> &mut LowNode {
        let raw = self.load_page(page_id);
        let node = raw as *mut LowNode;
        unsafe { &mut (*node) }
    }

    fn load_leaf(&self, page_id: u32) -> &mut LowLeaf {
        let raw = self.load_page(page_id);
        let node = raw as *mut LowLeaf;
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

enum Typ {
    Pid,
    Node,
    NodeItem,
    Leaf,
    LeafItem,
}

trait Inode {
    fn typ(&self) -> Typ;
    fn pid(&self) -> u32 { panic!("panic"); }
    fn sync(&self) { panic!("panic"); }
    fn get(&self, key:&[u8]) -> Option<Rc<Vec<u8>>>
    { panic!("panic"); }
    fn set(&self, key:&[u8], val:&[u8])
    { panic!("panic"); }
    fn del(&self, key:&[u8]) -> bool
    { panic!("panic"); }
    fn has(&self, key:&[u8]) -> bool
    { panic!("panic"); }
    fn key(&self) -> Rc<Vec<u8>>
    { panic!("panic"); }
    fn val(&self) -> Rc<Vec<u8>>
    { panic!("panic"); }
    fn empty(&self) -> bool
    { panic!("panic"); }
    fn balance(&self) -> Vec<Rc<dyn Inode>>
    { panic!("panic"); }
}

struct NodeItem{
    pid:u32,
    key:Rc<Vec<u8>>,
}

impl Inode for NodeItem {
    fn typ(&self) -> Typ { Typ::NodeItem }
    fn pid(&self) -> u32 { self.pid }
    fn key(&self) -> Rc<Vec<u8>> { self.key.clone() }
    fn empty(&self) -> bool  { false }
    fn sync(&self) {}
}

struct Node {
    tx: Rc<Meta>,
    dirty: bool,
    pid:u32,
    origin: bool,
    key_depth: u8,
    childs: Vec<Rc<dyn Inode>>,
}

struct Leaf {
    pid : u32,
    dirty: bool,
    origin: bool,
    tx: Rc<Meta>,
    items: Vec<LeafItem>,
}

#[derive(Clone, Debug)]
struct LeafItem{
    key:Rc<Vec<u8>>,
    val:Rc<Vec<u8>>,
}

impl LeafItem {
    fn size(&self) -> usize {
        self.key.len() + self.val.len() + 2
    }
}

impl Node {
    fn as_mut(&self) -> &mut Node{
        let t = self as *const _ as *mut _;
        unsafe{ &mut (*t) }
    }

    fn new(tx: Rc<Meta>, childs: Vec<Rc<dyn Inode>>) -> Node {
        Node {
            tx:tx,
            dirty: true,
            pid: 0,
            key_depth: 0,
            origin: false,
            childs: childs,
        }
    }

    fn init(&mut self) {
        self.dirty = false;
        self.origin = true;

        let low = self.tx.mmap.load_node(self.pid);
        self.childs = low.load();
    }

    fn seek(&self, key:&[u8]) -> (bool,usize) {
        let key2 = subkey(key, self.key_depth);

        let f = |i:usize| {
            let key3 = &self.childs[i].key();
            str_cmp(&key3, &key2)
        };

        sorted_search_by(self.childs.len(), f)
    }

    fn capacity(&self) -> usize {
        let s1 = mem::size_of::<LowItem>();
        let s2 = mem::size_of::<LowNode>();
        let page_size = self.tx.mmap.page_size as usize;
        page_size - (s2 - s1)
    }

    fn size(&self) -> usize {
        let s1 = mem::size_of::<LowItem>();
        let s2 = mem::size_of::<LowNode>();
        let s3 = s1 * self.childs.len();
        s2 - s1 + s3
    }

    fn child(&self, i:usize) -> Rc<dyn Inode> {
        let typ = self.childs[i].typ();

        if let Typ::NodeItem = typ {
            let tmp = load_inode(self.tx.clone(), self.childs[i].pid());
            self.as_mut().childs[i] = tmp;
        }

        self.childs[i].clone()
    }

    fn childs_balance(&self) {
        let mut childs = Vec::new();

        for child in self.childs.iter() {
            let items = balance(self.tx.clone(), child.clone());

            for tmp in items.iter() {
                childs.push(tmp.clone());
            }
        }

        self.as_mut().childs = childs;
    }
}

impl Inode for Node {
    fn typ(&self) -> Typ { Typ::Node }
    fn pid(&self) -> u32 { self.pid }
    fn empty(&self) -> bool { self.childs.len() == 0 }

    fn has(&self, key:&[u8]) -> bool {
        let (_, i) = self.seek(key);

        if self.childs.get(i).is_none() {
            return false;
        }

        self.child(i).has(key)
    }

    fn get(&self, key:&[u8]) -> Option<Rc<Vec<u8>>> {
        let (_, i) = self.seek(key);

        if self.childs.get(i).is_none() {
            return None;
        }

        self.child(i).get(key)
    }

    fn del(&self, key:&[u8]) -> bool {
        let (_, i) = self.seek(key);

        if self.childs.get(i).is_none() {
            return false;
        }

        let ok = self.childs[i].del(key);
        if ok {
            self.as_mut().dirty = true;
        }
        ok
    }

    fn set(&self, key:&[u8], val:&[u8]) {
        let (_, i) = self.seek(key);

        self.as_mut().dirty = true;

        if self.childs.get(i).is_none() {
            let tmp = Leaf::new(self.tx.clone(), vec![]);
            tmp.set(key, val);

            self.as_mut().childs.insert(i, Rc::new(tmp));
        } else {
            self.child(i).set(key, val);
        }
    }

    fn key(&self) -> Rc<Vec<u8>> {
        if self.childs.len() == 0 {
            return Rc::new(vec![]);
        }
        self.childs[0].key()
    }

    fn balance(&self) -> Vec<Rc<dyn Inode>> {
        if self.empty() {
            let this = self.as_mut();

            if self.pid != 0 {
                if self.origin {
                    self.tx.page_hold(self.pid, 1);
                } else {
                    self.tx.page_free(self.pid, 1);
                }
                this.pid = 0;
            }

            this.dirty = false;
        }

        if !self.dirty {
            return vec![];
        }

        self.childs_balance();

        let s1 = self.capacity();
        let s2 = self.size();

        //页数, 每页个数
        let page = (s2 / s1) + (s2 % s1 > 0) as usize;
        let num = self.childs.len() / page;

        let mut out:Vec<Rc<dyn Inode>> = vec![];

        if self.childs.len() > num {
            let this = self.as_mut();

            let mut items = this.childs.split_off(num);
            while items.len() > num {
                let tmp = items.split_off(num);

                let node = Node::new(self.tx.clone(), items.clone());
                out.push(Rc::new(node));

                items = tmp;
            }

            let node = Node::new(self.tx.clone(), items.clone());
            out.push(Rc::new(node));
        }

        out
    }

    fn sync(&self) {
        if !self.dirty {
            return;
        }

        for item in self.childs.iter() {
            item.sync();
        }

        let this = self.as_mut();
        this.dirty = false;

        if self.origin {
            self.tx.page_hold(self.pid, 1);
            this.origin = false;
            this.pid = 0;
        }

        if this.pid == 0 {
            this.pid = self.tx.page_alloc(1);
        }

        let low = self.tx.mmap.load_node(self.pid);
        low.init(self.key_depth);
        low.write(&self.childs);
    }
}

impl Leaf {
    fn as_mut(&self) -> &mut Leaf{
        let t = self as *const _ as *mut _;
        unsafe{ &mut(*t) }
    }

    fn new(tx: Rc<Meta>, childs: Vec<LeafItem>) -> Leaf {
        Leaf {
            tx:tx,
            dirty: true,
            pid: 0,
            origin: false,
            items: childs,
        }
    }

    fn init(&mut self) {
        self.dirty = false;
        self.origin = true;

        let low = self.tx.mmap.load_leaf(self.pid);
        self.items = low.load();
    }

    fn seek(&self, key:&[u8]) -> (bool,usize) {
        let f = |i:usize| {
            let key2 = &self.items[i].key;
            str_cmp(key2, key)
        };

        sorted_search_by(self.items.len(), f)
    }

    fn capacity(&self) -> usize {
        let sizeof_leaf = mem::size_of::<LowLeaf>();
        let page_size = self.tx.mmap.page_size as usize;
        page_size - sizeof_leaf
    }
}

impl Inode for Leaf {
    fn typ(&self) -> Typ { Typ::Leaf }
    fn pid(&self) -> u32 { self.pid }
    fn empty(&self) -> bool { self.items.len() == 0 }

    fn set(&self, key:&[u8], val:&[u8]) {
        let tmp = LeafItem{
            key:Rc::new(key.to_vec()),
            val:Rc::new(val.to_vec()),
        };
        let (has,i) = self.seek(key);

        let this = self.as_mut();
        this.dirty = true;

        if has {
            this.items[i] = tmp;
        } else {
            this.items.insert(i, tmp);
        }
    }

    fn get(&self, key:&[u8]) -> Option<Rc<Vec<u8>>> {
        let (has,i) = self.seek(key);

        if has {
            Some(self.items[i].val.clone())
        } else {
            None
        }
    }

    fn has(&self, key:&[u8]) -> bool {
        let (has,_) = self.seek(key);
        has
    }

    fn del(&self, key:&[u8]) -> bool {
        let (has,i) = self.seek(key);

        if has {
            let this = self.as_mut();
            this.dirty = true;
            this.items.remove(i);

            true
        } else {
            false
        }
    }

    fn key(&self) -> Rc<Vec<u8>> {
        if self.items.len() == 0 {
            return Rc::new(vec![]);
        }
        self.items[0].key.clone()
    }

    fn balance(&self) -> Vec<Rc<dyn Inode>> {
        if self.empty() {
            let this = self.as_mut();

            if self.pid != 0 {
                if self.origin {
                    self.tx.page_hold(self.pid, 1);
                } else {
                    self.tx.page_free(self.pid, 1);
                }
                this.pid = 0;
            }

            this.dirty = false;
        }

        if !self.dirty {
            return vec![];
        }

        let mut size = 0;
        for item in self.items.iter() {
            size += item.size();
        }

        let s1 = self.capacity();
        let num = (size / s1) + (size % s1 > 0) as usize;
        let s2 = size / num;

        size = 0;
        let mut pages = vec![vec![]];
        for item in self.items.iter() {
            size += item.size();
            pages.last_mut().unwrap().push(item.clone());

            if size > s2 {
                size = 0;
                pages.push(Vec::new());
            }
        }

        self.as_mut().items = pages[0].clone();

        let mut out:Vec<Rc<dyn Inode>> = Vec::new();

        for i in 1..pages.len() {
            let mut leaf = Leaf::new(self.tx.clone(), pages[i].clone());
            leaf.dirty = true;
            out.push(Rc::new(leaf));
        }

        out
    }

    fn sync(&self) {
        if !self.dirty {
            return;
        }

        let this = self.as_mut();

        if self.origin {
            self.tx.page_hold(self.pid, 1);
            this.origin = false;
            this.pid = 0;
        }

        if this.pid == 0 {
            this.pid = self.tx.page_alloc(1);
        }

        let low = self.tx.mmap.load_leaf(self.pid);
        low.write(&self.items);

        this.dirty = false;
    }
}

fn load_inode(tx:Rc<Meta>, pid:u32) -> Rc<dyn Inode> {
    if let NodeType::Node = tx.mmap.node_type(pid) {
        let mut node = Node::new(tx.clone(), vec![]);
        node.pid = pid;
        node.init();
        Rc::new(node)
    } else {
        let mut leaf = Leaf::new(tx.clone(), vec![]);
        leaf.pid = pid;
        leaf.init();
        Rc::new(leaf)
    }
}

fn balance(tx:Rc<Meta>, node:Rc<dyn Inode>) -> Vec<Rc<dyn Inode>> {
    let mut out: Vec<Rc<dyn Inode>> = vec![];

    let mut items = node.balance();

    if !node.empty() {
        out.push(node);
        out.append(&mut items);
    }

    out
}

#[derive(Clone)]
struct Meta {
    file: Rc<File>,
    meta_id : u16,
    low : LowMeta,
    free_pages : Vec<LowRange>,
    hold_pages : Vec<LowRange>,
    mmap: Rc<Mmap>,
}

impl Meta {
    fn as_mut<'a>(&self) -> &'a mut Meta{
        let t = self as *const _ as *mut Meta;
        unsafe{ &mut (*t) }
    }

    fn init(&mut self) {
        let items = self.mmap.load_range(self.low.free_page, self.low.free_num);
        self.free_pages = items.to_vec();

        let items = self.mmap.load_range(self.low.hold_page, self.low.hold_num);
        self.hold_pages = items.to_vec();
    }

    fn sync(&self) {
        let sizeof_range = mem::size_of::<LowRange>();
        let psize = self.mmap.page_size as usize;

        if self.low.free_page != 0 {
            let s1 = self.low.free_num as usize * sizeof_range;
            let num = (s1 / psize) + (s1 % psize > 0) as usize;
            self.page_hold(self.low.free_page, num as u32);
        }

        if self.low.hold_page != 0 {
            let s1 = self.low.hold_num as usize * sizeof_range;
            let num = (s1 / psize) + (s1 % psize > 0) as usize;
            self.page_hold(self.low.hold_page, num as u32);
        }

        let this = self.as_mut();

        let dst_meta_id = (self.meta_id + 1) % self.mmap.meta_num;
        let mut dst_meta = self.mmap.load_meta(dst_meta_id);

        //将目标版本保持的页添加到空闲表
        if dst_meta.check() {
            let holds = self.mmap.load_range(dst_meta.hold_page, dst_meta.hold_num);
            for hold in holds.iter() {
                this.page_free(hold.from_id, hold.end_id - hold.from_id);
            }
        }

        //保存当前hold
        this.low.hold_page = if self.hold_pages.len() > 0 {
            let s1 = self.hold_pages.len() * sizeof_range;
            let pnum = (s1 / psize) + (s1 % psize > 0) as usize;

            let dst_id = self.page_alloc(pnum as u32);
            let dst = self.mmap.load_range(dst_id, self.hold_pages.len() as u32);
            dst.copy_from_slice(&self.hold_pages);
            dst_id
        } else {
            0
        };

        //保存当前free
        this.low.free_page = if self.free_pages.len() > 0 {
            let s1 = self.free_pages.len() * sizeof_range;
            let pnum = (s1 / psize) + (s1 % psize > 0) as usize;

            let dst_id = self.page_alloc(pnum as u32);
            let dst = self.mmap.load_range(dst_id, self.free_pages.len() as u32);
            dst.copy_from_slice(&self.free_pages);
            dst_id
        } else {
            0
        };

        this.low.tx_id += 1;

        //校验码
        this.low.sum = this.low.sum_fnv();

        //先刷入数据
        self.mmap.data.flush().unwrap();

        unsafe {
            ptr::copy(&self.low, dst_meta, 1);
        }

        //再同步元信息
        self.mmap.data.flush().unwrap();
    }

    fn page_free(&self, pid:u32, len:u32) {
        let page = LowRange{
            from_id : pid,
            end_id : pid + len,
        };

        let tx = self.as_mut();
        for i in 0..self.free_pages.len() {
            if page.from_id == self.free_pages[i].end_id {
                tx.free_pages[i].end_id = page.end_id;
                return;
            } else if page.end_id == self.free_pages[i].from_id {
                tx.free_pages[i].from_id = page.from_id;
                return;
            } else if page.end_id < self.free_pages[i].from_id {
                tx.free_pages.insert(i, page);
                return;
            }
        }

        tx.free_pages.push(page);
    }

    fn page_hold(&self, pid:u32, len:u32) {
        let page = LowRange{
            from_id : pid,
            end_id : pid + len,
        };

        let tx = self.as_mut();
        for i in 0..self.hold_pages.len() {
            if page.from_id == self.hold_pages[i].end_id {
                tx.hold_pages[i].end_id = page.end_id;
                return;
            } else if page.end_id == self.hold_pages[i].from_id {
                tx.hold_pages[i].from_id = page.from_id;
                return;
            } else if page.end_id < self.hold_pages[i].from_id {
                tx.hold_pages.insert(i, page);
                return;
            }
        }

        tx.hold_pages.push(page);
    }

    fn page_alloc(&self, num:u32) -> u32 {
        let tx = self.as_mut();

        let mut i = 0;
        while i < self.free_pages.len() {
            let has = self.free_pages[i].end_id - self.free_pages[i].from_id;

            if has > num {
                let out = self.free_pages[i].from_id;
                tx.free_pages[i].from_id += num;
                return out;
            } else if has == num {
                let out = self.free_pages[i].from_id;
                tx.free_pages.remove(i);
                return out;
            }

            i+=1;
        }

        let (old, end) = tx.resize(num);

        tx.free_pages.push(LowRange{
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

        self.file.set_len(size).unwrap();
        // fh.sync_all().unwrap();

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

#[repr(C)]
#[derive(Default, Clone, Debug)]
struct LowLeaf{
    node_type: NodeType,
    items: [u8;1],
}

impl LowLeaf {
    fn as_mut(&self) -> &mut LowLeaf{
        let t = self as *const _ as *mut _;
        unsafe{ &mut(*t) }
    }

    fn raw(&self) -> &mut [u8] {
        let out = &self.items[0] as *const _ as *mut [u8; U32_MAX as usize];
        unsafe { &mut (*out) }
    }

    fn load(&self) -> Vec<LeafItem> {
        let raw = self.raw();

        let mut out = vec![];
        let mut i=0;
        loop {
            let len = raw[i] as usize;
            if len == 0 {
                break;
            }

            let key = raw[i..i+len].to_vec();
            i+=len;

            let len = raw[i] as usize;
            let val = raw[i..i+len].to_vec();
            i+=len;

            out.push(LeafItem{
                key:Rc::new(key),
                val:Rc::new(val),
            });
        }

        out
    }

    fn write(&mut self, items:&[LeafItem]) {
        self.node_type = NodeType::LeafNode;

        let raw = self.raw();

        let mut i=0;
        for item in items.iter() {
            let len = item.key.len();
            raw[i] = len as u8;
            raw[i..i+len].copy_from_slice(&item.key);
            i+=len;

            let len = item.val.len();
            raw[i] = len as u8;
            raw[i..i+len].copy_from_slice(&item.val);
            i+=len;
        }

        raw[i] = 0;
    }
}

#[repr(C)]
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


#[repr(C)]
#[derive(Default, Clone, Copy, Debug)]
struct LowRange{
    from_id: u32,
    end_id: u32,
}

type Subkey = [u8; 8];

#[repr(C)]
#[derive(Default, Clone, Debug)]
struct LowItem {
    child_id: u32,
    key: Subkey,
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
    }

    fn item(&self, i: usize) -> &mut LowItem {
        let item = &self.items[0] as *const _ as *mut LowItem;

        unsafe {
            let item = item.offset(i as isize);
            &mut (*item)
        }
    }

    fn load(&self) -> Vec<Rc<dyn Inode>> {
        let mut out:Vec<Rc<dyn Inode>> = vec![];

        for i in 0..self.num as usize {
            let tmp = self.item(i);
            let r = Rc::new(NodeItem{
                pid: tmp.child_id,
                key: Rc::new(tmp.key.to_vec()),
            });
            out.push(r);
        }

        out
    }

    fn write(&mut self, items:&[Rc<dyn Inode>]) {
        self.num = items.len() as u16;

        for i in 0..items.len() {
            let mut tmp = self.item(i);

            tmp.child_id = items[i].pid();
            let key = items[i].key();

            let len = if tmp.key.len() < key.len() {
                tmp.key.len()
            } else {
                key.len()
            };

            tmp.key = Default::default();
            tmp.key[..len].copy_from_slice(&key[..len]);
        }
    }
}

// fn minkey(tx:Rc<Meta>, pid:u32, depth:u8) -> Subkey {
    // let out = if let NodeType::Node = tx.mmap.node_type(pid) {
        // let node = tx.mmap.load_node(pid);
        // let item = node.item(0);

        // if depth == node.key_depth {
            // item.key
        // } else {
            // minkey(tx.clone(), item.child_id, depth)
        // }
    // } else {
        // let key = tx.mmap.load_leaf(pid).item_key(0);
        // subkey(key, depth)
    // };

    // out
// }

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

fn sorted_search_by<F>(num:usize, f:F) -> (bool,usize)
    where F: Fn(usize)-> Ordering
{
    let mut size = num;
    if size == 0 {
        return (false, 0);
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
            return (true, mid);
        }

        base = if cmp == Ordering::Greater { base } else { mid };
        size -= half;
    }

    // base is always in [0, size) because base <= mid.
    let cmp = f(base);
    if cmp == Ordering::Equal {
        (true, base)
    } else {
        (false, (base + (cmp == Ordering::Less) as usize))
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

#[cfg(test)]
mod tests {
    use std::ptr;
    use super::*;
    use tempfile;

    #[test]
    fn sorted_search() {
        let aa = vec![1,3,5,7,9];

        let find = 5;
        let res = sorted_search_by(aa.len(), |i|{
            aa[i].cmp(&find)
        });
        assert_eq!(res, (true, 2));

        let find = 4;
        let res = sorted_search_by(aa.len(), |i|{
            aa[i].cmp(&find)
        });
        assert_eq!(res, (false, 2));

        let find = 0;
        let res = sorted_search_by(aa.len(), |i|{
            aa[i].cmp(&find)
        });
        assert_eq!(res, (false, 0));

        let find = 10;
        let res = sorted_search_by(aa.len(), |i|{
            aa[i].cmp(&find)
        });
        assert_eq!(res, (false, 5));
    }

    #[test]
    fn leaf_set() {
        let file = tempfile::tempfile().unwrap();
        let db = Db::load(file, true).unwrap();

        let mut tx = db.getTxWriter();

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
        assert_eq!(ret, Some(Rc::new(key.to_vec())));
    }


}
