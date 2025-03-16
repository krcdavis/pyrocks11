import os
import shutil
import unittest
from pyrocks11 import RocksDB, DBOptions, CompressionType, CFOptions

class TestColumnFamilies(unittest.TestCase):
    """Test cases for RocksDB column family functionality."""
    
    def setUp(self):
        self.db_path = "test_database_cf"
        # Clean up any existing database
        if os.path.exists(self.db_path):
            shutil.rmtree(self.db_path)
        
        # Create options
        options = DBOptions()
        options.create_if_missing = True
        options.create_missing_column_families = True
        
        # Create column family options
        default_cf_options = CFOptions()
        default_cf_options.compression = CompressionType.ZSTD_COMPRESSION
        
        # Create multiple column family options
        cf_options_dict = {
            "default": default_cf_options,
            "cf1": CFOptions(),
            "cf2": CFOptions()
        }
        
        # Set different compression for cf2
        cf_options_dict["cf2"].compression = CompressionType.LZ4_COMPRESSION
        
        # Open the database with column family options
        self.db = RocksDB.open(self.db_path, options, cf_options_dict)
        
        # Get column family handles
        self.default_cf = self.db.get_column_family_handle("default")
        self.cf1 = self.db.get_column_family_handle("cf1")
        self.cf2 = self.db.get_column_family_handle("cf2")
    
    def tearDown(self):
        # Close the database
        self.db.close()
        # Clean up
        if os.path.exists(self.db_path):
            shutil.rmtree(self.db_path)
    
    def test_get_column_family_handle(self):
        """Test getting column family handles."""
        # Test getting existing column families
        default_cf = self.db.get_column_family_handle("default")
        cf1 = self.db.get_column_family_handle("cf1")
        cf2 = self.db.get_column_family_handle("cf2")
        
        # Verify handles are not None
        self.assertIsNotNone(default_cf)
        self.assertIsNotNone(cf1)
        self.assertIsNotNone(cf2)
        
        # Test getting non-existent column family
        self.assertRaises(RuntimeError, self.db.get_column_family_handle, "nonexistent")
    
    def test_put_get_with_column_families(self):
        """Test put and get operations with different column families."""
        # Put data in different column families
        self.db.put(self.default_cf, b"key1", b"default_value")
        self.db.put(self.cf1, b"key1", b"cf1_value")
        self.db.put(self.cf2, b"key1", b"cf2_value")
        
        # Get data from different column families
        self.assertEqual(self.db.get(self.default_cf, b"key1"), b"default_value")
        self.assertEqual(self.db.get(self.cf1, b"key1"), b"cf1_value")
        self.assertEqual(self.db.get(self.cf2, b"key1"), b"cf2_value")
    
    def test_column_family_isolation(self):
        """Test that column families are isolated from each other."""
        # Put data in one column family
        self.db.put(self.cf1, b"isolated_key", b"isolated_value")
        
        # Verify the key exists in cf1
        self.assertEqual(self.db.get(self.cf1, b"isolated_key"), b"isolated_value")
        
        # Verify the key doesn't exist in other column families
        self.assertEqual(self.db.get(self.default_cf, b"isolated_key"), None)
        self.assertEqual(self.db.get(self.cf2, b"isolated_key"), None)
    
    def test_delete_with_column_families(self):
        """Test delete operations with different column families."""
        # Put data in different column families
        self.db.put(self.default_cf, b"delete_key", b"default_value")
        self.db.put(self.cf1, b"delete_key", b"cf1_value")
        self.db.put(self.cf2, b"delete_key", b"cf2_value")
        
        # Delete from cf1 only
        self.db.delete(self.cf1, b"delete_key")
        
        # Verify key is deleted from cf1
        self.assertEqual(self.db.get(self.cf1, b"delete_key"), None)
        
        # Verify key still exists in other column families
        self.assertEqual(self.db.get(self.default_cf, b"delete_key"), b"default_value")
        self.assertEqual(self.db.get(self.cf2, b"delete_key"), b"cf2_value")
        
        # Delete from all column families
        self.db.delete(self.default_cf, b"delete_key")
        self.db.delete(self.cf2, b"delete_key")
        
        self.assertEqual(self.db.get(self.default_cf, b"delete_key"), None)
        self.assertEqual(self.db.get(self.cf2, b"delete_key"), None)
    
    def test_binary_data_with_column_families(self):
        """Test handling binary data in different column families."""
        # Test with binary data
        binary_key = b"\x00\x01\x02\x03"
        binary_value_default = b"\xFF\xFE\xFD\xFC"
        binary_value_cf1 = b"\xAA\xBB\xCC\xDD"
        binary_value_cf2 = b"\x11\x22\x33\x44"
        
        # Put binary data in different column families
        self.db.put(self.default_cf, binary_key, binary_value_default)
        self.db.put(self.cf1, binary_key, binary_value_cf1)
        self.db.put(self.cf2, binary_key, binary_value_cf2)
        
        # Get binary data from different column families
        self.assertEqual(self.db.get(self.default_cf, binary_key), binary_value_default)
        self.assertEqual(self.db.get(self.cf1, binary_key), binary_value_cf1)
        self.assertEqual(self.db.get(self.cf2, binary_key), binary_value_cf2)