from ._rocksdb_cpp import cWriteBatch, cCFHandle  # type: ignore
class WriteBatch:
    """
    A batch of write operations.
    
    This class provides a way to atomically apply multiple updates to a database.
    """
    
    def __init__(self) -> None:
        """Initialize an empty write batch."""
        self._batch = cWriteBatch()
    
    def put(self, cfh: cCFHandle, key : bytes, value : bytes) -> None:
        """
        Add a put operation to the batch.
        
        Args:
            key (bytes): Key to put
            value (bytes): Value to put
            
        Returns:
            WriteBatch: self for method chaining
        """
        self._batch.put(cfh, key, value)
        return None
    
    def delete(self, cfh: cCFHandle, key: bytes) -> None:
        """
        Add a delete operation to the batch.
        
        Args:
            key (bytes): Key to delete
            
        Returns:
            WriteBatch: self for method chaining
        """
        self._batch.delete(cfh, key)
        return None
    
    def clear(self) -> None:
        """
        Clear all operations from the batch.
        
        Returns:
            WriteBatch: self for method chaining
        """
        self._batch.clear()
        return None
    
    def count(self) -> int:
        """
        Get the number of operations in the batch.
        
        Returns:
            int: Number of operations
        """
        return self._batch.count()
