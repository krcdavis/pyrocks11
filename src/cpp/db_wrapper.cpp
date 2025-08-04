#include "db_wrapper.h"
#include "helpers.h"
#include <rocksdb/db.h>
#include <rocksdb/options.h>
#include <rocksdb/utilities/db_ttl.h>

#include <stdexcept>
#include <optional>

#include <iostream>


DBWrapper::DBWrapper(rdb::DB* db, const std::vector<rdb::ColumnFamilyDescriptor>& cf_desc, 
    const std::vector<rdb::ColumnFamilyHandle*>& handles) : db(db) 
{
    default_cfh = nullptr;
    for(size_t i=0; i < handles.size(); i++) {
        if(cf_desc[i].name == rdb::kDefaultColumnFamilyName)
            default_cfh = handles[i];
        cfh.emplace(cf_desc[i].name, ColumnFamilyHandle(handles[i], db));
    }
    
    if(default_cfh == nullptr)
        default_cfh = handles[0];
}

DBWrapper::~DBWrapper() {
    close();
}

std::vector<rdb::ColumnFamilyDescriptor> helper_parse_colum_families_opt(const py::object& column_families) {
    std::vector<rdb::ColumnFamilyDescriptor> cf_desc;

    if (py::isinstance<py::dict>(column_families)) {        
        py::dict dict = column_families.cast<py::dict>();
        cf_desc.reserve(dict.size());

        for (const auto& item : dict) {
            if(!py::isinstance<py::str>(item.first) || !py::isinstance<rdb::ColumnFamilyOptions>(item.second)) {
                throw py::type_error("column_families must be either cCFOptions or dict[str, cCFOptions]");
            }

            const std::string& key = item.first.cast<std::string>();
            const rdb::ColumnFamilyOptions& value = item.second.cast<rdb::ColumnFamilyOptions>();
            cf_desc.emplace_back(key, value);
        }
    }
    else if (py::isinstance<rdb::ColumnFamilyOptions>(column_families)) {
        cf_desc.reserve(1);
        const rdb::ColumnFamilyOptions& cf_options = column_families.cast<rdb::ColumnFamilyOptions>();
        cf_desc.emplace_back(rdb::kDefaultColumnFamilyName, cf_options);
    } 
    else {
        throw py::type_error("column_families must be either cCFOptions or dict[str, cCFOptions]");
    }

    if(cf_desc.size() == 0) {
        throw py::type_error("dict[str, cCFOptions must not be empty");
    }

    return cf_desc;
}

std::unique_ptr<DBWrapper> DBWrapper::open(const std::string& path, const rdb::DBOptions& db_options, 
    py::object& column_families, const DbOpenBase& access_type) {
    std::vector<rdb::ColumnFamilyDescriptor> cf_desc = helper_parse_colum_families_opt(column_families);
    std::vector<rdb::ColumnFamilyHandle*> handles;
    handles.reserve(cf_desc.size());
    rdb::DB* db;

    rocksdb::Status status;
    switch(access_type.type) {
    case DbOpenType::RW:
        status = rdb::DB::Open(db_options, path, cf_desc, &handles, &db);
        break;
    case DbOpenType::RO:
        status = rdb::DB::OpenForReadOnly(db_options, path, cf_desc, &handles, &db);
        break;
    }
    
    if (!status.ok()) {
        throw std::runtime_error("Failed to open database: " + status.ToString());
    }

    if(handles.size() != cf_desc.size()) {
        delete db;
        throw std::runtime_error("Got a different number of handles from db than descriptors.");
    }

    return std::unique_ptr<DBWrapper>(new DBWrapper(db, cf_desc, handles));
}


ColumnFamilyHandle DBWrapper::get_column_family(const char* name) {
    auto s = cfh.find(name); 
    if(s == cfh.end())
        throw std::runtime_error("Column Family not found");

    return s->second;
}

void DBWrapper::put(ColumnFamilyHandle cfh, const py::bytes& key, const py::bytes& value) {
    if(!cfh.check_db(db.get())){
        throw std::runtime_error("Invalid column family");
    }

    rocksdb::Status status = db->Put(rocksdb::WriteOptions(), cfh.get_cf_handle(), toslice(key), toslice(value));
    if (!status.ok()) {
        throw std::runtime_error("Failed to put key-value: " + status.ToString());
    }
}

std::optional<py::bytes> DBWrapper::get(ColumnFamilyHandle cfh, const py::bytes& key) {
    if(!cfh.check_db(db.get())){
        throw std::runtime_error("Invalid column family");
    }

    std::string value;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(), cfh.get_cf_handle(),  toslice(key), &value);
    
    std::optional<py::bytes> rv;
    if(status.ok()) {
        rv = std::move(value);
    }
    else if(status.IsNotFound()) {
        rv = std::nullopt;
    } 
    else {
        throw std::runtime_error("Failed to get value: " + status.ToString());
    }
    return rv;
}

void DBWrapper::delete_key(ColumnFamilyHandle cfh, const py::bytes& key) {
    if(!cfh.check_db(db.get())){
        throw std::runtime_error("Invalid column family");
    }

    rocksdb::Status status = db->Delete(rocksdb::WriteOptions(), cfh.get_cf_handle(), toslice(key));
    if (!status.ok()) {
        throw std::runtime_error("Failed to delete key: " + status.ToString());
    }
}

void DBWrapper::write(const WriteBatchWrapper& batch) {
    rocksdb::Status status = db->Write(rocksdb::WriteOptions(), batch.get_batch());
    if (!status.ok()) {
        throw std::runtime_error("Failed to write batch: " + status.ToString());
    }
}

void DBWrapper::compact_range(const rdb::CompactRangeOptions& opt, const std::optional<py::bytes>& from_key, const std::optional<py::bytes>& to_key) {
    std::optional<rdb::Slice> slice_from, slice_to;
    if (from_key)
        slice_from.emplace(toslice(from_key.value()));

    if (to_key)
        slice_to.emplace(toslice(to_key.value()));
    
    rocksdb::Status status = db->CompactRange(opt, 
                                             slice_from ? &slice_from.value() : nullptr,
                                             slice_to ? &slice_to.value() : nullptr);
    if (!status.ok()) {
        throw std::runtime_error("CompactRange failed " + status.ToString());
    }
}

std::unique_ptr<IteratorWrapper> DBWrapper::create_iterator(ColumnFamilyHandle cfh) {
    if(!cfh.check_db(db.get())){
        throw std::runtime_error("Invalid column family");
    }

    return std::make_unique<IteratorWrapper>(db->NewIterator(rocksdb::ReadOptions(), cfh.get_cf_handle()));
}

const rocksdb::Snapshot* DBWrapper::create_snapshot() {
    return db->GetSnapshot();
}

void DBWrapper::release_snapshot(const rocksdb::Snapshot* snapshot) {
    db->ReleaseSnapshot(snapshot);
}

void DBWrapper::close() {
    if(db != nullptr)
        db->Close();
    db.reset();
}



std::unordered_map<std::string, ColumnFamilyHandle> DBWrapper::list_column_families() {
    return( cfh );
}


//static
//should probably return status, pass column family results by reference?
//unique ptr?

//rdb::ColumnFamilyOptions

std::unique_ptr<std::vector<std::string>> DBWrapper::get_column_families(const std::string& dbname, const rdb::DBOptions& db_options) {

    std::vector<std::string>* results = new std::vector<std::string>();

//options, &string name, std::vector<std::string>*- not unique pointer.
    auto sts = rdb::DB::ListColumnFamilies( db_options, dbname, results );

    std::unique_ptr<std::vector<std::string>> results2 = std::make_unique<std::vector<std::string>>(*results);//yayy.
    std::cout << results2->at(0) << std::endl;//yeah...

    return(results2);//.
}
