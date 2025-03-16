#pragma once
#include <pybind11/pybind11.h>
#include <rocksdb/iterator.h>
#include <memory>
#include <string>

namespace py = pybind11;

class IteratorWrapper {
public:
    IteratorWrapper(rocksdb::Iterator* iter);
    ~IteratorWrapper() = default;
    
    void seek_to_first();
    void seek_to_last();
    void seek(const py::bytes& key);
    bool valid() const;
    void next();
    void prev();
    py::bytes key() const;
    py::bytes value() const;
    
    void check_db() const { if(!iter_) throw std::runtime_error("You cannot use this iterator. It has been already closed.");}
    void close() { iter_.reset(); }
private:
    std::unique_ptr<rocksdb::Iterator> iter_;
};
