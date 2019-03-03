
#ifndef _LRUCACHE_INCLUDE_LRUNODE_H_
#define _LRUCACHE_INCLUDE_LRUNODE_H_

#include "LRU_Global.h"
#include "slice.h"

#include <stdint.h>
#include <atomic>

struct LruNode {
    void* ptr;                  // use ptr
    void* value;
    void (*deleter)(const Slice&, void* value);
    LruNode* next;
    LruNode* front;
    size_t charge;              // length of value
    size_t key_length;          // length of key
    bool in_cache;              // Whether entry is in the cache.
    bool in_use;                // Whether entry is in the use list
    std::atomic<uint32_t> refs; // References, including cache reference, if present.
    uint64_t hash;              // Hash of key(); used for fast sharding and comparisons
    long stamp;                 // last use time
    char key_data[1];           // Beginning of key

    Slice key() const {
        // next_ is only equal to this if the LRU handle is the list head of an
        // empty list. List heads never have meaningful keys.
        assert(next != this);

        return Slice(key_data, key_length);
    }
};


#endif /* _LRUCACHE_INCLUDE_LRUNODE_H_ */
