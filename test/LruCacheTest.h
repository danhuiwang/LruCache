
#ifndef _LRUCACHE_TEST_LRUCACHETEST_H_
#define _LRUCACHE_TEST_LRUCACHETEST_H_

#include "LruCache.h"

#include <gtest/gtest.h>

class LruCacheTemp : public LruCache{
public:
    LruCacheTemp(const bool hotFlag, const long aliveTime,
            const long cleanTime, const int bucketSize)
    : LruCache(hotFlag, aliveTime, cleanTime, bucketSize)
    {}
    virtual ~LruCacheTemp(){}

    LruNode* testPut(const uint64_t hash, Slice& key, void* value, size_t charge, void (*deleter)(const Slice&, void* value))
    {
        return put(hash, key, value, charge, deleter);
    }

    LruNode* testGet(const uint64_t hash, Slice& inKey)
    {
        return get(hash, inKey);
    }

    int testRemove(const uint64_t hash, Slice& key)
    {
        return remove(hash, key);
    }

    int testGetMapSize()
    {
        return getMapSize();
    }

    int testCleanMap(){
        return cleanMap();
    }

    int testInsertToUsedList(LruNode* object)
    {
        return insertToUsedList(object);
    }

    int testEraseFromUsedList(LruNode* object)
    {
        return eraseFromUsedList(object);
    }
};

class LruCacheTest : public testing::Test {
public:
    LruCacheTest();
    virtual ~LruCacheTest();

    virtual void SetUp();
    virtual void TearDown();
    static void SetUpTestCase();
    static void TearDownTestCase();

    LruCacheTemp* m_LruCacheTemp;
};

#endif /* _LRUCACHE_TEST_LRUCACHETEST_H_ */
