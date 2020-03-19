use btree_kv::*;
use tempfile;
use std::rc::Rc;

#[test]
fn btree_test() {
    let dir = tempfile::tempdir().unwrap();
    let file = dir.path().join("test.db");

    let mut db = Db::open(&file).unwrap();

    let tx = db.get_writer();

    let max = 3000;

    for i in 0..max {
        let key = format!("{:->5}", i);
        let key = key.as_bytes();

        tx.set(key, key);
    }
    db.commit(tx);

    let db = Db::open(&file).unwrap();
    let tx = db.get_reader();

    for i in 0..3000 {
        let key = format!("{:->5}", i);
        let key = key.as_bytes();

        let ret = tx.get(&key);
        assert_eq!(ret, Some(Rc::new(key.to_vec())));
    }

    drop(file);
    dir.close().unwrap();
}
