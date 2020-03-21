extern crate libc;

use std::default::Default;
use std::mem;
use std::sync::{Arc, Mutex, MutexGuard, RwLock, RwLockReadGuard};
use std::u32::MAX as U32_MAX;
use std::cmp::Ordering;
use std::hash::Hash;
use std::hash::Hasher;
use std::rc::Rc;
use std::cell::RefCell;
use std::path::Path;
use std::fs::{self, File, OpenOptions};
use std::io;

use fnv::FnvHasher;
use memmap::{MmapMut, MmapOptions};

const MAGIC_NUM : u32 = 12349872;

pub struct Db {
    meta: Mutex<RefCell<Rc<Meta>>>,
    locker: Vec<Rc<RwLock<()>>>,
    writer: Mutex<()>,
}

impl Db {
    pub fn open<P: AsRef<Path>>(path: P) -> io::Result<Arc<Db>> {
        let need_init = match fs::metadata(&path) {
            Ok(_) => false,
            Err(_) => true,
        };

        let file = OpenOptions::new()
            .read(true).write(true).create(true).open(path)?;

        Db::load(file, need_init)
    }

    fn load(file: File, need_init:bool) -> io::Result<Arc<Db>> {
        let mmap = Rc::new(Mmap::load(&file, need_init)?);

        let meta_id = mmap.get_meta_id();
        let meta = mmap.load_meta(meta_id).clone();

        let items = mmap.load_range(meta.free_page, meta.free_num);

        let meta = Meta{
            file: Rc::new(file),
            meta_id: meta_id,
            low : meta,
            free_pages : items.to_vec(),
            hold_pages : Vec::new(),
            mmap: mmap.clone(),
        };

        let mut locker = vec![];
        for _ in 0..mmap.meta_num {
            locker.push(Rc::new(RwLock::new(())));
        }

        let db = Db {
            meta: Mutex::new(RefCell::new(Rc::new(meta))),
            locker: locker,
            writer: Mutex::new(()),
        };

        Ok(Arc::new(db))
    }

    //##################################
    //操作模型
    //有N个可操作位置, 是同一个文件上的不同区域
    //多个读，1个写，读写互不影响
    //
    //每次写操作都不会改原数据而在下一个位置写改动
    //如果写操作过快追上正进行的读则等待读解锁
    //##################################

    pub fn get_reader(&self) -> TxReader {
        let (meta, locker) = loop {
            let meta = {
                //1.上元信息锁
                let meta = self.meta.lock().unwrap();
                let meta = meta.borrow().as_ref().clone();
                Rc::new(meta)
                //2.解元信息锁
            };

            //3.得到元信息读锁
            let meta_id = meta.meta_id as usize;
            let locker = self.locker[meta_id].read().unwrap();

            //防止得到的元信息在加锁前被写覆盖
            let low = meta.mmap.load_meta(meta.meta_id);
            if meta.low.tx_id == low.tx_id {
                break (meta, locker);
            }
        };

        let node = load_inode(meta.clone(), meta.low.root_page);

        //4.对元信息上读锁
        //5.直到read被释放自动解锁
        TxReader {
            root: node,
            _locker: locker,
        }
    }

    pub fn get_writer(&self) -> TxWriter {
        //1 上写动作锁
        let writer = self.writer.lock().unwrap();

        let meta = {
            //1.上元信息锁
            let meta = self.meta.lock().unwrap();
            let meta = meta.borrow().as_ref().clone();
            Rc::new(meta)
            //2.解元信息锁
        };

        let node = load_inode(meta.clone(), meta.low.root_page);

        //4.writer被释放解写动作锁
        TxWriter {
            dirty: false,
            meta: meta,
            root: node,
            _locker: writer,
        }
    }

    pub fn commit(&self, tx:TxWriter) {
        tx.as_mut().sync();

        let meta_id = (tx.meta.meta_id + 1) % tx.meta.mmap.meta_num;
        let meta_id = meta_id as usize;

        {
            //1.对被覆盖元信息上写锁
            let _ = self.locker[meta_id].write().unwrap();
            //写动作
            tx.meta.sync();
            //2.写解锁
        }

        {
            //1.上元信息锁
            let meta = self.meta.lock().unwrap();
            //替换原元信息
            *meta.borrow_mut() = tx.meta.clone();
            //2.解元信息锁
        }
    }

    pub fn info(&self) {
        let meta = {
            //1.上元信息锁
            let meta = self.meta.lock().unwrap();
            let meta = meta.borrow().clone();
            meta
            //2.解元信息锁
        };

        println!("db version:{}", 1);
        println!("pagesize:{}", meta.mmap.page_size);
        println!("meta num:{}", meta.mmap.meta_num);
        println!("tx id:{}", meta.low.tx_id);
        println!("data num:{}", meta.low.data_num);
        println!("node use:{}", meta.low.node_num);
        println!("node hold:{}", meta.low.hold_num);
        println!("node free:{}", meta.low.free_num);
    }
}

pub struct TxReader<'a> {
    root: Rc<dyn Inode>,
    _locker: RwLockReadGuard<'a, ()>,
}

impl <'a>TxReader<'a> {
    pub fn get(&self, key:&[u8]) -> Option<Rc<Vec<u8>>> {
        self.root.get(key)
    }

    pub fn has(&self, key:&[u8]) -> bool {
        self.root.has(key)
    }
}

pub struct TxWriter<'a> {
    dirty: bool,
    meta:Rc<Meta>,
    root: Rc<dyn Inode>,
    _locker: MutexGuard<'a, ()>,
}

impl <'a>TxWriter<'a> {
    fn as_mut(&self) -> &mut Self{
        let t = self as *const _ as *mut _;
        unsafe{ &mut (*t) }
    }

    pub fn get(&self, key:&[u8]) -> Option<Rc<Vec<u8>>> {
        self.root.get(key)
    }

    pub fn has(&self, key:&[u8]) -> bool {
        self.root.has(key)
    }

    pub fn set(&self, key:&[u8], val:&[u8]) -> Result<(), &str> {
        if key.len() > 255 {
            return Err("key too long");
        }
        if val.len() > 1024 {
            return Err("val too long");
        }

        self.as_mut().dirty = true;
        self.root.set(key, val);

        Ok(())
    }

    pub fn del(&self, key:&[u8]) -> bool {
        self.as_mut().dirty = true;
        self.root.del(key)
    }

    fn sync(&mut self) {
        if !self.dirty {
            return;
        }
        self.dirty = false;

        self.root = balance_root(self.meta.clone(), self.root.clone());

        self.root.sync();

        self.meta.as_mut().low.root_page = self.root.pid();
    }
}

struct Mmap {
    page_size: u32,
    page_num: u32,
    meta_num : u16,
    data: MmapMut,
}

impl Mmap {
    fn load(fh:&File, init:bool) -> io::Result<Self> {
        let meta_num = 3;
        let mut page_size:u32 = 4096;

        let len = if init {
            page_size = unsafe { libc::sysconf(libc::_SC_PAGESIZE) } as u32;
            let len = page_size as u64 * (meta_num+1) as u64;

            fh.set_len(len)?;
            fh.sync_all()?;
            len
        } else {
            let len = fh.metadata()?.len();
            len
        };

        let data = unsafe {
            MmapOptions::new().map_mut(fh)?
        };

        let header = LowHeader::load(&data);

        if init {
            header.init(page_size, meta_num);
        } else if !header.vaild() {
            return Err(io::Error::new(io::ErrorKind::Other, "Invalid file type"));
        }

        let page_num = ((len / header.page_size as u64) + (len % header.page_size as u64 > 0) as u64) as u32;

        let mmap = Self {
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
            if m2.vaild() {
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

    fn node_type(&self, page_id: u32) -> NodeType {
        let raw = self.load_page(page_id);
        unsafe {
            mem::transmute::<_, NodeType>(*raw)
        }
    }
}

enum Typ {
    Node,
    NodeItem,
    Leaf,
}

trait Inode {
    fn typ(&self) -> Typ;
    fn pid(&self) -> u32 { panic!("panic"); }
    fn sync(&self) { panic!("panic"); }
    fn get(&self, _key:&[u8]) -> Option<Rc<Vec<u8>>>
    { panic!("panic"); }
    fn set(&self, _key:&[u8], _val:&[u8])
    { panic!("panic"); }
    fn del(&self, _key:&[u8]) -> bool
    { panic!("panic"); }
    fn has(&self, _key:&[u8]) -> bool
    { panic!("panic"); }
    fn key(&self) -> Rc<Vec<u8>>
    { panic!("panic"); }
    fn val(&self) -> Rc<Vec<u8>>
    { panic!("panic"); }
    fn empty(&self) -> bool { panic!("panic"); }
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
    fn empty(&self) -> bool { false }
    fn sync(&self) {}
    fn balance(&self) -> Vec<Rc<dyn Inode>> { vec![] }
}

struct Node {
    tx: Rc<Meta>,
    dirty: bool,
    pid:u32,
    balanced: bool,
    origin: bool,
    childs: Vec<Rc<dyn Inode>>,
}

struct Leaf {
    pid : u32,
    dirty: bool,
    origin: bool,
    balanced: bool,
    tx: Rc<Meta>,
    items: Vec<LeafItem>,
}

#[derive(Clone, Debug)]
struct LeafItem{
    key:Rc<Vec<u8>>,
    val:Rc<Vec<u8>>,
}

impl Node {
    fn as_mut(&self) -> &mut Self {
        let t = self as *const _ as *mut _;
        unsafe{ &mut (*t) }
    }

    fn new(tx: Rc<Meta>, childs: Vec<Rc<dyn Inode>>) -> Self {
        Self {
            tx:tx,
            dirty: true,
            pid: 0,
            balanced: true,
            origin: false,
            childs: childs,
        }
    }

    fn load(tx: Rc<Meta>, pid:u32) -> Self {
        let low = tx.mmap.load_node(pid);

        Self {
            tx:tx.clone(),
            dirty: false,
            pid: pid,
            balanced: true,
            origin: true,
            childs: low.load(),
        }
    }

    fn seek(&self, key:&[u8]) -> usize {
        let f = |i:usize| {
            let key3 = self.childs[i].key();
            str_cmp(&key3, &key)
        };

        let (has, i) = sorted_search_by(self.childs.len(), f);
        if has {
            i
        } else if i > 1 {
            i-1
        } else {
            0
        }
    }

    fn capacity(&self) -> usize {
        let s1 = mem::size_of::<LowNode>() - 1;
        let page_size = self.tx.mmap.page_size as usize;
        page_size - s1
    }

    fn size(&self) -> usize {
        let s1 = mem::size_of::<LowNode>() - 1;
        let mut size = 0;
        for x in self.childs.iter() {
            size += 4 + 1 + x.key().len();
        }
        size + s1
    }

    fn child(&self, i:usize) -> Rc<dyn Inode> {
        let typ = self.childs[i].typ();

        if let Typ::NodeItem = typ {
            let tmp = load_inode(self.tx.clone(), self.childs[i].pid());
            self.as_mut().childs[i] = tmp;
        }

        self.childs[i].clone()
    }

    fn childs_balance(&mut self) {
        let mut childs = Vec::new();

        for child in self.childs.iter() {
            if let Typ::NodeItem = child.typ() {
                childs.push(child.clone());
                continue;
            }

            let items = balance_split(child.clone());

            for tmp in items.iter() {
                childs.push(tmp.clone());
            }
        }

        self.childs = childs;
    }
}

impl Inode for Node {
    fn typ(&self) -> Typ { Typ::Node }
    fn pid(&self) -> u32 { self.pid }
    fn empty(&self) -> bool { self.childs.len() == 0 }

    fn has(&self, key:&[u8]) -> bool {
        let i = self.seek(key);

        if self.childs.get(i).is_none() {
            return false;
        }

        self.child(i).has(key)
    }

    fn get(&self, key:&[u8]) -> Option<Rc<Vec<u8>>> {
        let i = self.seek(key);

        if self.childs.get(i).is_none() {
            return None;
        }

        self.child(i).get(key)
    }

    fn del(&self, key:&[u8]) -> bool {
        let i = self.seek(key);

        if self.childs.get(i).is_none() {
            return false;
        }

        let ok = self.child(i).del(key);
        if ok {
            let this = self.as_mut();
            this.balanced = false;
            this.dirty = true;
        }
        ok
    }

    fn set(&self, key:&[u8], val:&[u8]) {
        let i = self.seek(key);

        let this = self.as_mut();
        this.dirty = true;
        this.balanced = false;

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
        let this = self.as_mut();

        if self.empty() {
            if self.pid != 0 {
                if self.origin {
                    self.tx.page_hold(self.pid, 1);
                } else {
                    self.tx.page_free(self.pid, 1);
                }
                this.pid = 0;
            }

            this.dirty = false;
            this.balanced = true;
        }

        if this.balanced {
            return vec![];
        }
        this.balanced = true;

        this.childs_balance();

        let s1 = self.capacity();
        let s2 = self.size();

        //页数, 每页个数
        let page = (s2 / s1) + (s2 % s1 > 0) as usize;
        let size = s2 / page;

        let mut pages:Vec<Vec<Rc<dyn Inode>>> = vec![vec![]];

        let mut now = 0;
        for item in self.childs.iter() {
            now += LowNode::item_size(item.clone());
            pages.last_mut().unwrap().push(item.clone());

            if now > size {
                pages.push(vec![]);
            }
        }

        this.childs = pages[0].clone();

        let mut out:Vec<Rc<dyn Inode>> = vec![];
        for i in 1..pages.len() {
            let node = Node::new(self.tx.clone(), pages[i].clone());
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
        low.write(&self.childs);
    }
}

impl Leaf {
    fn as_mut(&self) -> &mut Self {
        let t = self as *const _ as *mut _;
        unsafe{ &mut(*t) }
    }

    fn new(tx: Rc<Meta>, childs: Vec<LeafItem>) -> Self {
        Self {
            tx:tx,
            dirty: true,
            balanced: true,
            pid: 0,
            origin: false,
            items: childs,
        }
    }

    fn load(tx: Rc<Meta>, pid:u32) -> Self {
        let low = tx.mmap.load_leaf(pid);

        Self {
            tx: tx.clone(),
            dirty: false,
            balanced: true,
            pid: pid,
            origin: true,
            items: low.load(),
        }
    }

    fn seek(&self, key:&[u8]) -> (bool,usize) {
        let f = |i:usize| {
            let key2 = &self.items[i].key;
            str_cmp(&key2, &key)
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
        let (has,i) = self.seek(key);

        let this = self.as_mut();
        this.dirty = true;
        this.balanced = false;

        let tmp = LeafItem{
            key:Rc::new(key.to_vec()),
            val:Rc::new(val.to_vec()),
        };

        if has {
            this.items[i] = tmp;
        } else {
            this.items.insert(i, tmp);
            self.tx.as_mut().low.data_num += 1;
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
        let (has, _) = self.seek(key);
        has
    }

    fn del(&self, key:&[u8]) -> bool {
        let (has,i) = self.seek(key);

        if has {
            let this = self.as_mut();
            this.dirty = true;
            this.balanced = false;
            this.items.remove(i);

            self.tx.as_mut().low.data_num -= 1;

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
        let this = self.as_mut();

        if self.empty() {
            if self.pid != 0 {
                if self.origin {
                    self.tx.page_hold(self.pid, 1);
                } else {
                    self.tx.page_free(self.pid, 1);
                }
                this.pid = 0;
            }

            this.dirty = false;
            this.balanced = true;
        }

        if this.balanced {
            return vec![];
        }
        this.balanced = true;

        let mut size = 0;
        for item in self.items.iter() {
            size += LowLeaf::item_size(item);
        }

        let s1 = self.capacity();
        let num = (size / s1) + (size % s1 > 0) as usize;
        let s2 = size / num;

        size = 0;
        let mut pages = vec![vec![]];
        for item in self.items.iter() {
            if size > s2 {
                size = 0;
                pages.push(Vec::new());
            }

            size += LowLeaf::item_size(item);
            pages.last_mut().unwrap().push(item.clone());
        }

        this.items = pages[0].clone();

        let mut out:Vec<Rc<dyn Inode>> = Vec::new();

        for i in 1..pages.len() {
            let leaf = Leaf::new(self.tx.clone(), pages[i].clone());
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
    if pid == 0 {
        return Rc::new(Leaf::new(tx.clone(), vec![]));
    }

    if let NodeType::Node = tx.mmap.node_type(pid) {
        let node = Node::load(tx.clone(), pid);
        Rc::new(node)
    } else {
        let leaf = Leaf::load(tx.clone(), pid);
        Rc::new(leaf)
    }
}

fn balance_root(tx:Rc<Meta>, node:Rc<dyn Inode>) -> Rc<dyn Inode> {
    let nodes = balance_split(node);

    if nodes.len() == 0 {
        Rc::new(Leaf::new(tx.clone(), vec![]))
    } else if nodes.len() == 1{
        nodes[0].clone()
    } else {
        let node = Rc::new(Node::new(tx.clone(), nodes));
        balance_root(tx.clone(), node)
    }
}

fn balance_split(node:Rc<dyn Inode>) -> Vec<Rc<dyn Inode>> {
    let mut out: Vec<Rc<dyn Inode>> = vec![];

    let mut items = node.balance();

    if !node.empty() {
        out.push(node);
        if items.len() > 0 {
            out.append(&mut items);
        }
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

    fn sync(&self) {
        let sizeof_range = mem::size_of::<LowRange>();
        let psize = self.mmap.page_size as usize;

        if self.low.free_page != 0 {
            let s1 = self.low.free_num as usize * sizeof_range;
            let pnum = (s1 / psize) + (s1 % psize > 0) as usize;
            self.page_hold(self.low.free_page, pnum as u32);
        }

        if self.low.hold_page != 0 {
            let s1 = self.low.hold_num as usize * sizeof_range;
            let pnum = (s1 / psize) + (s1 % psize > 0) as usize;
            self.page_hold(self.low.hold_page, pnum as u32);
        }

        let this = self.as_mut();

        let dst_meta_id = (self.meta_id + 1) % self.mmap.meta_num;
        let dst_meta = self.mmap.load_meta(dst_meta_id);

        //将目标版本保持的页添加到空闲表
        if dst_meta.vaild() {
            if dst_meta.hold_page != 0 && dst_meta.hold_num > 0 {
                let holds = self.mmap.load_range(dst_meta.hold_page, dst_meta.hold_num);
                for hold in holds.iter() {
                    this.page_free(hold.from_id, hold.end_id - hold.from_id);
                }
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
        this.low.free_num = self.free_pages.len() as u32;
        this.low.hold_num = self.hold_pages.len() as u32;

        //校验码
        this.low.sum = this.low.sum_fnv();

        //先刷入数据
        self.mmap.data.flush().unwrap();

        *dst_meta = this.low;

        //再同步元信息
        self.mmap.data.flush().unwrap();

        this.meta_id = dst_meta_id;
        this.hold_pages = vec![];
    }

    fn page_free(&self, pid:u32, len:u32) {
        let page = LowRange{
            from_id : pid,
            end_id : pid + len,
        };

        let tx = self.as_mut();

        tx.low.node_num -= len;

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

        tx.low.node_num += num;

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

    fn vaild(&self) -> bool {
        if self.sum == 0 || self.magic != MAGIC_NUM || self.version != 1 {
            return false;
        }

        self.sum_fnv() == self.sum
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
    fn item_size(i: &LeafItem) -> usize {
        3 + i.key.len() + i.val.len()
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

            i+=1;
            let key = raw[i..i+len].to_vec();
            i+=len;

            let len = ((raw[i] as usize) << 8) | (raw[i+1] as usize);
            i+=2;

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
            i+=1;

            raw[i..i+len].copy_from_slice(&item.key);
            i+=len;

            let len = item.val.len();
            raw[i] = ((len >> 8)&255) as u8;
            raw[i+1] = (len&255) as u8;
            i+=2;

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
    node_num: u32,
    data_num: u64,
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

    fn vaild(&self) -> bool {
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

#[repr(C)]
#[derive(Default, Clone, Debug)]
struct LowNode{
    node_type: NodeType,
    num: u16,
    data: [u32; 1],
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
    fn item_size(i: Rc<dyn Inode>) -> usize {
        5 + i.key().len()
    }

    fn pid(&self, i: u16) -> &mut u32 {
        let item = &self.data[0] as *const _ as *mut u32;
        unsafe {
            let item = item.offset(i as isize);
            &mut (*item)
        }
    }

    fn raw(&self) -> &mut[u8] {
        let raw = self.pid(self.num) as *const _ as *mut [u8; U32_MAX as usize];
        unsafe{ &mut (*raw) }
    }

    fn load(&self) -> Vec<Rc<dyn Inode>> {
        let mut out:Vec<Rc<dyn Inode>> = vec![];
        let raw = self.raw();

        let mut oft = 0;
        for i in 0..self.num {
            let len = raw[oft] as usize;
            oft += 1;

            let pid = self.pid(i);
            let key = &raw[oft..oft+len];
            oft += len;

            let r = Rc::new(NodeItem{
                pid: *pid,
                key: Rc::new(key.to_vec()),
            });

            out.push(r);
        }

        out
    }

    fn write(&mut self, items:&[Rc<dyn Inode>]) {
        self.node_type = NodeType::Node;
        self.num = items.len() as u16;

        let raw = self.raw();

        let mut oft = 0;
        for i in 0..items.len() {
            let tmp = self.pid(i as u16);
            *tmp = items[i].pid();

            let key = &items[i].key()[..];
            let len = key.len();

            raw[oft] = len as u8;
            oft += 1;

            raw[oft..oft+len].copy_from_slice(key);
            oft += len;
        }
    }
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

    // let s1 = &a[0] as *const _ as *const libc::c_void;
    // let s2 = &b[0] as *const _ as *const libc::c_void;

    // let cmp = a.len().cmp(&b.len());
    // let len = if Ordering::Less == cmp { a.len() } else { b.len() };

    // let rs = unsafe {
        // libc::memcmp(s1, s2, len)
    // };

    // if rs < 0 {
        // Ordering::Less
    // } else if rs == 0 {
        // cmp
    // } else {
        // Ordering::Greater
    // }
}

#[cfg(test)]
mod tests {
    use super::*;

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
    fn low_leaf_test() {
        let raw = Box::new([0u8; 4096]);
        let low = &raw[0] as *const _ as *mut LowLeaf;
        let low = unsafe{ &mut (*low) };

        let mut items = vec![];
        for i in 0..20 {
            let key = format!("{:->5}", i);
            let key = key.as_bytes();

            items.push(LeafItem{
                key: Rc::new(key.to_vec()),
                val: Rc::new(key.to_vec()),
            });
        }

        low.write(&items);

        let out = low.load();

        assert_eq!(items.len(), out.len());

        if items.len() == out.len() {
            for i in 0..items.len() {
                assert_eq!(items[i].key, out[i].key);
            }
        }
    }

    #[test]
    fn low_node_test() {
        let raw = Box::new([0u8; 4096]);
        let low = &raw[0] as *const _ as *mut LowNode;
        let low = unsafe{ &mut (*low) };

        let mut items:Vec<Rc<dyn Inode>> = vec![];

        for i in 0..20 {
            let key = format!("{:->5}", i);
            let key = key.as_bytes();

            items.push(Rc::new(NodeItem{
                pid: i as u32,
                key: Rc::new(key.to_vec()),
            }));
        }

        low.write(&items);

        let out = low.load();

        assert_eq!(items.len(), out.len());

        if items.len() == out.len() {
            for i in 0..items.len() {
                assert_eq!(items[i].pid(), out[i].pid());
            }
        }
    }

}
