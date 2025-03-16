import os
import shutil
import unittest
from pyrocks11 import RocksDB, DBOptions

class TestIterator(unittest.TestCase):
    def setUp(self):
        self.db_path = "test_database_iter"
        # Clean up any existing database
        if os.path.exists(self.db_path):
            shutil.rmtree(self.db_path)
        
        # Create options
        options = DBOptions()
        options.create_if_missing = True
        
        # Open the database
        self.db = RocksDB.open(self.db_path, options)
        self.default_cf = self.db.get_column_family_handle("default")
        
        # Add some test data
        self.db.put(cfh=self.default_cf, key=b"key1", value=b"value1")
        self.db.put(cfh=self.default_cf, key=b"key2", value=b"value2")
        self.db.put(cfh=self.default_cf, key=b"key3", value=b"value3")
        self.db.put(cfh=self.default_cf, key=b"key4", value=b"value4")
        self.db.put(cfh=self.default_cf, key=b"key5", value=b"value5")
    
    def tearDown(self):
        # Close the database
        self.db.close()
        # Clean up
        if os.path.exists(self.db_path):
            shutil.rmtree(self.db_path)
    
    def test_iterator_basic(self):
        # Create an iterator and seek to first
        it = self.db.iterator(self.db.get_column_family_handle('default'))
        it.seek_to_first()
        
        # Check if valid
        self.assertTrue(it.valid())
        
        # Check first key-value pair
        self.assertEqual(it.key(), b"key1")
        self.assertEqual(it.value(), b"value1")
        
        # Move to next and check
        it.next()
        self.assertEqual(it.key(), b"key2")
        self.assertEqual(it.value(), b"value2")
    
    def test_iterator_seek(self):
        # Seek to a specific key
        it = self.db.iterator(self.db.get_column_family_handle('default'))
        it.seek(b"key3")
        
        # Check if valid
        self.assertTrue(it.valid())
        
        # Check the key-value pair
        self.assertEqual(it.key(), b"key3")
        self.assertEqual(it.value(), b"value3")
        
        # Seek to a key that doesn't exist but comes after the first key
        it.seek(b"key35")
        self.assertTrue(it.valid())
        self.assertEqual(it.key(), b"key4")
    
    def test_iterator_backwards(self):
        # Seek to last and iterate backwards
        it = self.db.iterator(self.db.get_column_family_handle('default'))
        it.seek_to_last()
        
        # Check if valid
        self.assertTrue(it.valid())
        
        # Check last key-value pair
        self.assertEqual(it.key(), b"key5")
        
        # Move to previous and check
        it.prev()
        self.assertEqual(it.key(), b"key4")
        
        it.prev()
        self.assertEqual(it.key(), b"key3")
    
    def test_iterator_python_iteration(self):
        # Use Python iteration protocol
        it = self.db.iterator(self.db.get_column_family_handle('default'))
        it.seek_to_first()
        
        # Collect all key-value pairs
        pairs = list(it)
        
        # Check the results
        expected = [
            (b"key1", b"value1"),
            (b"key2", b"value2"),
            (b"key3", b"value3"),
            (b"key4", b"value4"),
            (b"key5", b"value5")
        ]
        
        self.assertEqual(pairs, expected)
