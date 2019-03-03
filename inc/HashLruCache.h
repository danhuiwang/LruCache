
#ifndef _LRUCACHE_INCLUDE_HASHLRUCACHE_H_
#define _LRUCACHE_INCLUDE_HASHLRUCACHE_H_

#include "LruCache.h"
#include "LRU_Cache.h"
#include "LRU_Lock.h"

#include <vector>
#include <string>

#include <openssl/md5.h>
static const int kNumShardBits = 5;
static const int kNumShards = 1 << kNumShardBits;

typedef struct HashUnit{
    LruCache* cache;

    HashUnit(){
        cache = NULL;
    }

    ~HashUnit(){
        if(cache) {
            delete cache;
            cache = NULL;
        }
    }
}HASH_UNIT_S;

class HashLruCache : public Cache {
public:
    explicit HashLruCache(const uint64_t hashMod);
    virtual ~HashLruCache();

    virtual void setBucketSize(const int bucketSize);
    virtual void setHotdataFlag(const bool flag);
    virtual void setAliveTime(const long aliveTime);
    virtual void setCleanTime(const long cleanTime);
    virtual int initCache();

    int getCacheSize();
    int getMaxBucketSize();
    int getBucketSize(int index);

    virtual int put(const std::string& key, void* value, size_t charge, void (*deleter)(const Slice& key, void* value), std::shared_ptr<const Handle>& sptr);
    virtual int get(const std::string& inKey, std::shared_ptr<const Handle>& sptr);

    virtual Handle* put(const Slice& key, void* value, size_t charge, void (*deleter)(const Slice& key, void* value));
    virtual Handle* get(const Slice& inKey);
    virtual int remove(const Slice& key);


    virtual int put(const std::string& key, const std::string& value);
    virtual int get(const std::string& inKey, std::string& value);
    virtual int remove(const std::string& key);

    virtual int Release(Handle* e);
    virtual void* value(Handle* e);
private:
/*
    static inline uint32_t HashSlice(const Slice& s) {
        return LRU_Hash(s.data(), s.size(), 0);
    }

    static uint32_t Shard(uint32_t hash) {
        return hash >> (32 - kNumShardBits);
    }
*/
    static inline uint64_t HashSlice(const Slice& s) {
        char buf[BYTE16] = {0};
        MD5((uint8_t*)s.data(), (uint32_t)s.size(), (uint8_t*)buf);
        uint64_t hash = *(uint64_t*)buf;
        return hash;
    }

    uint32_t Shard(uint32_t hash) {
        return hash % m_hashMod;
    }
protected:
private:
    long m_hashMod;
    std::vector<HashUnit*> m_shardCache;
    int m_bucketSize;
    bool m_hotFlag;
    long m_aliveTime;
    long m_cleanTime;
};

#endif /* _LRUCACHE_INCLUDE_HASHLRUCACHE_H_ */
