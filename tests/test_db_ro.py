import os
import shutil
import unittest
from pyrocks11 import RocksDB, DBOptions, CompressionType, CFOptions, DbOpenRO, DbOpenRW

class TestDBModeRO(unittest.TestCase):
    def setUp(self):
        self.db_path = "test_database_rw"
        # Clean up any existing database
        if os.path.exists(self.db_path):
            shutil.rmtree(self.db_path)

        # Create options
        dbo = DBOptions()
        dbo.create_if_missing = True 
        dbo.create_missing_column_families = True

        cfo = CFOptions()
        cfo.compression = CompressionType.ZSTD_COMPRESSION

        self.default_dbo = dbo
        self.default_cfo = cfo
        self.db_open_ro = DbOpenRO()
        
        # Create a writable database and add some test data
        self.db = RocksDB.open(self.db_path, dbo, cfo, DbOpenRW())
        cfh = self.db.get_column_family_handle("default")
        self.db.put(cfh, b"key1", b"value1")
        self.db.close()

    def tearDown(self):
        # Clean up database
        if os.path.exists(self.db_path):
            shutil.rmtree(self.db_path)

    def test_create_db_failure(self):
        db_path = "test_database_ro_failure"
        # This should raise an exception because the DB doesn't exist and can't be created in RO mode
        self.assertRaises(RuntimeError, RocksDB.open, db_path, self.default_dbo, self.default_cfo, self.db_open_ro)
        
    def test_open_existing_db(self):
        # Should successfully open an existing database in read-only mode
        db = RocksDB.open(self.db_path, self.default_dbo, self.default_cfo, self.db_open_ro)
        self.assertIsInstance(db, RocksDB)
        db.close()

    def test_read_from_existing_db(self):
        # Should successfully read data from an existing database
        db = RocksDB.open(self.db_path, self.default_dbo, self.default_cfo, self.db_open_ro)
        cfh = db.get_column_family_handle("default")
        value = db.get(cfh, b"key1")
        self.assertEqual(value, b"value1")  # Verify the actual value
        db.close()

    def test_write_to_existing_db(self):
        # Should fail to write to a read-only database
        db = RocksDB.open(self.db_path, self.default_dbo, self.default_cfo, self.db_open_ro)
        cfh = db.get_column_family_handle("default")
        self.assertRaises(RuntimeError, db.put, cfh, b"key2", b"value2")
        db.close()