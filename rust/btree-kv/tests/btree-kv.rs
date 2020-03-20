use btree_kv::*;
use tempfile;
use std::rc::Rc;

#[test]
fn btree_leaf_test() {
    let dir = tempfile::tempdir().unwrap();
    let file = dir.path().join("test.db");

    let key1 = "key1".as_bytes();
    let val1 = "val1".as_bytes();
    let val2 = "val2".as_bytes();

    let mut db = Db::open(&file).unwrap();
    let tx = db.get_writer();

    assert_eq!(tx.has(&key1), false);

    tx.set(key1, val1);
    db.commit(tx);

    let mut db = Db::open(&file).unwrap();
    let tx = db.get_reader();
    assert_eq!(tx.has(&key1), true);
    assert_eq!(tx.get(&key1), Some(Rc::new(val1.to_vec())));

    let mut db = Db::open(&file).unwrap();
    let tx = db.get_writer();
    tx.set(key1, val2);
    db.commit(tx);

    let mut db = Db::open(&file).unwrap();
    let tx = db.get_reader();
    assert_eq!(tx.get(&key1), Some(Rc::new(val2.to_vec())));

    let mut db = Db::open(&file).unwrap();
    let tx = db.get_writer();
    assert_eq!(tx.del(key1), true);
    assert_eq!(tx.del(key1), false);
    db.commit(tx);

    let mut db = Db::open(&file).unwrap();
    let tx = db.get_reader();
    assert_eq!(tx.get(&key1), None);
    assert_eq!(tx.has(&key1), false);

    dir.close().unwrap();
}

#[test]
fn btree_node_test() {
    let dir = tempfile::tempdir().unwrap();
    let file = dir.path().join("test.db");

    let val = Box::new([1u8; 255]);

    let mut db = Db::open(&file).unwrap();
    let tx = db.get_writer();
    for i in 0..20 {
        tx.set(&[i], &val[..]);
    }
    tx.del(&[5]);
    db.commit(tx);

    let mut db = Db::open(&file).unwrap();
    let tx = db.get_writer();
    for i in 20..40 {
        tx.set(&[i], &val[..]);
    }
    db.commit(tx);

    let mut db = Db::open(&file).unwrap();
    let tx = db.get_reader();

    for i in 0..40 {
        if i == 5 {
            assert_eq!(tx.has(&[i]), false);
        } else {
            assert_eq!(tx.has(&[i]), true);
        }
    }

    dir.close().unwrap();
}

#[test]
fn btree_test() {
    let dir = tempfile::tempdir().unwrap();
    let file = dir.path().join("test.db");

    let max = 3000;

    let mut db = Db::open(&file).unwrap();
    let tx = db.get_writer();
    for i in 0..max {
        if i % 3 == 0 {
            let key = format!("{:->10}", i);
            let key = key.as_bytes();
            tx.set(key, key);
        }
    }
    db.commit(tx);

    let mut db = Db::open(&file).unwrap();
    let tx = db.get_writer();
    for i in 0..max {
        if i % 3 == 0 {
            continue;
        }
        let key = format!("{:->10}", i);
        let key = key.as_bytes();
        tx.set(key, key);
    }
    db.commit(tx);

    let mut db = Db::open(&file).unwrap();
    let tx = db.get_writer();
    for i in 0..max {
        if i % 3 == 0 {
            let key = format!("{:->10}", i);
            let key = key.as_bytes();
            tx.del(key);
        }
    }
    db.commit(tx);

    let db = Db::open(&file).unwrap();
    let tx = db.get_reader();
    for i in 0..max {
        let key = format!("{:->10}", i);
        let key = key.as_bytes();

        if i % 3 == 0 {
            let ret = tx.has(&key);
            assert_eq!(ret, false);
        } else if i % 3 == 1 {
            let ret = tx.has(&key);
            assert_eq!(ret, true);
        } else {
            let ret = tx.get(&key);
            assert_eq!(ret, Some(Rc::new(key.to_vec())));
        }
    }

    dir.close().unwrap();
}
