
#include "HashLruCache.h"

#include <stdexcept>

#define HASHMOD_NUM      30
static uint64_t g_hashMod[HASHMOD_NUM] = {
        5ul,          7ul,          53ul,          97ul,          193ul,
        389ul,        769ul,        1543ul,        3079ul,        6151ul,
        12289ul,      24593ul,      49157ul,       98317ul,       196613ul,
        393241ul,     786433ul,     1572869ul,     3145739ul,     6291469ul,
        12582917ul,  25165843ul,   50331653ul,    100663319ul,   201326611ul,
        402653189ul, 805306457ul,  1610612741ul,  3221225473ul,  4294967291ul
};

Cache::~Cache() {
}

Cache* NewLRUCache(const uint64_t hashMod) {
    return new HashLruCache(hashMod);
}

HashLruCache::HashLruCache(const uint64_t hashMod)
{
    m_hashMod = 0;
    for(int i = 0; i < HASHMOD_NUM; ++i) {
        if(g_hashMod[i] > hashMod) {
            m_hashMod = g_hashMod[i];
            break;
        }
    }

    if(m_hashMod == 0) {
        throw std::runtime_error("error hashmod.");
    }

    for(long i = 0; i < m_hashMod; ++i) {
        HASH_UNIT_S* hashUnit = new HASH_UNIT_S;
        m_shardCache.push_back(hashUnit);
    }
    m_bucketSize = -1;
    m_hotFlag = false;
    m_aliveTime = -1;
    m_cleanTime = -1;
}

HashLruCache::~HashLruCache() {
    for(size_t i = 0; i < m_shardCache.size(); ++i) {
        if(m_shardCache[i]) {
            delete m_shardCache[i];
            m_shardCache[i] = NULL;
        }
    }
}

void HashLruCache::setBucketSize(const int bucketSize)
{
    m_bucketSize = bucketSize;
}

void HashLruCache::setHotdataFlag(const bool flag)
{
    m_hotFlag = flag;
}

void HashLruCache::setAliveTime(const long aliveTime)
{
    m_aliveTime = aliveTime;
}

void HashLruCache::setCleanTime(const long cleanTime)
{
    m_cleanTime = cleanTime;
}

int HashLruCache::initCache()
{
    for(int i = 0; i < m_hashMod; ++i) {
        if(m_shardCache[i]->cache)
            m_shardCache[i]->cache = new LruCache(m_hotFlag, m_aliveTime, m_cleanTime, m_bucketSize);
    }

    return STATUS_SUCCEED;
}

int HashLruCache::getCacheSize()
{
    int num = 0;
    for(size_t i = 0; i < m_shardCache.size(); ++i) {
        if(m_shardCache[i]->cache) {
            num += m_shardCache[i]->cache->getMapSize();
        }
    }

    return num;
}

int HashLruCache::getMaxBucketSize()
{
    int maxNum = 0;
    for(size_t i = 0; i < m_shardCache.size(); ++i) {
        if(m_shardCache[i]->cache) {
            maxNum = m_shardCache[i]->cache->getMapSize() > maxNum ?
                    m_shardCache[i]->cache->getMapSize() : maxNum;
        }
    }

    return maxNum;
}

int HashLruCache::getBucketSize(int index)
{
    int num = 0;
    if(index >= m_hashMod) {
        return STATUS_FAILED;
    }

    if(m_shardCache[index]->cache){
        num = m_shardCache[index]->cache->getMapSize();
    }

    return num;
}

Cache::Handle* HashLruCache::put(const Slice& key, void* value, size_t charge, void (*deleter)(const Slice& key, void* value))
{
    LruNode* e = NULL;
    uint64_t hash;

    hash = HashSlice(key);
    HashUnit * hashUnit = m_shardCache[Shard(hash)];

    if(hashUnit->cache == NULL) {
        hashUnit->cache = new LruCache(m_hotFlag,
                m_aliveTime, m_cleanTime, m_bucketSize);
    }

    e = hashUnit->cache->put(hash, key, value, charge, deleter);

    return reinterpret_cast<Cache::Handle*>(e);
}

Cache::Handle* HashLruCache::get(const Slice& inKey)
{
    LruNode* e = NULL;
    uint64_t hash;
    
    hash = HashSlice(inKey);
    HashUnit* hashUnit = m_shardCache[Shard(hash)];

    if(hashUnit->cache != NULL) {
        e = hashUnit->cache->get(hash, inKey);
    }

    return reinterpret_cast<Cache::Handle*>(e);
}


int HashLruCache::Release(Cache::Handle* e)
{
    int res = STATUS_FAILED;
    if(e == NULL) {
        return res;
    }

    LruNode* h = reinterpret_cast<LruNode*>(e);
    HashUnit* hashUnit = m_shardCache[Shard(h->hash)];

    if(hashUnit->cache == NULL) {
        res = BUCKET_NOKEY;
    } else {
        hashUnit->cache->Release(h);
        res = STATUS_SUCCEED;
    }

    return res;
}

int HashLruCache::remove(const Slice& key)
{
    int res = STATUS_FAILED;
    uint64_t hash;
    
    hash = HashSlice(key);
    HashUnit* hashUnit = m_shardCache[Shard(hash)];

    if(hashUnit->cache == NULL) {
        res = BUCKET_NOKEY;
    } else {
        res = hashUnit->cache->remove(hash, key);
    }

    return res;
}

void DeleteCharBuffer(const Slice& key, void* value)
{
    char* block = reinterpret_cast<char*>(value);
    delete[] block;
    block = NULL;
}

int HashLruCache::put(const std::string& key, const std::string& value)
{
    int res = STATUS_FAILED;
    uint64_t hash;
    LruNode* e = NULL;
    Slice tmpKey(key.c_str(), key.size());

    char* value_buf = new char[value.size()];
    memcpy(value_buf, value.c_str(), value.size());

    void* tmpValue = value_buf;
    size_t charge = value.size();

    hash = HashSlice(tmpKey);
    HashUnit* hashUnit = m_shardCache[Shard(hash)];

    if(hashUnit->cache == NULL) {
        hashUnit->cache = new LruCache(m_hotFlag,
                m_aliveTime, m_cleanTime, m_bucketSize);
    }

    e = hashUnit->cache->put(hash, tmpKey, tmpValue, charge, &DeleteCharBuffer);
    hashUnit->cache->Release(e);
    if(e)
        res = STATUS_SUCCEED;

    return res;
}

int HashLruCache::get(const std::string& inKey, std::string& value)
{
    int res = STATUS_FAILED;
    uint64_t hash = 0;
    Slice tmpKey(inKey.c_str(), inKey.size());

    hash = HashSlice(tmpKey);
    HashUnit* hashUnit = m_shardCache[Shard(hash)];
    
    LruNode* e = NULL;

    if(hashUnit->cache == NULL) {
        res = BUCKET_NOKEY;
    } else {
        e = hashUnit->cache->get(hash, tmpKey);
        if(e) {
            res = STATUS_SUCCEED;
            value = std::string((char*)e->value, e->charge);
            hashUnit->cache->Release(e);
        }
    }

    return res;
}

void ReleaseSharedPtr(Cache::Handle* e)
{
    if(e)
    {
        LruNode* node = reinterpret_cast<LruNode*> (e);
        if(node->ptr) {
            HashLruCache* pThis = reinterpret_cast<HashLruCache*> (node->ptr);
            pThis->Release(e);
        }
    }
}

int HashLruCache::put(const std::string& key, void* value, size_t charge, void (*deleter)(const Slice& key, void* value), std::shared_ptr<const Cache::Handle>& sptr)
{
    int res = STATUS_FAILED;
    uint64_t hash;
    LruNode* e = NULL;
    Slice tmpKey(key.c_str(), key.size());

    hash = HashSlice(tmpKey);
    HashUnit* hashUnit = m_shardCache[Shard(hash)];

    if(hashUnit->cache == NULL) {
        hashUnit->cache = new LruCache(m_hotFlag,
                m_aliveTime, m_cleanTime, m_bucketSize);
    }

    e = hashUnit->cache->put(hash, tmpKey, value, charge, deleter, this);
    if(e)
    {
        sptr.reset(reinterpret_cast<Cache::Handle*>(e), ReleaseSharedPtr);
        res = STATUS_SUCCEED;
    }
    else
    {
        sptr.reset();
    }

    return res;
}

int HashLruCache::get(const std::string& inKey, std::shared_ptr<const Cache::Handle>& sptr)
{
    int res = STATUS_FAILED;
    uint64_t hash = 0;
    Slice tmpKey(inKey.c_str(), inKey.size());

    hash = HashSlice(tmpKey);
    HashUnit* hashUnit = m_shardCache[Shard(hash)];
    LruNode* e = NULL;

    if(hashUnit->cache == NULL) {
        sptr.reset();
        res = BUCKET_NOKEY;
    } else {
        e = hashUnit->cache->get(hash, tmpKey);
        if(e) {
            res = STATUS_SUCCEED;
            sptr.reset(reinterpret_cast<Cache::Handle*>(e), ReleaseSharedPtr);
        }
        else {
            sptr.reset();
        }
    }

    return res;
}

int HashLruCache::remove(const std::string& key)
{
    int res = STATUS_FAILED;
    Slice tmpKey(key.c_str(), key.size());
    uint64_t hash = 0;
    
    hash = HashSlice(tmpKey);
    HashUnit* hashUnit = m_shardCache[Shard(hash)];

    if(hashUnit->cache == NULL) {
        res = BUCKET_NOKEY;
    } else {
        res = hashUnit->cache->remove(hash, tmpKey);
    }

    return res;
}

void* HashLruCache::value(Cache::Handle* e)
{
    return reinterpret_cast<LruNode*>(e)->value;
}

