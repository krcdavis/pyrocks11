import os
import shutil
import unittest
from pyrocks11 import RocksDB, DBOptions, WriteBatch

class TestBatch(unittest.TestCase):
    def setUp(self):
        self.db_path = "test_database_batch"
        # Clean up any existing database
        if os.path.exists(self.db_path):
            shutil.rmtree(self.db_path)
        
        # Create options
        options = DBOptions()
        options.create_if_missing = True
        
        # Open the database
        self.db = RocksDB.open(self.db_path, options)
        self.default_cf = self.db.get_column_family_handle("default")
    
    def tearDown(self):
        # Close the database
        self.db.close()
        # Clean up
        if os.path.exists(self.db_path):
            shutil.rmtree(self.db_path)
    
    def test_batch_operations(self):
        # Create a batch
        batch = WriteBatch()

        # Add operations to the batch
        batch.put(cfh=self.default_cf, key=b"key1", value=b"value1")
        batch.put(cfh=self.default_cf, key=b"key2", value=b"value2")
        batch.put(cfh=self.default_cf, key=b"key3", value=b"value3")
        batch.delete(cfh=self.default_cf, key=b"key3")
        
        # Check the count
        self.assertEqual(batch.count(), 4)
        
        # Apply the batch
        self.db.write(batch)
        
        # Verify the results
        # self.assertEqual(self.db.get(b"key1"), b"value1")
        # self.assertEqual(self.db.get(b"key2"), b"value2")
        # self.assertEqual(self.db.get(b"key3"), None)
        self.assertEqual(self.db.get(cfh=self.default_cf, key=b"key1"), b"value1")
        self.assertEqual(self.db.get(cfh=self.default_cf, key=b"key2"), b"value2")
        self.assertEqual(self.db.get(cfh=self.default_cf, key=b"key3"), None)
    
    def test_batch_clear(self):
        # Create a batch with operations
        batch = WriteBatch()
        batch.put(cfh=self.default_cf, key=b"key1", value=b"value1")
        batch.put(cfh=self.default_cf, key=b"key2", value=b"value2")
        
        # Clear the batch
        batch.clear()
        
        # Count should be 0
        self.assertEqual(batch.count(), 0)
        
        # Write the empty batch
        self.db.write(batch)
        
        # Keys should not exist
        self.assertEqual(self.db.get(cfh=self.default_cf,key=b"key1"), None)
        self.assertEqual(self.db.get(cfh=self.default_cf,key=b"key2"), None)

