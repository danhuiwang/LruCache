
#include "HashLruCacheTest.h"

HashLruCacheTest::HashLruCacheTest() {
    // TODO Auto-generated constructor stub

}

HashLruCacheTest::~HashLruCacheTest() {
    // TODO Auto-generated destructor stub
}

void HashLruCacheTest::SetUp()
{
    m_cacheTemp = new HashLruCacheTemp(7ul);
}

void HashLruCacheTest::TearDown()
{
    if(m_cacheTemp) {
        delete m_cacheTemp;
        m_cacheTemp = NULL;
    }
}

void HashLruCacheTest::SetUpTestCase()
{

}

void HashLruCacheTest::TearDownTestCase()
{

}

static void Delete(const Slice& key, void* value) {
    char* block = reinterpret_cast<char*>(value);
    delete[] block;
}

TEST_F(HashLruCacheTest, testPut)
{
    std::string key = "testKey";
    std::string valueTemp = "testValue";
    char* buf = new char[valueTemp.length()];

    memcpy(buf, valueTemp.c_str(), valueTemp.length());
    Slice keyDatum(const_cast<char*>(key.c_str()), key.size());

    void* value;
    size_t charge = valueTemp.length();
    value = buf;
    
    Cache::Handle* e = NULL;
    e = m_cacheTemp->testPut(keyDatum, value, charge, &Delete);
    EXPECT_NE(e, (Cache::Handle*)NULL);
    m_cacheTemp->Release(e);
    EXPECT_EQ(m_cacheTemp->getCacheSize(), 1);
    EXPECT_EQ(m_cacheTemp->getMaxBucketSize(), 1);
    e = m_cacheTemp->testGet(keyDatum);
    EXPECT_NE(e, (Cache::Handle*)NULL);
    m_cacheTemp->Release(e);

    std::shared_ptr<const Cache::Handle> sptr;
    char* tmp = new char[valueTemp.length()];
    memcpy(tmp, valueTemp.c_str(), valueTemp.length());
    value = tmp;

    EXPECT_EQ(m_cacheTemp->put("testkey1", value, charge, &Delete, sptr), STATUS_SUCCEED);
    //delete[] buf;
}

TEST_F(HashLruCacheTest, testGet)
{
    std::string key = "testKey";
    std::string valueTemp = "testValue";
    void* value = NULL;
    Slice keyDatum(const_cast<char*>(key.c_str()), key.size());
    char* buf = new char[valueTemp.length()];

    memcpy(buf, valueTemp.c_str(), valueTemp.length());
    value = buf;
    size_t charge = 0;

    Cache::Handle* e = NULL;
    e = m_cacheTemp->testGet(keyDatum);
    EXPECT_EQ(e, (Cache::Handle*)NULL);
    m_cacheTemp->Release(e);

    charge = valueTemp.length();
    e = m_cacheTemp->testPut(keyDatum, value, charge, &Delete);
    EXPECT_NE(e, (Cache::Handle*)NULL);
    m_cacheTemp->Release(e);
    EXPECT_EQ(m_cacheTemp->getCacheSize(), 1);
    EXPECT_EQ(m_cacheTemp->getMaxBucketSize(), 1);
    e = m_cacheTemp->testGet(keyDatum);
    EXPECT_NE(e, (Cache::Handle*)NULL);
    m_cacheTemp->Release(e);

    if(m_cacheTemp) {
        delete m_cacheTemp;
        m_cacheTemp = NULL;
    }

    m_cacheTemp = new HashLruCacheTemp(1ul);
    m_cacheTemp->setAliveTime(1);
    m_cacheTemp->setBucketSize(1);
    
    buf = new char[valueTemp.length()];
    memcpy(buf, valueTemp.c_str(), valueTemp.length());
    value = buf;

    charge = valueTemp.length();
    e = m_cacheTemp->testPut(keyDatum, value, charge, &Delete);
    EXPECT_NE(e, (Cache::Handle*)NULL);
    m_cacheTemp->Release(e);
    sleep(1);
    e = m_cacheTemp->testGet(keyDatum);
    EXPECT_EQ(e, (Cache::Handle*)NULL);
    m_cacheTemp->Release(e);
    std::string key1 = "testKey1";
    
    buf = new char[valueTemp.length()];
    memcpy(buf, valueTemp.c_str(), valueTemp.length());
    value = buf;
    charge = valueTemp.length();
    Slice key1Datum(const_cast<char*>(key1.c_str()), key1.size());
    e = m_cacheTemp->testPut(key1Datum, value, charge, &Delete);
    EXPECT_NE(e, (Cache::Handle*)NULL);
    m_cacheTemp->Release(e);
    e = m_cacheTemp->testGet(keyDatum);
    EXPECT_EQ(e, (Cache::Handle*)NULL);
    m_cacheTemp->Release(e);
    //delete[] buf;
}

TEST_F(HashLruCacheTest, testRemove)
{
    std::string key = "testKey";
    std::string valueTemp = "testValue";
    Slice keyDatum(const_cast<char*>(key.c_str()), key.size());

    void* value;
    char* buf = new char[valueTemp.length()];
    memcpy(buf, valueTemp.c_str(), valueTemp.length());
    value = buf;
    size_t charge = valueTemp.length();
    
    EXPECT_EQ(m_cacheTemp->testRemove(keyDatum), BUCKET_NOKEY);
    Cache::Handle* e = NULL;
    e = m_cacheTemp->testPut(keyDatum, value, charge, &Delete);
    EXPECT_NE(e, (Cache::Handle*)NULL);
    m_cacheTemp->Release(e);
    EXPECT_EQ(m_cacheTemp->getCacheSize(), 1);
    EXPECT_EQ(m_cacheTemp->getMaxBucketSize(), 1);
    e = m_cacheTemp->testGet(keyDatum);
    EXPECT_NE(e, (Cache::Handle*)NULL);
    m_cacheTemp->Release(e);
    EXPECT_EQ(m_cacheTemp->testRemove(keyDatum), STATUS_SUCCEED);
    e = m_cacheTemp->testGet(keyDatum);
    EXPECT_EQ(e, (Cache::Handle*)NULL);
    m_cacheTemp->Release(e);
    //delete[] buf;
}

