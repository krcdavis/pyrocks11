#pragma once
#include <rocksdb/db.h>
#include <rocksdb/options.h>
#include <rocksdb/write_batch.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <memory>
#include <optional>
#include <unordered_map>
#include "cf_handle.h"
#include "iterator_wrapper.h"
#include "batch_wrapper.h"
#include "db_open_types.h"

namespace py  = pybind11;
namespace rdb = rocksdb;

class DBWrapper {
public:
    ~DBWrapper();
    static std::unique_ptr<DBWrapper> open(const std::string& path, 
        const rdb::DBOptions& db_options,  py::object& column_families, const DbOpenBase& access_type);
    
    ColumnFamilyHandle get_column_family(const char* name);
    void put(ColumnFamilyHandle cfh, const py::bytes& key, const py::bytes& value);
    std::optional<py::bytes> get(ColumnFamilyHandle cfh, const py::bytes& key);
    void delete_key(ColumnFamilyHandle cfh, const py::bytes& key);
    void write(const WriteBatchWrapper& batch);

    void compact_range(const rdb::CompactRangeOptions& opt, const std::optional<py::bytes>& from_key, const std::optional<py::bytes>& to_key);

    std::unique_ptr<IteratorWrapper> create_iterator(ColumnFamilyHandle cfh);

    const rdb::Snapshot* create_snapshot();
    void release_snapshot(const rdb::Snapshot* snapshot);
    
    void close();

    char* list_column_families();
    
private:
    DBWrapper(rdb::DB* db, const std::vector<rdb::ColumnFamilyDescriptor>& cf_desc, const std::vector<rdb::ColumnFamilyHandle*>& handles);
    std::unique_ptr<rdb::DB> db;
    std::unordered_map<std::string, ColumnFamilyHandle> cfh;
    rdb::ColumnFamilyHandle* default_cfh;
};
