import os
import shutil
import unittest
from pyrocks11 import RocksDB, DBOptions, CompressionType, CFOptions

class TestDB(unittest.TestCase):
    def setUp(self):
        self.db_path = "test_database"
        # Clean up any existing database
        if os.path.exists(self.db_path):
            shutil.rmtree(self.db_path)
        
        # Create options
        dbo = DBOptions()
        dbo.create_if_missing = True 
        dbo.create_missing_column_families = True

        cfo = CFOptions()
        cfo.compression = CompressionType.ZSTD_COMPRESSION
        
        # Open the database
        self.db = RocksDB.open(self.db_path, dbo, cfo)
        self.default_cf = self.db.get_column_family_handle("default")
    
    def tearDown(self):
        # Close the database
        self.db.close()
        # Clean up
        if os.path.exists(self.db_path):
            shutil.rmtree(self.db_path)
    
    def test_put_get(self):
        # Test basic put and get
        self.db.put(self.default_cf, b"key1", b"value1")
        self.assertEqual(self.db.get(self.default_cf, b"key1"), b"value1")
        
        # Test with binary data
        binary_key = b"\x00\x01\x02\x03"
        binary_value = b"\xFF\xFE\xFD\xFC"
        self.db.put(self.default_cf, binary_key, binary_value)
        self.assertEqual(self.db.get(self.default_cf, binary_key), binary_value)
    
    def test_delete(self):
        # Put a key and verify it exists
        # self.db.put(b"key2", b"value2")
        # self.assertEqual(self.db.get(b"key2"), b"value2")
        self.db.put(self.default_cf, b"key2", b"value2")
        self.assertEqual(self.db.get(self.default_cf, b"key2"), b"value2")
        
        # Delete the key and verify it's gone
        # self.db.delete(b"key2")
        self.db.delete(self.default_cf, b"key2")

        self.assertEqual(self.db.get(self.default_cf,b"key2"), None)
    
    def test_nonexistent_key(self):
        # Try to get a key that doesn't exist
        self.assertEqual(self.db.get(self.default_cf,b"nonexistent"), None)
