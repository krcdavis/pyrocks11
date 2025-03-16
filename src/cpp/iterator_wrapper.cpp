#include "iterator_wrapper.h"
#include "helpers.h"
#include <stdexcept>

IteratorWrapper::IteratorWrapper(rocksdb::Iterator* iter) : iter_(iter) {}

void IteratorWrapper::seek_to_first() {
    check_db();
    iter_->SeekToFirst();
}

void IteratorWrapper::seek_to_last() {
    check_db();
    iter_->SeekToLast();
}

void IteratorWrapper::seek(const py::bytes& key) {
    check_db();
    iter_->Seek(toslice(key));
}

bool IteratorWrapper::valid() const {
    check_db();
    return iter_->Valid();
}

void IteratorWrapper::next() {
    check_db();
    iter_->Next();
}

void IteratorWrapper::prev() {
    check_db();
    iter_->Prev();
}

py::bytes IteratorWrapper::key() const {
    check_db();
    if (!valid()) {
        throw std::runtime_error("Iterator not valid");
    }
    return iter_->key().ToString();
}

py::bytes IteratorWrapper::value() const {
    check_db();
    if (!valid()) {
        throw std::runtime_error("Iterator not valid");
    }
    return iter_->value().ToString();
}
