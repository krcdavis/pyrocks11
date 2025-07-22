from __future__ import annotations
from ._rocksdb_cpp import cDB, cCFHandle, DbOpenBase, DbOpenRW, CompactRangeOptions # type: ignore
from .options import DBOptions, CFOptions
from .iterator import DbIterator
from .batch import WriteBatch
from typing import Optional, Any
import weakref

class RocksDB:
    """
    Python wrapper for RocksDB database.
    
    This class provides a pythonic interface to interact with RocksDB databases,
    including support for column families which allow for logical separation of data
    within the same database file.
    """
    @classmethod
    def open(cls, 
             path : str, 
             options : Optional[DBOptions] = None, 
             column_family_options : Optional[CFOptions|dict[str,CFOptions]] = None,
             open_type : DbOpenBase = DbOpenRW()
             ) -> RocksDB:
        """
        Open a RocksDB database.
        
        Args:
            path (str): Path to the database directory
            options (Options, optional): Database options. If not provided, default options will be used.
            column_family_options (cCFOptions|dict[str,cCFOptions], optional): Options for column families.
                - If a cCFOptions object is provided, it will be used as the default column family options.
                - If a dictionary is provided, keys should be column family names and values should be cCFOptions objects.
                - If None is provided, default column family options will be used.
            
        Returns:
            DB: Database instance
        """
        if options is None:
            options = DBOptions()
            options.create_if_missing = True
        
        if column_family_options is None:
            column_family_options = CFOptions()
 
        return cls(cDB.open(path, options, column_family_options, open_type))

    def __init__(self, db : cDB) -> None:
        self._db = db
        self._fin_set = set()
        self._closed = False

    def get_column_family_handle(self, column_name : str) -> cCFHandle:
        """
        Get a handle for a specific column family.
        
        Args:
            column_name (str): Name of the column family
            
        Returns:
            cCFHandle: Column family handle that can be used in operations
            
        Raises:
            KeyError: If the column family does not exist
        """
        return self._db.get_column_family(column_name)

    def put(self, cfh: cCFHandle, key : bytes, value : bytes) -> None:
        """
        Store a key-value pair in the database under the specified column family.
        
        Args:
            cfh (cCFHandle): Column family handle
            key (bytes): The key to store
            value (bytes): The value to store
        """
        self._db.put(cfh, key, value)
    
    def get(self, cfh: cCFHandle, key: bytes ) -> bytes | None:
        """
        Retrieve a value for the given key from the specified column family.
        
        Args:
            cfh (cCFHandle): Column family handle
            key (bytes): The key to retrieve
            
        Returns:
            bytes: The value associated with the key
            
        Raises:
            KeyError: If the key does not exist in the specified column family
        """
        return self._db.get(cfh, key)
    
    def delete(self, cfh: cCFHandle, key : bytes) -> None:
        """
        Delete a key-value pair from the specified column family.
        
        Args:
            cfh (cCFHandle): Column family handle
            key (bytes): The key to delete
        """
        self._db.delete(cfh, key)
    
    def write(self, batch : WriteBatch) -> None:
        """
        Apply a batch of operations to the database.
        
        Args:
            batch (WriteBatch): Batch of operations to apply
        """
        self._db.write(batch._batch)
    
    def iterator(self, cfh : cCFHandle) -> DbIterator:
        """
        Create an iterator for this database.
        
        Returns:
            DbIterator: Database iterator
        """
        dbi =  self._db.create_iterator(cfh)
        dbw = DbIterator(dbi)
        self._fin_set.add(weakref.finalize(dbw, self._close_hnd, dbi))
        return dbw
    
    def snapshot(self) -> Any:
        """
        Create a snapshot of the database.
        
        Returns:
            object: Snapshot object that can be used with read operations
        """
        return self._db.create_snapshot()
    
    def release_snapshot(self, snapshot : Any) -> None:
        """
        Release a snapshot.
        
        Args:
            snapshot: Snapshot to release
        """
        self._db.release_snapshot(snapshot)
    
    def compact_range(self, compact_range_options: CompactRangeOptions, from_key: Optional[bytes], to_key: Optional[bytes]) -> None:
        """
        Compact a range of keys in the database.
        
        Args:
            compact_range_options (CompactRangeOptions): Options for the compaction
            from_key (bytes, optional): Start key for the range to compact
            to_key (bytes, optional): End key for the range to compact
        """
        self._db.compact_range(compact_range_options, from_key, to_key)
    
    def close(self) -> None:
        """Close the database."""
        if self._closed:
            return
        for fin in self._fin_set:
            fin()
        self._fin_set.clear()
        self._db.close()
        self._closed = True

    def _close_hnd(self, hnd):
        hnd.close()

    def __enter__(self) -> RocksDB:
        return self
    
    def __exit__(self, exc_type: Optional[type], exc_val: Optional[BaseException], exc_tb: Optional[Any]) -> None:
        self.close()




    def list_column_families(self) -> str:
        #return "this is where my column families would go..... if I had one!!!"
        return self._db.list_column_families()
