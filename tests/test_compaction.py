import os
import shutil
import unittest
from pyrocks11 import RocksDB, DBOptions, CompressionType, CFOptions, DbOpenRW, CompactRangeOptions
from tests.utils import check_log_for_occurrence


class TestCompaction(unittest.TestCase):
    def setUp(self):
        self.db_path = "test_database_compaction"
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

        self.db = RocksDB.open(self.db_path, dbo, cfo, DbOpenRW())
        self.default_cfh = self.db.get_column_family_handle("default")

    def tearDown(self):
        # Close the database
        self.db.close()
        # Clean up
        if os.path.exists(self.db_path):
            shutil.rmtree(self.db_path)
        
    def test_manual_compaction(self):
        for i in range(1000):
            key = f"key_{i:06d}".encode()
            value = f"value_{i}".encode() * 50
            self.db.put(self.default_cfh, key, value)
        
        compact_options = CompactRangeOptions()
        compact_options.exclusive_manual_compaction = True
        
        self.db.compact_range(compact_options, None, None)
        
        # Check if compaction occurred by examining the LOG file
        log_file_path = os.path.join(self.db_path, "LOG")
        compaction_patterns = [
            r"Manual compaction.*from.*to",
            r"Manual compaction starting",
        ]
        compaction_occurred = check_log_for_occurrence(log_file_path, compaction_patterns)
        
        # Assert that compaction actually happened
        self.assertTrue(compaction_occurred, "Manual compaction did not occur according to the LOG file")