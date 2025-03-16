#include "batch_wrapper.h"

WriteBatchWrapper::WriteBatchWrapper() : batch_(new rocksdb::WriteBatch()) {}

void WriteBatchWrapper::put(ColumnFamilyHandle cfh, const std::string& key, const std::string& value) {
    batch_->Put(cfh.get_cf_handle(), key, value);
}

void WriteBatchWrapper::delete_key(ColumnFamilyHandle cfh, const std::string& key) {
    batch_->Delete(cfh.get_cf_handle(), key);
}

void WriteBatchWrapper::clear() {
    batch_->Clear();
}

int WriteBatchWrapper::count() const {
    return batch_->Count();
}

rocksdb::WriteBatch* WriteBatchWrapper::get_batch() const {
    return batch_.get();
}
