
#ifndef _LRUCACHE_TEST_HASHLRUCACHETEST_H_
#define _LRUCACHE_TEST_HASHLRUCACHETEST_H_

#include "HashLruCache.h"

#include <gtest/gtest.h>

class HashLruCacheTemp : public HashLruCache {
public:
    HashLruCacheTemp(const long hashMod) : HashLruCache(hashMod){}
    virtual ~HashLruCacheTemp(){}

    Handle* testPut(const Slice& key, void* value, size_t charge, void (*deleter)(const Slice&, void* value))
    {
        return put(key, value, charge, deleter);
    }

    Handle* testGet(const Slice& inKey)
    {
        return get(inKey);
    }

    int testRemove(const Slice& key)
    {
        return remove(key);
    }

    int testPutPtr(const std::string& key, void* value, size_t charge, void (*deleter)(const Slice&, void* value), std::shared_ptr<const Handle>& sptr)
    {
        return put(key, value, charge, deleter, sptr);
    }

};

class HashLruCacheTest : public testing::Test {
public:
    HashLruCacheTest();
    virtual ~HashLruCacheTest();

    virtual void SetUp();
    virtual void TearDown();
    static void SetUpTestCase();
    static void TearDownTestCase();

    HashLruCacheTemp* m_cacheTemp;
};

#endif /* _LRUCACHE_TEST_HASHLRUCACHETEST_H_ */
