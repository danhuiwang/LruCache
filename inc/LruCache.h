
#ifndef _LRUCACHE_INCLUDE_LRUCACHE_H_
#define _LRUCACHE_INCLUDE_LRUCACHE_H_

#include "LruNode.h"
#include "LRU_Global.h"
#include "LRU_Lock.h"

#include <unordered_map>
#include <list>

#define BUCKET_CONFLICT          -10
#define BUCKET_NOKEY             -11
#define BUCKET_TIMEOUT           -12

class LruCache {
public:
    LruCache(const bool hotFlag, const long aliveTime,
            const long cleanTime, const int bucketSize);
    virtual ~LruCache();

    LruNode* put(const uint64_t hash, const Slice& key, void* value, size_t charge, void (*deleter)(const Slice& key, void* value), void* ptr = NULL);
    LruNode* get(const uint64_t hash, const Slice& inKey);
    int remove(const uint64_t hash, const Slice& key);
    bool remove(LruNode* e);

    void Ref(LruNode* e);
    void UnRef(LruNode* e);
    void LRU_Remove(LruNode* e);
    void LRU_Append(LruNode* list, LruNode* e);

    int Release(LruNode* e);

    int getMapSize();
protected:
    int cleanMap();
    int cleanCleanMap();
    
    int insertToUsedList(LruNode* node);
    int eraseFromUsedList(LruNode* node);
    int insertToCleanList(LruNode* node);
    int eraseFromCleanList(LruNode* node);
private:
    bool m_hotFlag;
    long m_aliveTime;
    long m_cleanTime;
    int m_bucketSize;

    std::unordered_map<uint64_t, LruNode*> m_nodes;
    LruNode* m_usedHead;
    LruNode* m_usedTail;

    std::unordered_map<uint64_t, LruNode*> m_cleannodes;
    LruNode* m_cleanHead;// out of alive node,but refs is not 1
    LruNode* m_cleanTail;// out of alive node,but refs is not 1

    long m_time;
    LRU_Mutex m_mutex;
};

#endif /* _LRUCACHE_INCLUDE_LRUCACHE_H_ */
