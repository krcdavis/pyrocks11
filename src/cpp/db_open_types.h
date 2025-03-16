#pragma once
#include <inttypes.h>
#include <string>
#include <stdio.h>

enum class DbOpenType : int {
    RW,
    RO,
    TTL,
    SECONDARY,
    OPTIMISTIC_TRANSACTION,
    TRANSACTION
};

struct DbOpenBase {
    DbOpenType type;
    virtual ~DbOpenBase() = default;
};

struct DbOpenRW : DbOpenBase {
    DbOpenRW() { type = DbOpenType::RW; }
};

struct DbOpenRO : DbOpenBase {
    DbOpenRO() { type = DbOpenType::RO; }
};

//TODO : UNFINISHED
struct DbOpenTTL : DbOpenBase {
    DbOpenTTL(uint64_t ttl, bool read_only) : type(DbOpenType::TTL), ttl(ttl), read_only(read_only) {}
    DbOpenType type;
    int ttl;
    bool read_only;
};

struct DbOpenSecondary : DbOpenBase {
    DbOpenType type;
    std::string name;
    DbOpenSecondary(const std::string& name) : type(DbOpenType::SECONDARY), name(name) {}
};

struct DBOpenOptimisticTransation : DbOpenBase {
    DbOpenType type = DbOpenType::OPTIMISTIC_TRANSACTION;
};

struct DBOpenTransation : DbOpenBase {
    DbOpenType type = DbOpenType::TRANSACTION;

    //TODO : add arguments
};