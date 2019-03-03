
#include <LruCache/LRU_Cache.h>
//#include "../inc/LRU_Cache.h"

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <memory>
using namespace std;

void Deleter(const Slice& key, void* value) {
    if(value){
        char* ptr = reinterpret_cast<char*>(value);
        delete ptr;
    }
}

int main() {
    int ret = 0;
    Cache*  cache = NewLRUCache(49157ul);
    //Cache*  cache(NewLRUCache(49157ul));
    cache->setBucketSize(10);
    cache->setHotdataFlag(true);
    cache->setAliveTime(10);
    cache->setCleanTime(100);

    std::string key("test");
    Slice datum(key.c_str(), key.length());

    Cache::Handle* e = NULL;
    void * value;

    char* buf = new char[32];
    sprintf(buf, "%s", "value");
    value = buf;

    e = cache->put(datum, value, 6, &Deleter);
    cache->Release(e);
    e = cache->get(datum);
    cache->Release(e);

    if(e == NULL) {
        printf("return: %d \n", ret);
    }
    else {
        printf("return: %d \tvalue: %s\n", ret, (char*)cache->value(e));
    }

    //cache->remove("test");
    cache->remove(datum);
    e = cache->get(datum);
    cache->Release(e);
    if(e == NULL) {
        printf("return: %d \n", ret);
    }
    else {
        printf("return: %d \tvalue: %s\n", ret, (char*)cache->value(e));
    }

    std::string value_str = "test";
    ret = cache->put(key, value_str);
    cache->Release(e);
    ret = cache->get(key, value_str);
    cache->Release(e);
    cout << "return: " << ret << "\tvalue: "<< value_str << endl;


    //sptr
    char* tmp = new char[32];
    sprintf(tmp, "%s", "v1");
    value = tmp;
    string key1("t1");
    shared_ptr<const Cache::Handle> sptr;
    ret = cache->put(key, value, 6, &Deleter, sptr);
    sptr.reset();

    ret = cache->get(key, sptr);
    if(sptr == NULL) {
          printf("return: %d \n", ret);
    }
    else {
          Cache::Handle* ptr = const_cast<Cache::Handle*> (sptr.get());
          printf("return: %d \tvalue: %s\n", ret, (char*)cache->value(ptr));
    }
    sptr.reset();

    delete cache;

    return 0;
}

