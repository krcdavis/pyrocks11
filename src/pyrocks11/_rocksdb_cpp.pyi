from enum import IntEnum
from typing import Any, Union, Optional, Dict, Mapping

class CompressionType(IntEnum):
    NO_COMPRESSION: int
    SNAPPY_COMPRESSION: int
    ZLIB_COMPRESSION: int
    LZ4_COMPRESSION: int
    LZ4HC_COMPRESSION: int
    ZSTD_COMPRESSION: int

class BlobGarbageCollectionPolicy(IntEnum):
    kForce: int
    kDisable: int
    kUseDefault: int

class BottommostLevelCompaction(IntEnum):
    kSkip: int
    kIfHaveCompactionFilter: int
    kForce: int
    kForceOptimized: int

class EncodingType(IntEnum):
    kPlain: int
    kPrefix: int

class cCFHandle:
    pass

class DbOpenBase:
    pass

class DbOpenRW(DbOpenBase):
    def __init__(self) -> None: ...

class DbOpenRO(DbOpenBase):
    def __init__(self) -> None: ...

class CompactRangeOptions:
    def __init__(self) -> None: ...

    change_level: bool
    exclusive_manual_compaction: bool
    change_level: bool
    target_level: int
    target_path_id: int
    bottommost_level_compaction: int
    allow_write_stall: bool
    max_subcompactions: int
    full_history_ts_low: str
    blob_garbage_collection_policy: int
    blob_garbage_collection_age_cutoff: float

    def to_dict(self) -> dict[str, Union[int, bool, str]]: ...

class PlainTableOptions:
    def __init__(self) -> None: ...

    user_key_len : int
    store_index_in_file : bool
    index_sparseness : int
    huge_page_tlb_size : int
    hash_table_ratio : float
    full_scan_mode : bool
    encoding_type : int
    bloom_bits_per_key : int

    def to_dict(self) -> dict[str, Union[int, bool, str]]: ...

class cCFOptions:
    def __init__(self) -> None: ...
    def optimize_level_style_compaction(self, memtable_memory_budget: int = 512 * 1024 * 1024) -> None: ...
    def optimize_for_small_db(self) -> None: ...

    enable_blob_files: bool
    min_blob_size: int
    enable_blob_garbage_collection: bool
    blob_file_size: int 
    blob_compression_type: int
    compression: int
    bottom_most_compression: int
    write_buffer_size: int
    level0_file_num_compaction_trigger: int
    max_bytes_for_level_base: int
    disable_auto_compactions: bool

    def to_dict(self) -> dict[str, Union[int, bool, str]]: ...

class cDBOptions:
    def __init__(self) -> None: ...
    def increase_parallelism(self, total_threads: int) -> None: ...

    # Configuration properties
    create_if_missing: bool
    create_missing_column_families: bool
    error_if_exists: bool
    paranoid_checks: bool
    flush_verify_memtable_count: bool
    track_and_verify_wals_in_manifest: bool
    verify_sst_unique_id_in_manifest: bool
    max_open_files: int
    max_file_opening_threads: int
    max_total_wal_size: int
    use_fsync: bool
    db_log_dir: str
    wal_dir: str
    delete_obsolete_files_period_micros: int
    max_background_jobs: int
    max_background_compactions: int
    max_subcompactions: int
    max_background_flushes: int
    max_log_file_size: int
    log_file_time_to_roll: int
    keep_log_file_num: int
    recycle_log_file_num: int
    max_manifest_file_size: int
    table_cache_numshardbits: int
    WAL_ttl_seconds: int
    WAL_size_limit_MB: int
    manifest_preallocation_size: int
    allow_mmap_reads: bool
    allow_mmap_writes: bool
    use_direct_reads: bool
    use_direct_io_for_flush_and_compaction: bool
    allow_fallocate: bool
    is_fd_close_on_exec: bool
    stats_dump_period_sec: int
    stats_persist_period_sec: int
    persist_stats_to_disk: bool
    stats_history_buffer_size: int
    advise_random_on_open: bool
    db_write_buffer_size: int
    compaction_readahead_size: int
    random_access_max_buffer_size: int
    writable_file_max_buffer_size: int
    use_adaptive_mutex: bool
    bytes_per_sync: int
    wal_bytes_per_sync: int
    strict_bytes_per_sync: bool
    enable_thread_tracking: bool
    delayed_write_rate: int
    enable_pipelined_write: bool
    unordered_write: bool
    allow_concurrent_memtable_write: bool
    enable_write_thread_adaptive_yield: bool
    max_write_batch_group_size_bytes: int
    write_thread_max_yield_usec: int
    write_thread_slow_yield_usec: int
    skip_stats_update_on_db_open: bool
    skip_checking_sst_file_sizes_on_db_open: bool
    allow_2pc: bool
    fail_if_options_file_error: bool
    dump_malloc_stats: bool
    avoid_flush_during_recovery: bool
    avoid_flush_during_shutdown: bool
    allow_ingest_behind: bool
    two_write_queues: bool
    manual_wal_flush: bool
    atomic_flush: bool
    avoid_unnecessary_blocking_io: bool
    write_dbid_to_manifest: bool
    log_readahead_size: int
    best_efforts_recovery: bool
    max_bgerror_resume_count: int
    bgerror_resume_retry_interval: int
    allow_data_in_errors: bool
    db_host_id: str
    enforce_single_del_contracts: bool
    compaction_verify_record_count: bool
    write_identity_file: bool
    prefix_seek_opt_in_only: bool
    # lowest_used_cache_tier: Any
    # wal_filter: Any
    wal_recovery_mode: int
    # wal_compression: CompressionType
    info_log_level: int
    # file_checksum_gen_factory: Any
    follower_refresh_catchup_period_ms: int
    follower_catchup_retry_count: int
    wal_write_temperature: str
    metadata_write_temperature: str
    background_close_inactive_wals: bool
    follower_catchup_retry_wait_ms: int
    
    def to_dict(self) -> dict[str, Union[int, bool, str]]: ...

class cDB:
    @staticmethod
    def open(path: str, db_options: cDBOptions, column_families : cCFOptions | Mapping[str, cCFOptions], open_type : DbOpenBase) -> 'cDB': ...
    def get_column_family(self, name: str) -> cCFHandle: ...
    def put(self, cfh: cCFHandle, key: bytes, value: bytes) -> None: ...
    def get(self, cfh: cCFHandle, key: bytes) -> bytes: ...
    def delete(self, cfh: cCFHandle, key: bytes) -> None: ...
    def write(self, batch: cWriteBatch) -> None: ...
    def create_iterator(self, cfh : cCFHandle) -> cIterator: ...
    def create_snapshot(self) -> Any: ...
    def release_snapshot(self, snapshot: Any) -> None: ...
    def compact_range(self, compact_range_options: CompactRangeOptions, from_key: Optional[bytes], to_key: Optional[bytes]) -> None: ...
    def close(self) -> None: ...

    def list_column_families(self) -> dict: ...

class cIterator:
    def seek_to_first(self) -> None: ...
    def seek_to_last(self) -> None: ...
    def seek(self, key: bytes) -> None: ...
    def valid(self) -> bool: ...
    def next(self) -> None: ...
    def prev(self) -> None: ...
    def key(self) -> bytes: ...
    def value(self) -> bytes: ...

class cWriteBatch:
    def __init__(self) -> None: ...
    def put(self, cfh: cCFHandle, key: bytes, value: bytes) -> None: ...
    def delete(self, cfh: cCFHandle, key: bytes) -> None: ...
    def clear(self) -> None: ...
    def count(self) -> int: ...