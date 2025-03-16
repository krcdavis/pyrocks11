import os
import re

def check_log_for_occurrence(log_file_path:str, patterns: list[str] = []) -> bool:
    """
    Checks if a pattern occurred in the RocksDB LOG file.
    
    Args:
        log_file_path (str): Path to the RocksDB LOG file
        patterns (list[str]): List of patterns to search for in the log file
        
    Returns:
        bool: True if a pattern occurred, False otherwise
    """
    if not os.path.exists(log_file_path):
        return False
    
    try:
        with open(log_file_path, 'r') as log_file:
            log_content = log_file.read()
            
            for pattern in patterns:
                if re.search(pattern, log_content):
                    return True
    except Exception as e:
        print(f"Error reading log file: {e}")
        
    return False
