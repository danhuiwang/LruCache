
#ifndef _LRUCACHE_INCLUDE_CACHE_H_
#define _LRUCACHE_INCLUDE_CACHE_H_

#include "slice.h"

#include <memory>
#include <vector>
#include <string>
#include <stdint.h>

class Cache;
extern Cache* NewLRUCache(const uint64_t hashMod);

class Cache {
public:
    Cache() = default;
    Cache(const Cache&) = delete;
    //Cache& operator=(const Cache&) = delete;

    virtual ~Cache();
    // Opaque handle to an entry stored in the cache.

    struct Handle { };

    virtual void setBucketSize(const int bucketSize) = 0;
    virtual void setHotdataFlag(const bool flag) = 0;
    virtual void setAliveTime(const long aliveTime) = 0;
    virtual void setCleanTime(const long cleanTime) = 0;
    virtual int initCache() = 0;

    virtual int put(const std::string& key, void* value, size_t charge, void (*deleter)(const Slice& key, void* value), std::shared_ptr<const Handle>& sptr) = 0;
    virtual int get(const std::string& inKey, std::shared_ptr<const Handle>& sptr) = 0;

    virtual Handle* put(const Slice& key, void* value, size_t charge, void (*deleter)(const Slice& key, void* value)) = 0;
    virtual Handle* get(const Slice& inKey) = 0;
    virtual int remove(const Slice& key) = 0;


    virtual int put(const std::string& key, const std::string& value) = 0;
    virtual int get(const std::string& inKey, std::string& value) = 0;
    virtual int remove(const std::string& key) = 0;

    virtual int Release(Handle* e) = 0;
    virtual void* value(Handle* e) = 0;
};

#endif /* _LRUCACHE_INCLUDE_CACHE_H_ */
