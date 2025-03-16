#pragma once
#include <rocksdb/db.h>

namespace rdb = rocksdb;

class ColumnFamilyHandle {
public:
    ColumnFamilyHandle(rdb::ColumnFamilyHandle* cfh, rdb::DB* dbh) : 
        cfh(cfh), dbh(dbh) {}

    inline bool check_db(rdb::DB* target_dbh) {
        return dbh == target_dbh;
    }

    inline rdb::ColumnFamilyHandle* get_cf_handle() {
        return cfh;
    }

private:
    rdb::ColumnFamilyHandle* cfh;
    rdb::DB* dbh;
};