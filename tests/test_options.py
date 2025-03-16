import os
import shutil
import unittest
from pyrocks11 import RocksDB, DBOptions, CompressionType, CFOptions

class TestOptions(unittest.TestCase):
    def setUp(self):
        self.db_path = "test_rocksdb_opt"
        # Clean up any existing database
        if os.path.exists(self.db_path):
            shutil.rmtree(self.db_path)
    
    def tearDown(self):
        # Clean up
        if os.path.exists(self.db_path):
            shutil.rmtree(self.db_path)
    
    def test_options_chain(self):
        options = DBOptions()
        options.create_if_missing = True
        options.increase_parallelism(4)
        options.max_open_files = 100

        default_cf_options = CFOptions()
        default_cf_options.compression = CompressionType.SNAPPY_COMPRESSION
        default_cf_options.optimize_level_style_compaction(64 * 1024 * 1024)
        default_cf_options.write_buffer_size = 8 * 1024 * 1024
        
        # Open a database with these options
        db = RocksDB.open(self.db_path, options)
        cfh = db.get_column_family_handle("default")
        
        # Put a key just to test that the database works
        db.put(cfh=cfh,key=b"test_key", value=b"test_value")
        self.assertEqual(db.get(cfh=cfh, key=b"test_key"), b"test_value")
        
        # Close the database
        db.close()