
#include "LruCacheTest.h"

LruCacheTest::LruCacheTest() {
    // TODO Auto-generated constructor stub

}

LruCacheTest::~LruCacheTest() {
    // TODO Auto-generated destructor stub
}

void LruCacheTest::SetUp()
{
    m_LruCacheTemp = new LruCacheTemp(true, 10, 20, 10);
}

void LruCacheTest::TearDown()
{
    if(m_LruCacheTemp) {
        delete m_LruCacheTemp;
        m_LruCacheTemp = NULL;
    }
}

void LruCacheTest::SetUpTestCase()
{

}

void LruCacheTest::TearDownTestCase()
{

}

static void Delete(const Slice& key, void* value) {
    char* block = reinterpret_cast<char*>(value);
    delete[] block;
}

TEST_F(LruCacheTest, testPut)
{
    void* value;

    char* keydata = new char[KBYTES1];
    memset(keydata, 0, KBYTES1);
    memcpy(keydata, "testKey", strlen("testKey"));
    Slice key(keydata, strlen("testKey"));

    char* buf = new char[KBYTES1];
    memset(buf, 0, KBYTES1);
    size_t charge = strlen("testValue");
    memcpy(buf, "testValue", charge);

    value = buf;
    LruNode* e = NULL;
    e = m_LruCacheTemp->testPut(123456, key, value, charge, &Delete);
    EXPECT_NE(e, (LruNode*)(LruNode*)NULL);
    m_LruCacheTemp->Release(e);

    //delete[] buf;
    delete[] keydata;
}

TEST_F(LruCacheTest, testGet)
{
    void* value;

    char* keydata = new char[KBYTES1];
    memset(keydata, 0, KBYTES1);
    memcpy(keydata, "testKey", strlen("testKey"));
    Slice key(keydata, strlen("testKey"));

    char* buf = new char[KBYTES1];
    memset(buf, 0, KBYTES1);
    size_t charge = strlen("testValue");
    memcpy(buf, "testValue", charge);

    value = buf;
    LruNode* e = NULL;
    e = m_LruCacheTemp->testPut(123456, key, value, charge, &Delete);
    m_LruCacheTemp->Release(e);
    EXPECT_NE(e, (LruNode*)NULL);
    e = m_LruCacheTemp->testGet(123456, key);
    EXPECT_NE(e, (LruNode*)NULL);
    m_LruCacheTemp->Release(e);
    e = m_LruCacheTemp->testGet(223456, key);
    EXPECT_EQ(e, (LruNode*)NULL);
    m_LruCacheTemp->Release(e);

    memcpy(keydata, "testErr", strlen("testErr"));
    Slice key1(keydata, strlen("testErr"));
    e = m_LruCacheTemp->testGet(123456, key1);
    EXPECT_EQ(e, (LruNode*)NULL);
    m_LruCacheTemp->Release(e);
    delete[] keydata;
    //delete[] buf;
}

TEST_F(LruCacheTest, testRemove)
{
    void* value;

    char* keydata = new char[KBYTES1];
    memset(keydata, 0, KBYTES1);
    memcpy(keydata, "testKey", strlen("testKey"));
    Slice key(keydata, strlen("testKey"));

    char* buf = new char[KBYTES1];
    memset(buf, 0, KBYTES1);
    size_t charge = strlen("testValue");
    memcpy(buf, "testValue", charge);

    value = buf;
    LruNode* e = NULL;
    e = m_LruCacheTemp->testPut(123456, key, value, charge, &Delete);
    EXPECT_NE(e, (LruNode*)NULL);
    m_LruCacheTemp->Release(e);
    EXPECT_EQ(m_LruCacheTemp->testRemove(223456, key), BUCKET_NOKEY);
    EXPECT_EQ(m_LruCacheTemp->testRemove(123456, key), STATUS_SUCCEED);
    e = m_LruCacheTemp->testGet(123456, key);
    EXPECT_EQ(e, (LruNode*)NULL);
    m_LruCacheTemp->Release(e);
    delete[] keydata;
    //delete[] buf;
}

TEST_F(LruCacheTest, testGetMapSize)
{
    void* value;

    char* keydata = new char[KBYTES1];
    memset(keydata, 0, KBYTES1);
    memcpy(keydata, "testKey", strlen("testKey"));
    Slice key(keydata, strlen("testKey"));

    char* buf = new char[KBYTES1];
    memset(buf, 0, KBYTES1);
    size_t charge = strlen("testValue");
    memcpy(buf, "testValue", charge);

    value = buf;
    LruNode* e = NULL;
    EXPECT_EQ(m_LruCacheTemp->testGetMapSize(), 0);
    e = m_LruCacheTemp->testPut(123456, key, value, charge, &Delete);
    EXPECT_NE(e, (LruNode*)NULL);
    m_LruCacheTemp->Release(e);
    EXPECT_EQ(m_LruCacheTemp->testGetMapSize(), 1);
    //delete[] buf;
    delete[] keydata;
}

TEST_F(LruCacheTest, testCleanMap)
{
    void* value;

    char* keydata = new char[KBYTES1];
    memset(keydata, 0, KBYTES1);
    memcpy(keydata, "testKey", strlen("testKey"));
    Slice key(keydata, strlen("testKey"));

    char* buf = new char[KBYTES1];
    memset(buf, 0, KBYTES1);
    size_t charge = strlen("testValue");
    memcpy(buf, "testValue", charge);

    if(m_LruCacheTemp) {
        delete m_LruCacheTemp;
    }
    m_LruCacheTemp = new LruCacheTemp(true, 1, 20, 10);

    value = buf;
    EXPECT_EQ(m_LruCacheTemp->testCleanMap(), STATUS_SUCCEED);
    LruNode* e = NULL;
    e = m_LruCacheTemp->testPut(123456, key, value, charge, &Delete);
    EXPECT_NE(e, (LruNode*)NULL);
    m_LruCacheTemp->Release(e);
    sleep(1);
    EXPECT_EQ(m_LruCacheTemp->testCleanMap(), STATUS_SUCCEED);
    EXPECT_EQ(m_LruCacheTemp->testGetMapSize(), 0);
    //delete[] buf;
    delete[] keydata;
}

