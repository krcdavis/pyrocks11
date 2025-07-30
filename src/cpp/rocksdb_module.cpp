#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <rocksdb/table.h>
#include "db_wrapper.h"
#include "iterator_wrapper.h"
#include "batch_wrapper.h"
#include "cf_handle.h"
#include "db_open_types.h"

namespace py = pybind11;
using namespace py::literals;

const char* get_compression_name(rocksdb::CompressionType type) {
    switch(type) {
    case rocksdb::CompressionType::kNoCompression:
        return "kNocompression";
    case rocksdb::CompressionType::kSnappyCompression:
        return "kSnappyCompression";
    case rocksdb::CompressionType::kZlibCompression:
        return "kZlibCompression";
    case rocksdb::CompressionType::kBZip2Compression:
        return "kBZip2Compression";
    case rocksdb::CompressionType::kLZ4Compression:
        return "kLZ4Compression";
    case rocksdb::CompressionType::kLZ4HCCompression:
        return "kLZ4HCCompression";
    case rocksdb::CompressionType::kXpressCompression:
        return "kXpressCompression";
    case rocksdb::CompressionType::kZSTD:
        return "kZSTD";
    case rocksdb::kZSTDNotFinalCompression:
        return "kZSTDNotFinalCompression";
    case rocksdb::kDisableCompressionOption:
        return "kDisableCompressionOption";
    default:
        return "kUNKNOWNCompressionType";
    }
}

const char* get_blob_garbage_collection_policy_name(rocksdb::BlobGarbageCollectionPolicy policy) {
    switch(policy) {
    case rocksdb::BlobGarbageCollectionPolicy::kForce:
        return "kForce";
    case rocksdb::BlobGarbageCollectionPolicy::kDisable:
        return "kDisable";
    case rocksdb::BlobGarbageCollectionPolicy::kUseDefault:
        return "kUseDefault";
    default:
        return "kUNKNOWNBlobGarbageCollectionPolicy";
    }
}

const char* get_bottommost_level_compaction_name(rocksdb::BottommostLevelCompaction option) {
    switch(option) {
    case rocksdb::BottommostLevelCompaction::kSkip:
        return "kSkip";
    case rocksdb::BottommostLevelCompaction::kIfHaveCompactionFilter:
        return "kIfHaveCompactionFilter";
    case rocksdb::BottommostLevelCompaction::kForce:
        return "kForce";
    case rocksdb::BottommostLevelCompaction::kForceOptimized:
        return "kForceOptimized";
    default:
        return "kUNKNOWNBottommostLevelCompaction";
    }
}

const char* get_encoding_type_name(rocksdb::EncodingType enc) {
    switch(enc) {
    case rocksdb::EncodingType::kPlain:
        return "kPlain";
    case rocksdb::EncodingType::kPrefix:
        return "kPrefix";
    default:
        return "kUNKNOWNEncodingType";
    }
}

PYBIND11_MODULE(_rocksdb_cpp, m) {
    m.doc() = "Python 11 bindings for RocksDB";

    py::class_<ColumnFamilyHandle>(m, "cCFHandle");

    py::class_<rocksdb::ColumnFamilyOptions>(m, "cCFOptions")
        .def(py::init())
        .def("optimize_level_style_compaction", [](rocksdb::ColumnFamilyOptions& self, int memtable_memory_budget = 512 * 1024 * 1024) {
            self.OptimizeLevelStyleCompaction(memtable_memory_budget);
            return py::none();
        })
        .def("optimize_for_small_db", [](rocksdb::ColumnFamilyOptions& self) {
            /** TODO: Implemnet caching */
            self.OptimizeForSmallDb();
            return py::none();
        })
        .def("set_plain_table", [](rocksdb::ColumnFamilyOptions& self, const rocksdb::PlainTableOptions& pto) {
          self.table_factory.reset(NewPlainTableFactory(pto));
          return py::none();
        })
        .def_readwrite("enable_blob_files", &rocksdb::ColumnFamilyOptions::enable_blob_files)
        .def_readwrite("enable_blob_garbage_collection", &rocksdb::ColumnFamilyOptions::enable_blob_garbage_collection)
        .def_readwrite("min_blob_size", &rocksdb::ColumnFamilyOptions::min_blob_size)
        .def_readwrite("blob_file_size", &rocksdb::ColumnFamilyOptions::blob_file_size)
        .def_readwrite("blob_compression_type", &rocksdb::ColumnFamilyOptions::blob_compression_type)
        .def_readwrite("compression", &rocksdb::ColumnFamilyOptions::compression)
        .def_readwrite("bottom_most_compression", &rocksdb::ColumnFamilyOptions::bottommost_compression)
        .def_readwrite("write_buffer_size", &rocksdb::ColumnFamilyOptions::write_buffer_size)
        .def_readwrite("level0_file_num_compaction_trigger", &rocksdb::ColumnFamilyOptions::level0_file_num_compaction_trigger)
        .def_readwrite("max_bytes_for_level_base", &rocksdb::ColumnFamilyOptions::max_bytes_for_level_base)
        .def_readwrite("disable_auto_compactions", &rocksdb::ColumnFamilyOptions::disable_auto_compactions)
        .def("to_dict", [](const rocksdb::ColumnFamilyOptions &instance) {
            return py::dict(
                "blob_compression_type"_a = get_compression_name(instance.blob_compression_type),
                "compression"_a = get_compression_name(instance.compression),
                "bottom_most_compression"_a = get_compression_name(instance.bottommost_compression),
                "enable_blob_files"_a = instance.enable_blob_files,
                "enable_blob_garbage_collection"_a = instance.enable_blob_garbage_collection,
                "min_blob_size"_a = instance.min_blob_size,
                "blob_file_size"_a = instance.blob_file_size,
                "write_buffer_size"_a = instance.write_buffer_size,
                "level0_file_num_compaction_trigger"_a = instance.level0_file_num_compaction_trigger,
                "max_bytes_for_level_base"_a = instance.max_bytes_for_level_base,
                "disable_auto_compactions"_a = instance.disable_auto_compactions); 
        });

    py::enum_<rocksdb::BottommostLevelCompaction>(m, "BottommostLevelCompaction")
        .value("kSkip", rocksdb::BottommostLevelCompaction::kSkip)
        .value("kIfHaveCompactionFilter", rocksdb::BottommostLevelCompaction::kIfHaveCompactionFilter)
        .value("kForce", rocksdb::BottommostLevelCompaction::kForce)
        .value("kForceOptimized", rocksdb::BottommostLevelCompaction::kForceOptimized)
        .export_values();
    
    py::enum_<rocksdb::BlobGarbageCollectionPolicy>(m, "BlobGarbageCollectionPolicy")
        .value("kForce", rocksdb::BlobGarbageCollectionPolicy::kForce)
        .value("kDisable", rocksdb::BlobGarbageCollectionPolicy::kDisable)
        .value("kUseDefault", rocksdb::BlobGarbageCollectionPolicy::kUseDefault)
        .export_values();

    py::class_<rocksdb::CompactRangeOptions>(m,"CompactRangeOptions")
        .def(py::init())
        .def_readwrite("exclusive_manual_compaction", &rocksdb::CompactRangeOptions::exclusive_manual_compaction)
        .def_readwrite("change_level", &rocksdb::CompactRangeOptions::change_level)
        .def_readwrite("target_level", &rocksdb::CompactRangeOptions::target_level)
        .def_readwrite("target_path_id", &rocksdb::CompactRangeOptions::target_path_id)
        .def_readwrite("bottommost_level_compaction", &rocksdb::CompactRangeOptions::bottommost_level_compaction)
        .def_readwrite("allow_write_stall", &rocksdb::CompactRangeOptions::allow_write_stall)
        .def_readwrite("max_subcompactions", &rocksdb::CompactRangeOptions::max_subcompactions)
        // .def_readwrite("full_history_ts_low", &rocksdb::CompactRangeOptions::full_history_ts_low)
        .def_readwrite("blob_garbage_collection_policy", &rocksdb::CompactRangeOptions::blob_garbage_collection_policy)
        .def_readwrite("blob_garbage_collection_age_cutoff", &rocksdb::CompactRangeOptions::blob_garbage_collection_age_cutoff)
        .def("to_dict", [](const rocksdb::CompactRangeOptions &instance) {
            return py::dict(
                "exclusive_manual_compaction"_a = instance.exclusive_manual_compaction,
                "change_level"_a = instance.change_level,
                "target_level"_a = instance.target_level,
                "target_path_id"_a = instance.target_path_id,
                "bottommost_level_compaction"_a = get_bottommost_level_compaction_name(instance.bottommost_level_compaction),
                "allow_write_stall"_a = instance.allow_write_stall,
                "max_subcompactions"_a = instance.max_subcompactions,
                // "full_history_ts_low"_a = instance.full_history_ts_low,
                "blob_garbage_collection_policy"_a = get_blob_garbage_collection_policy_name(instance.blob_garbage_collection_policy),
                "blob_garbage_collection_age_cutoff"_a = instance.blob_garbage_collection_age_cutoff);
        });

/** UNFINISHED
    py::class_<rocksdb::BlockBasedTableOptions>(m, "BlockBasedTableOptions")
        .def(py::init())
         // .def_readwrite("flush_block_policy_factory", &rocksdb::BlockBasedTableOptions::flush_block_policy_factory)
        .def_readwrite("cache_index_and_filter_blocks", &rocksdb::BlockBasedTableOptions::cache_index_and_filter_blocks)
        .def_readwrite("cache_index_and_filter_blocks_with_high_priority", &rocksdb::BlockBasedTableOptions::cache_index_and_filter_blocks_with_high_priority)
        .def_readwrite("pin_l0_filter_and_index_blocks_in_cache", &rocksdb::BlockBasedTableOptions::pin_l0_filter_and_index_blocks_in_cache)
        .def_readwrite("pin_top_level_index_and_filter", &rocksdb::BlockBasedTableOptions::pin_top_level_index_and_filter)
        .def_readwrite("metadata_cache_options", &rocksdb::BlockBasedTableOptions::metadata_cache_options)
         // .def_readwrite("index_type", &rocksdb::BlockBasedTableOptions::index_type)
        .def_readwrite("data_block_index_type", &rocksdb::BlockBasedTableOptions::data_block_index_type)
        .def_readwrite("data_block_hash_table_util_ratio", &rocksdb::BlockBasedTableOptions::data_block_hash_table_util_ratio)
        .def_readwrite("checksum", &rocksdb::BlockBasedTableOptions::checksum)
        .def_readwrite("no_block_cache", &rocksdb::BlockBasedTableOptions::no_block_cache)
         // .def_readwrite("block_cache", &rocksdb::BlockBasedTableOptions::block_cache)
        .def_readwrite("persistent_cache", &rocksdb::BlockBasedTableOptions::persistent_cache)
        .def_readwrite("block_size", &rocksdb::BlockBasedTableOptions::block_size)
        .def_readwrite("block_size_deviation", &rocksdb::BlockBasedTableOptions::block_size_deviation)
        .def_readwrite("block_restart_interval", &rocksdb::BlockBasedTableOptions::block_restart_interval)
        .def_readwrite("index_block_restart_interval", &rocksdb::BlockBasedTableOptions::index_block_restart_interval)
        .def_readwrite("metadata_block_size", &rocksdb::BlockBasedTableOptions::metadata_block_size)
        .def_readwrite("cache_usage_options", &rocksdb::BlockBasedTableOptions::cache_usage_options)
        .def_readwrite("partition_filters", &rocksdb::BlockBasedTableOptions::partition_filters)
        .def_readwrite("decouple_partitioned_filters", &rocksdb::BlockBasedTableOptions::decouple_partitioned_filters)
        .def_readwrite("optimize_filters_for_memory", &rocksdb::BlockBasedTableOptions::optimize_filters_for_memory)
        .def_readwrite("use_delta_encoding", &rocksdb::BlockBasedTableOptions::use_delta_encoding)
        // .def_readwrite("filter_policy", &rocksdb::BlockBasedTableOptions::filter_policy)
        .def_readwrite("whole_key_filtering", &rocksdb::BlockBasedTableOptions::whole_key_filtering)
        .def_readwrite("detect_filter_construct_corruption", &rocksdb::BlockBasedTableOptions::detect_filter_construct_corruption)
        .def_readwrite("verify_compression", &rocksdb::BlockBasedTableOptions::verify_compression)
        .def_readwrite("read_amp_bytes_per_bit", &rocksdb::BlockBasedTableOptions::read_amp_bytes_per_bit)
        .def_readwrite("format_version", &rocksdb::BlockBasedTableOptions::format_version)
        .def_readwrite("enable_index_compression", &rocksdb::BlockBasedTableOptions::enable_index_compression)
        .def_readwrite("block_align", &rocksdb::BlockBasedTableOptions::block_align)
        // .def_readwrite("index_shortening", &rocksdb::BlockBasedTableOptions::index_shortening)
        .def_readwrite("max_auto_readahead_size", &rocksdb::BlockBasedTableOptions::max_auto_readahead_size)
        // .def_readwrite("prepopulate_block_cache", &rocksdb::BlockBasedTableOptions::prepopulate_block_cache)
        .def_readwrite("initial_auto_readahead_size", &rocksdb::BlockBasedTableOptions::initial_auto_readahead_size)
        .def_readwrite("num_file_reads_for_auto_readahead", &rocksdb::BlockBasedTableOptions::num_file_reads_for_auto_readahead);
*/

    py::enum_<rocksdb::EncodingType>(m, "EncodingType")
        .value("kPlain", rocksdb::EncodingType::kPlain)
        .value("kPrefix", rocksdb::EncodingType::kPrefix)
        .export_values();

    py::class_<rocksdb::PlainTableOptions>(m, "PlainTableOptions")
        .def(py::init())
        .def_readwrite("user_key_len", &rocksdb::PlainTableOptions::user_key_len)
        .def_readwrite("store_index_in_file", &rocksdb::PlainTableOptions::store_index_in_file)
        .def_readwrite("index_sparseness", &rocksdb::PlainTableOptions::index_sparseness)
        .def_readwrite("huge_page_tlb_size", &rocksdb::PlainTableOptions::huge_page_tlb_size)
        .def_readwrite("hash_table_ratio", &rocksdb::PlainTableOptions::hash_table_ratio)
        .def_readwrite("full_scan_mode", &rocksdb::PlainTableOptions::full_scan_mode)
        .def_readwrite("encoding_type", &rocksdb::PlainTableOptions::encoding_type)
        .def_readwrite("bloom_bits_per_key", &rocksdb::PlainTableOptions::bloom_bits_per_key)
        .def("to_dict", [](const rocksdb::PlainTableOptions &instance) {
            return py::dict(
                "user_key_len"_a = instance.user_key_len,
                "store_index_in_file"_a = instance.store_index_in_file,
                "index_sparseness"_a = instance.index_sparseness,
                "huge_page_tlb_size"_a = instance.huge_page_tlb_size,
                "hash_table_ratio"_a = instance.hash_table_ratio,
                "full_scan_mode"_a = instance.full_scan_mode,
                "encoding_type"_a = get_encoding_type_name(instance.encoding_type),
                "bloom_bits_per_key"_a = instance.bloom_bits_per_key);
        });

    // Register DBOptions class
    py::class_<rocksdb::DBOptions>(m, "cDBOptions")
        .def(py::init())
        .def("increase_parallelism", [](rocksdb::DBOptions& self, int total_threads) {
            self.IncreaseParallelism(total_threads);
            return py::none();
        })
        .def_readwrite("create_if_missing", &rocksdb::DBOptions::create_if_missing)
        .def_readwrite("create_missing_column_families", &rocksdb::DBOptions::create_missing_column_families)
        .def_readwrite("error_if_exists", &rocksdb::DBOptions::error_if_exists)
        .def_readwrite("paranoid_checks", &rocksdb::DBOptions::paranoid_checks)
        .def_readwrite("flush_verify_memtable_count", &rocksdb::DBOptions::flush_verify_memtable_count)
        .def_readwrite("track_and_verify_wals_in_manifest", &rocksdb::DBOptions::track_and_verify_wals_in_manifest)
        .def_readwrite("verify_sst_unique_id_in_manifest", &rocksdb::DBOptions::verify_sst_unique_id_in_manifest)
        .def_readwrite("max_open_files", &rocksdb::DBOptions::max_open_files)
        .def_readwrite("max_file_opening_threads", &rocksdb::DBOptions::max_file_opening_threads)
        .def_readwrite("max_total_wal_size", &rocksdb::DBOptions::max_total_wal_size)
        .def_readwrite("use_fsync", &rocksdb::DBOptions::use_fsync)
        .def_readwrite("db_log_dir", &rocksdb::DBOptions::db_log_dir)
        .def_readwrite("wal_dir", &rocksdb::DBOptions::wal_dir)
        .def_readwrite("delete_obsolete_files_period_micros", &rocksdb::DBOptions::delete_obsolete_files_period_micros)
        .def_readwrite("max_background_jobs", &rocksdb::DBOptions::max_background_jobs)
        .def_readwrite("max_background_compactions", &rocksdb::DBOptions::max_background_compactions)
        .def_readwrite("max_subcompactions", &rocksdb::DBOptions::max_subcompactions)
        .def_readwrite("max_background_flushes", &rocksdb::DBOptions::max_background_flushes)
        .def_readwrite("max_log_file_size", &rocksdb::DBOptions::max_log_file_size)
        .def_readwrite("log_file_time_to_roll", &rocksdb::DBOptions::log_file_time_to_roll)
        .def_readwrite("keep_log_file_num", &rocksdb::DBOptions::keep_log_file_num)
        .def_readwrite("recycle_log_file_num", &rocksdb::DBOptions::recycle_log_file_num)
        .def_readwrite("max_manifest_file_size", &rocksdb::DBOptions::max_manifest_file_size)
        .def_readwrite("table_cache_numshardbits", &rocksdb::DBOptions::table_cache_numshardbits)
        .def_readwrite("WAL_ttl_seconds", &rocksdb::DBOptions::WAL_ttl_seconds)
        .def_readwrite("WAL_size_limit_MB", &rocksdb::DBOptions::WAL_size_limit_MB)
        .def_readwrite("manifest_preallocation_size", &rocksdb::DBOptions::manifest_preallocation_size)
        .def_readwrite("allow_mmap_reads", &rocksdb::DBOptions::allow_mmap_reads)
        .def_readwrite("allow_mmap_writes", &rocksdb::DBOptions::allow_mmap_writes)
        .def_readwrite("use_direct_reads", &rocksdb::DBOptions::use_direct_reads)
        .def_readwrite("use_direct_io_for_flush_and_compaction", &rocksdb::DBOptions::use_direct_io_for_flush_and_compaction)
        .def_readwrite("allow_fallocate", &rocksdb::DBOptions::allow_fallocate)
        .def_readwrite("is_fd_close_on_exec", &rocksdb::DBOptions::is_fd_close_on_exec)
        .def_readwrite("stats_dump_period_sec", &rocksdb::DBOptions::stats_dump_period_sec)
        .def_readwrite("stats_persist_period_sec", &rocksdb::DBOptions::stats_persist_period_sec)
        .def_readwrite("persist_stats_to_disk", &rocksdb::DBOptions::persist_stats_to_disk)
        .def_readwrite("stats_history_buffer_size", &rocksdb::DBOptions::stats_history_buffer_size)
        .def_readwrite("advise_random_on_open", &rocksdb::DBOptions::advise_random_on_open)
        .def_readwrite("db_write_buffer_size", &rocksdb::DBOptions::db_write_buffer_size)
        .def_readwrite("compaction_readahead_size", &rocksdb::DBOptions::compaction_readahead_size)
        .def_readwrite("random_access_max_buffer_size", &rocksdb::DBOptions::random_access_max_buffer_size)
        .def_readwrite("writable_file_max_buffer_size", &rocksdb::DBOptions::writable_file_max_buffer_size)
        .def_readwrite("use_adaptive_mutex", &rocksdb::DBOptions::use_adaptive_mutex)
        .def_readwrite("bytes_per_sync", &rocksdb::DBOptions::bytes_per_sync)
        .def_readwrite("wal_bytes_per_sync", &rocksdb::DBOptions::wal_bytes_per_sync)
        .def_readwrite("strict_bytes_per_sync", &rocksdb::DBOptions::strict_bytes_per_sync)
        .def_readwrite("enable_thread_tracking", &rocksdb::DBOptions::enable_thread_tracking)
        .def_readwrite("delayed_write_rate", &rocksdb::DBOptions::delayed_write_rate)
        .def_readwrite("enable_pipelined_write", &rocksdb::DBOptions::enable_pipelined_write)
        .def_readwrite("unordered_write", &rocksdb::DBOptions::unordered_write)
        .def_readwrite("allow_concurrent_memtable_write", &rocksdb::DBOptions::allow_concurrent_memtable_write)
        .def_readwrite("enable_write_thread_adaptive_yield", &rocksdb::DBOptions::enable_write_thread_adaptive_yield)
        .def_readwrite("max_write_batch_group_size_bytes", &rocksdb::DBOptions::max_write_batch_group_size_bytes)
        .def_readwrite("write_thread_max_yield_usec", &rocksdb::DBOptions::write_thread_max_yield_usec)
        .def_readwrite("write_thread_slow_yield_usec", &rocksdb::DBOptions::write_thread_slow_yield_usec)
        .def_readwrite("skip_stats_update_on_db_open", &rocksdb::DBOptions::skip_stats_update_on_db_open)
        .def_readwrite("skip_checking_sst_file_sizes_on_db_open", &rocksdb::DBOptions::skip_checking_sst_file_sizes_on_db_open)
        .def_readwrite("allow_2pc", &rocksdb::DBOptions::allow_2pc)
        .def_readwrite("fail_if_options_file_error", &rocksdb::DBOptions::fail_if_options_file_error)
        .def_readwrite("dump_malloc_stats", &rocksdb::DBOptions::dump_malloc_stats)
        .def_readwrite("avoid_flush_during_recovery", &rocksdb::DBOptions::avoid_flush_during_recovery)
        .def_readwrite("avoid_flush_during_shutdown", &rocksdb::DBOptions::avoid_flush_during_shutdown)
        .def_readwrite("allow_ingest_behind", &rocksdb::DBOptions::allow_ingest_behind)
        .def_readwrite("two_write_queues", &rocksdb::DBOptions::two_write_queues)
        .def_readwrite("manual_wal_flush", &rocksdb::DBOptions::manual_wal_flush)
        .def_readwrite("atomic_flush", &rocksdb::DBOptions::atomic_flush)
        .def_readwrite("avoid_unnecessary_blocking_io", &rocksdb::DBOptions::avoid_unnecessary_blocking_io)
        .def_readwrite("write_dbid_to_manifest", &rocksdb::DBOptions::write_dbid_to_manifest)
        .def_readwrite("log_readahead_size", &rocksdb::DBOptions::log_readahead_size)
        .def_readwrite("best_efforts_recovery", &rocksdb::DBOptions::best_efforts_recovery)
        .def_readwrite("max_bgerror_resume_count", &rocksdb::DBOptions::max_bgerror_resume_count)
        .def_readwrite("bgerror_resume_retry_interval", &rocksdb::DBOptions::bgerror_resume_retry_interval)
        .def_readwrite("allow_data_in_errors", &rocksdb::DBOptions::allow_data_in_errors)
        .def_readwrite("db_host_id", &rocksdb::DBOptions::db_host_id)
        .def_readwrite("enforce_single_del_contracts", &rocksdb::DBOptions::enforce_single_del_contracts)
        .def_readwrite("compaction_verify_record_count", &rocksdb::DBOptions::compaction_verify_record_count)
        .def_readwrite("write_identity_file", &rocksdb::DBOptions::write_identity_file)
        .def_readwrite("prefix_seek_opt_in_only", &rocksdb::DBOptions::prefix_seek_opt_in_only)
        // .def_readwrite("lowest_used_cache_tier", &rocksdb::DBOptions::lowest_used_cache_tier)
        // .def_readwrite("wal_filter", &rocksdb::DBOptions::wal_filter)
        .def_readwrite("wal_recovery_mode", &rocksdb::DBOptions::wal_recovery_mode)
        // .def_readwrite("wal_compression", &rocksdb::DBOptions::wal_compression)
        .def_readwrite("info_log_level", &rocksdb::DBOptions::info_log_level)
        // .def_readwrite("file_checksum_gen_factory", &rocksdb::DBOptions::file_checksum_gen_factory)
        .def_readwrite("follower_refresh_catchup_period_ms", &rocksdb::DBOptions::follower_refresh_catchup_period_ms)
        .def_readwrite("follower_catchup_retry_count", &rocksdb::DBOptions::follower_catchup_retry_count)
        .def_readwrite("wal_write_temperature", &rocksdb::DBOptions::wal_write_temperature)
        .def_readwrite("metadata_write_temperature", &rocksdb::DBOptions::metadata_write_temperature)
        .def_readwrite("background_close_inactive_wals", &rocksdb::DBOptions::background_close_inactive_wals)
        .def_readwrite("follower_catchup_retry_wait_ms", &rocksdb::DBOptions::follower_catchup_retry_wait_ms)
        .def("to_dict", [](const rocksdb::DBOptions &instance) {
            return py::dict(
                "create_if_missing"_a = instance.create_if_missing,
                "create_missing_column_families"_a = instance.create_missing_column_families,
                "error_if_exists"_a = instance.error_if_exists, 
                "paranoid_checks"_a = instance.paranoid_checks,
                "flush_verify_memtable_count"_a = instance.flush_verify_memtable_count,
                "track_and_verify_wals_in_manifest"_a = instance.track_and_verify_wals_in_manifest,
                "verify_sst_unique_id_in_manifest"_a = instance.verify_sst_unique_id_in_manifest,
                "max_open_files"_a = instance.max_open_files,
                "max_file_opening_threads"_a = instance.max_file_opening_threads,
                "max_total_wal_size"_a = instance.max_total_wal_size, 
                "use_fsync"_a = instance.use_fsync,
                "db_log_dir"_a = instance.db_log_dir, 
                "wal_dir"_a = instance.wal_dir,
                "delete_obsolete_files_period_micros"_a = instance.delete_obsolete_files_period_micros,
                "max_background_jobs"_a = instance.max_background_jobs,
                "max_background_compactions"_a = instance.max_background_compactions,
                "max_subcompactions"_a = instance.max_subcompactions,
                "max_background_flushes"_a = instance.max_background_flushes,
                "max_log_file_size"_a = instance.max_log_file_size,
                "log_file_time_to_roll"_a = instance.log_file_time_to_roll,
                "keep_log_file_num"_a = instance.keep_log_file_num,
                "recycle_log_file_num"_a = instance.recycle_log_file_num,
                "max_manifest_file_size"_a = instance.max_manifest_file_size,
                "table_cache_numshardbits"_a = instance.table_cache_numshardbits,
                "WAL_ttl_seconds"_a = instance.WAL_ttl_seconds, 
                "WAL_size_limit_MB"_a = instance.WAL_size_limit_MB,
                "manifest_preallocation_size"_a = instance.manifest_preallocation_size,
                "allow_mmap_reads"_a = instance.allow_mmap_reads, 
                "allow_mmap_writes"_a = instance.allow_mmap_writes,
                "use_direct_reads"_a = instance.use_direct_reads,
                "use_direct_io_for_flush_and_compaction"_a = instance.use_direct_io_for_flush_and_compaction,
                "allow_fallocate"_a = instance.allow_fallocate, 
                "is_fd_close_on_exec"_a = instance.is_fd_close_on_exec,
                "stats_dump_period_sec"_a = instance.stats_dump_period_sec,
                "stats_persist_period_sec"_a = instance.stats_persist_period_sec,
                "persist_stats_to_disk"_a = instance.persist_stats_to_disk,
                "stats_history_buffer_size"_a = instance.stats_history_buffer_size,
                "advise_random_on_open"_a = instance.advise_random_on_open,
                "db_write_buffer_size"_a = instance.db_write_buffer_size,
                "compaction_readahead_size"_a = instance.compaction_readahead_size,
                "random_access_max_buffer_size"_a = instance.random_access_max_buffer_size,
                "writable_file_max_buffer_size"_a = instance.writable_file_max_buffer_size,
                "use_adaptive_mutex"_a = instance.use_adaptive_mutex, 
                "bytes_per_sync"_a = instance.bytes_per_sync,
                "wal_bytes_per_sync"_a = instance.wal_bytes_per_sync,
                "strict_bytes_per_sync"_a = instance.strict_bytes_per_sync,
                "enable_thread_tracking"_a = instance.enable_thread_tracking,
                "delayed_write_rate"_a = instance.delayed_write_rate,
                "enable_pipelined_write"_a = instance.enable_pipelined_write,
                "unordered_write"_a = instance.unordered_write,
                "allow_concurrent_memtable_write"_a = instance.allow_concurrent_memtable_write,
                "enable_write_thread_adaptive_yield"_a = instance.enable_write_thread_adaptive_yield,
                "max_write_batch_group_size_bytes"_a = instance.max_write_batch_group_size_bytes,
                "write_thread_max_yield_usec"_a = instance.write_thread_max_yield_usec,
                "write_thread_slow_yield_usec"_a = instance.write_thread_slow_yield_usec,
                "skip_stats_update_on_db_open"_a = instance.skip_stats_update_on_db_open,
                "skip_checking_sst_file_sizes_on_db_open"_a = instance.skip_checking_sst_file_sizes_on_db_open,
                "allow_2pc"_a = instance.allow_2pc,
                "fail_if_options_file_error"_a = instance.fail_if_options_file_error,
                "dump_malloc_stats"_a = instance.dump_malloc_stats,
                "avoid_flush_during_recovery"_a = instance.avoid_flush_during_recovery,
                "avoid_flush_during_shutdown"_a = instance.avoid_flush_during_shutdown,
                "allow_ingest_behind"_a = instance.allow_ingest_behind,
                "two_write_queues"_a = instance.two_write_queues, 
                "manual_wal_flush"_a = instance.manual_wal_flush,
                "atomic_flush"_a = instance.atomic_flush,
                "avoid_unnecessary_blocking_io"_a = instance.avoid_unnecessary_blocking_io,
                "write_dbid_to_manifest"_a = instance.write_dbid_to_manifest,
                "log_readahead_size"_a = instance.log_readahead_size,
                "best_efforts_recovery"_a = instance.best_efforts_recovery,
                "max_bgerror_resume_count"_a = instance.max_bgerror_resume_count,
                "bgerror_resume_retry_interval"_a = instance.bgerror_resume_retry_interval,
                "allow_data_in_errors"_a = instance.allow_data_in_errors, 
                "db_host_id"_a = instance.db_host_id,
                "enforce_single_del_contracts"_a = instance.enforce_single_del_contracts,
                "compaction_verify_record_count"_a = instance.compaction_verify_record_count,
                "write_identity_file"_a = instance.write_identity_file,
                "prefix_seek_opt_in_only"_a = instance.prefix_seek_opt_in_only,
                // "lowest_used_cache_tier"_a = instance.lowest_used_cache_tier,
                // "wal_filter"_a = instance.wal_filter,
                "wal_recovery_mode"_a = instance.wal_recovery_mode,
                // "wal_compression"_a = instance.wal_compression,
                "info_log_level"_a = instance.info_log_level,
                // "file_checksum_gen_factory"_a = instance.file_checksum_gen_factory,
                "follower_refresh_catchup_period_ms"_a = instance.follower_refresh_catchup_period_ms,
                "follower_catchup_retry_count"_a = instance.follower_catchup_retry_count,
                "wal_write_temperature"_a = instance.wal_write_temperature,
                "metadata_write_temperature"_a = instance.metadata_write_temperature,
                "background_close_inactive_wals"_a = instance.background_close_inactive_wals,
                "follower_catchup_retry_wait_ms"_a = instance.follower_catchup_retry_wait_ms
            );
        });

    py::class_<DbOpenBase>(m, "DbOpenBase");
    py::class_<DbOpenRW, DbOpenBase>(m, "DbOpenRW")
        .def(py::init());
    py::class_<DbOpenRO, DbOpenBase>(m, "DbOpenRO")
        .def(py::init());
    /* TODO: Unfinished
    py::class_<DbOpenTTL>(m, "DbOpenTTL")
        .def(py::init<uint64_t>());
    py::class_<DbOpenSecondary>(m, "DbOpenSecondary")
        .def(py::init());
    py::class_<DBOpenOptimisticTransation>(m, "DBOpenOptimisticTransation")
        .def(py::init());
    py::class_<DBOpenTransation>(m, "DBOpenTransation")
        .def(py::init());
    */

    // Register DB class
    py::class_<DBWrapper>(m, "cDB")
        .def_static("open", &DBWrapper::open)
        .def("get_column_family", &DBWrapper::get_column_family)
        .def("put", &DBWrapper::put)
        .def("get", &DBWrapper::get)
        .def("delete", &DBWrapper::delete_key)
        .def("write", &DBWrapper::write)
        .def("compact_range", &DBWrapper::compact_range)
        .def("create_iterator", &DBWrapper::create_iterator, py::keep_alive<0, 1>())
        .def("create_snapshot", &DBWrapper::create_snapshot)
        .def("release_snapshot", &DBWrapper::release_snapshot)
        .def("close", &DBWrapper::close)
        .def("list_column_families", &DBWrapper::list_column_families)
        .def("get_column_families", &DBWrapper::get_column_families);

    // Register Iterator class
    py::class_<IteratorWrapper>(m, "cIterator")
        .def("seek_to_first", &IteratorWrapper::seek_to_first)
        .def("seek_to_last", &IteratorWrapper::seek_to_last)
        .def("seek", &IteratorWrapper::seek)
        .def("valid", &IteratorWrapper::valid)
        .def("next", &IteratorWrapper::next)
        .def("prev", &IteratorWrapper::prev)
        .def("key", &IteratorWrapper::key)
        .def("value", &IteratorWrapper::value)
        .def("close", &IteratorWrapper::close);

    // Register WriteBatch class
    py::class_<WriteBatchWrapper>(m, "cWriteBatch")
        .def(py::init<>())
        .def("put", &WriteBatchWrapper::put)
        .def("delete", &WriteBatchWrapper::delete_key)
        .def("clear", &WriteBatchWrapper::clear)
        .def("count", &WriteBatchWrapper::count);

    // Register enums
    py::enum_<rocksdb::CompressionType>(m, "CompressionType")
        .value("NO_COMPRESSION", rocksdb::CompressionType::kNoCompression)
        .value("SNAPPY_COMPRESSION", rocksdb::CompressionType::kSnappyCompression)
        .value("ZLIB_COMPRESSION", rocksdb::CompressionType::kZlibCompression)
        .value("LZ4_COMPRESSION", rocksdb::CompressionType::kLZ4Compression)
        .value("LZ4HC_COMPRESSION", rocksdb::CompressionType::kLZ4HCCompression)
        .value("ZSTD_COMPRESSION", rocksdb::CompressionType::kZSTD)
        .export_values();
}
