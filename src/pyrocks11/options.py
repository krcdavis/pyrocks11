from ._rocksdb_cpp import cDBOptions, cCFOptions # type: ignore

class DBOptions(cDBOptions):
    """
    Wrapper for DBOptions (datase) options for RocksDB database.
    """

class CFOptions(cCFOptions):
    """
    Wrapper for CFOptions (column family) options for RocksDB database.
    """