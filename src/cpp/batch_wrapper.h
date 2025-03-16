#pragma once
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <rocksdb/write_batch.h>
#include <string>
#include <memory>
#include "cf_handle.h"

namespace py  = pybind11;

class WriteBatchWrapper {
public:
    WriteBatchWrapper();
    
    void put(ColumnFamilyHandle cfh, const std::string& key, const std::string& value);
    void delete_key(ColumnFamilyHandle cfh, const std::string& key);
    void clear();
    int count() const;
    
    rocksdb::WriteBatch* get_batch() const;
    
private:
    std::unique_ptr<rocksdb::WriteBatch> batch_;
    rdb::DB* batch_db = nullptr;
};
