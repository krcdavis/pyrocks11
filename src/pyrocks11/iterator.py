from ._rocksdb_cpp import cIterator # type: ignore
from typing import Iterator

class DbIterator():
    """
    Iterator for RocksDB databases.
    
    This class provides a pythonic interface for RocksDB iterators.
    """
    
    def __init__(self, iter_handle : cIterator) -> None:
        """
        Initialize the iterator.
        
        Args:
            iter_handle: Native iterator handle
        """
        self._iter = iter_handle
    
    def seek_to_first(self) -> None:
        """Position at the first key in the database."""
        self._iter.seek_to_first()
    
    def seek_to_last(self) -> None:
        """Position at the last key in the database."""
        self._iter.seek_to_last()
    
    def seek(self, key : bytes) -> None:
        """
        Position at the first key in the database that is at or past the given key.
        
        Args:
            key (bytes): Key to seek to
        """
        self._iter.seek(key)
    
    def valid(self) -> bool:
        """
        Check if the iterator is valid.
        
        Returns:
            bool: True if the iterator is valid, False otherwise
        """
        return self._iter.valid()
    
    def next(self) -> None:
        """Move to the next entry in the database."""
        self._iter.next()
    
    def prev(self) -> None:
        """Move to the previous entry in the database."""
        self._iter.prev()
    
    def key(self) -> bytes:
        """
        Get the key at the current position.
        
        Returns:
            bytes: Current key
            
        Raises:
            StopIteration: If the iterator is not valid
        """
        if not self.valid():
            raise StopIteration("Iterator not valid")
        return self._iter.key()
    
    def value(self) -> bytes:
        """
        Get the value at the current position.
        
        Returns:
            bytes: Current value
            
        Raises:
            StopIteration: If the iterator is not valid
        """
        if not self.valid():
            raise StopIteration("Iterator not valid")
        return self._iter.value()
    
    def __iter__(self) -> Iterator[tuple[bytes,bytes]]:
        """Make this object iterable."""
        return self
    
    def __next__(self) -> tuple[bytes, bytes]:
        """
        Get the next key-value pair.
        
        Returns:
            tuple: (key, value) tuple
            
        Raises:
            StopIteration: When there are no more items
        """
        if not self.valid():
            raise StopIteration
        
        key = self.key()
        value = self.value()
        self.next()
        
        return (key, value)
