
#include "LruCache.h"

#include <time.h>
#include <assert.h>

LruCache::LruCache(const bool hotFlag, const long aliveTime,
        const long cleanTime, const int bucketSize)
 : m_hotFlag(hotFlag), m_aliveTime(aliveTime), m_cleanTime(cleanTime), m_bucketSize(bucketSize)
{
    m_time = time(0);
    m_usedHead = NULL;
    m_usedTail = NULL;
    m_cleanHead = NULL;
    m_cleanTail = NULL;
}

LruCache::~LruCache() {
    std::unordered_map<uint64_t, LruNode*>::iterator mit = m_nodes.begin();

    while(mit != m_nodes.end()){
        LruNode* e = mit->second;
        if(e) {
            assert(e->in_cache);
            e->in_cache = false;
            assert(e->refs == 1);
            UnRef(e);
        }
        ++mit;
    }

    mit = m_cleannodes.begin();
    while(mit != m_cleannodes.end()){
        LruNode* e = mit->second;
        if(e) {
            assert(e->in_cache);
            e->in_cache = false;
            assert(e->refs == 1);
            UnRef(e);
        }
        ++mit;
    }

}

LruNode* LruCache::put(const uint64_t hash, const Slice& key, void* value, size_t charge, void (*deleter)(const Slice& key, void* value), void* ptr)
{
    LRU_MutexLock l(&m_mutex);
    
    long curTime = time(NULL);
    if(m_cleanTime > 0 && (curTime - m_time) > m_cleanTime) {
        cleanMap();
        m_time = curTime;
    }

    LruNode* e = reinterpret_cast<LruNode*>(
                      malloc(sizeof(LruNode)-1 + key.size()));
    e->value = value;
    e->deleter = deleter;
    e->charge = charge;
    e->key_length = key.size();
    e->hash = hash;
    e->in_cache = true;
    e->in_use = true;
    e->refs = 2;  // for the returned handle.
    e->next = NULL;
    e->front = NULL;
    e->ptr = ptr;
    e->stamp = curTime;
    memcpy(e->key_data, key.data(), key.size());
    
    std::unordered_map<uint64_t, LruNode*>::iterator mit;
    mit = m_nodes.find(hash);
    if(mit != m_nodes.end()){
        LruNode* node = mit->second;
        LRU_Remove(node);
        if(node->refs == 1){
            node->in_cache = false;
            UnRef(node);
        }
        else {
            LRU_Append(NULL, node);
        }
    }
    
    if(mit == m_nodes.end() && (m_bucketSize > 0 &&
            ((int)m_nodes.size() > m_bucketSize))) {
        LruNode* temp = m_usedTail;
        LRU_Remove(temp);
        if(temp->refs == 1){
            temp->in_cache = false;
            UnRef(temp);
        }
        else {
            LRU_Append(NULL, temp);
        }
    }

    LRU_Append(NULL, e);

    return e;
}

LruNode* LruCache::get(const uint64_t hash, const Slice& inKey)
{
    LRU_MutexLock l(&m_mutex);
    
    long curTime = time(0);
    if(m_cleanTime > 0 && ((curTime - m_time) > m_cleanTime)) {
        cleanMap();
        m_time = curTime;
    }

    std::unordered_map<uint64_t, LruNode*>::iterator mit;
    LruNode* e = NULL;
    mit = m_nodes.find(hash);
    if(mit == m_nodes.end()) {
        return e;
    }

    e = mit->second;

    if(m_aliveTime > 0) {//timeout
        long dataTime = e->stamp;
        if(curTime - dataTime >= m_aliveTime) {
            LRU_Remove(e);
            if(e->refs == 1) {
                e->in_cache = false;
                UnRef(e);
            }
            else {
                LRU_Append(NULL, e);
            }
            
            return NULL;
        }
    }

    if(inKey != e->key()) {
        return NULL;
    }

    if(m_hotFlag) {
        e->stamp = curTime;
        LRU_Append(NULL, e);
    }

    if(e){
        e->refs++;
    }

    return e;
}

void LruCache::Ref(LruNode* e) {
    if(e == NULL) {
        return ;
    }

    if(e->refs == 1 && e->in_cache) {  // If on lru_ list, move to in_use_ list.
        if(!e->in_use) {
            LRU_Remove(e);
            e->in_use = true;
            //LRU_Append(&in_use_, e);
            LRU_Append(NULL, e);
        }
    }
    e->refs++;
}

void LruCache::UnRef(LruNode* e) {
    if(e == NULL) {
        return ;
    }

    assert(e->refs > 0);
    e->refs--;
    if (e->refs == 0) {  // Deallocate.
        assert(!e->in_cache);
        (*e->deleter)(e->key(), e->value);
        free(e);
    } else if (e->in_cache && e->refs == 1) {
        // No longer in use; move to lru_ list.
        //LRU_Remove(e);
        ////LRU_Append(&lru_, e);
        //LRU_Append(NULL, e);
    }
}

void LruCache::LRU_Remove(LruNode* e) {
    //e->next->prev = e->prev;
    //e->prev->next = e->next;
    if(e == NULL) {
        return ;
    }

    if(e->in_use) {
        eraseFromUsedList(e);
        m_nodes.erase(e->hash);
        e->in_use = false;
    }
    else {
        eraseFromCleanList(e);
        m_cleannodes.erase(e->hash);
    }
}

void LruCache::LRU_Append(LruNode* list, LruNode* e) {
    // Make "e" newest entry by inserting just before *list
    //e->next = list;
    //e->prev = list->prev;
    //e->prev->next = e;
    //e->next->prev = e;
    if(e == NULL)
        return ;

    if(e->in_use) {
        m_nodes[e->hash] = e;
        insertToUsedList(e);
    }
    else {
        m_cleannodes[e->hash] = e;
        insertToCleanList(e);
    }
}

#if 0
int LruCache::Release(LruNode* e)
{
    LRU_MutexLock l(&m_mutex);
    
    if(e == NULL)
        return STATUS_FAILED;

/*
    long curTime = time(0);
    if(m_cleanTime > 0 && ((curTime - m_time) > m_cleanTime)) {
        cleanMap();
        m_time = curTime;
    }
*/

    if(e->refs == 1) {
        LRU_Remove(e);
        e->in_cache = false;
    }

    UnRef(e);

    return STATUS_SUCCEED;
}
#endif

int LruCache::Release(LruNode* e)
{
    if(e == NULL)
        return STATUS_FAILED;

    if(e->refs >= 2) {
        e->refs--;
    }

    return STATUS_SUCCEED;
}

bool LruCache::remove(LruNode* e)
{
    if(e != NULL) {
        assert(e->in_cache);
        LRU_Remove(e);
        if(e->refs == 1)
            e->in_cache = false;

        UnRef(e);
    }

    return e != NULL;
}

int LruCache::remove(const uint64_t hash, const Slice& key)
{
    LRU_MutexLock l(&m_mutex);
    
    long curTime = time(0);
    if(m_cleanTime > 0 && ((curTime - m_time) > m_cleanTime)) {
        cleanMap();
        m_time = curTime;
    }

    std::unordered_map<uint64_t, LruNode*>::iterator mit;
    LruNode* e = NULL;
    mit = m_nodes.find(hash);
    if(mit != m_nodes.end()){
        e = mit->second;
    }
    else {
        mit = m_cleannodes.find(hash);
        if(mit != m_nodes.end()){
            e = mit->second;
        }
    }

    remove(e);

    if(e == NULL)
        return BUCKET_NOKEY;

    return STATUS_SUCCEED;
}

int LruCache::getMapSize()
{
    return m_nodes.size();
}

int LruCache::cleanMap()
{
    cleanCleanMap();

    if(m_nodes.size() == 0) {
        return STATUS_SUCCEED;
    }

    long curTime = time(0);
    LruNode* temp = NULL;

    while(m_usedTail && curTime - m_usedTail->stamp >= m_aliveTime){
#ifdef _LRU_CACHE_DEBUG
        LRU_DEBUG("used list, key:%s, ref:%d\n", m_usedTail->key_data, m_usedTail->refs);
#endif
        temp = m_usedTail;
        LRU_Remove(temp);
        if(temp->refs == 1) {
            temp->in_cache = false;
            UnRef(temp);
        }
        else {
            LRU_Append(NULL, temp);
        }
    }

    return STATUS_SUCCEED;
}

int LruCache::cleanCleanMap()
{
    if(m_cleannodes.size() == 0) {
        return STATUS_SUCCEED;
    }

    LruNode* temp = NULL;

    while(m_cleanTail) {
#ifdef _LRU_CACHE_DEBUG
        LRU_DEBUG("clean list, key:%s, ref:%d\n", m_cleanTail->key_data, m_cleanTail->refs);
#endif
        if(m_cleanTail->refs != 1)
            break;

        temp = m_cleanTail;
        LRU_Remove(temp);
        temp->in_cache = false;
        UnRef(temp);
    }

    return STATUS_SUCCEED;
}

int LruCache::insertToUsedList(LruNode* node)
{
    LruNode* front = node->front;
    LruNode* next = node->next;

    if(node == m_usedHead) {//head
        return STATUS_SUCCEED;
    }

    if(m_usedHead == NULL) {
        m_usedHead = m_usedTail = node;
        return STATUS_SUCCEED;
    }

    if(next) {//front true too
        front->next = next;
        next->front = front;
    } else if(front){//(front true  next NULL) tail node
        m_usedTail = front;
        m_usedTail->next = NULL;
    }

    m_usedHead->front = node;
    node->next = m_usedHead;
    node->front = NULL;
    if(m_usedHead == m_usedTail){
        m_usedTail->front = node;
    }
    m_usedHead = node;

    return STATUS_SUCCEED;
}

int LruCache::eraseFromUsedList(LruNode* node)
{
    LruNode* front = NULL;
    LruNode* next = NULL;

    if(node == m_usedHead) {
        front = m_usedHead->next;
        if(front) {
            front->front = NULL;
            m_usedHead = front;
        } else {
            m_usedHead = m_usedTail = front;
        }

        return STATUS_SUCCEED;
    }

    if(node == m_usedTail) {
        next = node->front;
        next->next = NULL;
        m_usedTail = next;

        return STATUS_SUCCEED;
    }

    front = node->front;
    next = node->next;
    front->next = next;
    next->front = front;

    return STATUS_SUCCEED;
}

int LruCache::insertToCleanList(LruNode* node)
{
    LruNode* front = node->front;
    LruNode* next = node->next;

    if(node == m_cleanHead) {//head
        return STATUS_SUCCEED;
    }

    if(m_cleanHead == NULL) {
        m_cleanHead = m_cleanTail = node;
        return STATUS_SUCCEED;
    }

    if(next) {//front true too
        front->next = next;
        next->front = front;
    } else if(front){//(front true  next NULL) tail node
        m_cleanTail = front;
        m_cleanTail->next = NULL;
    }

    m_cleanHead->front = node;
    node->next = m_cleanHead;
    node->front = NULL;
    if(m_cleanHead == m_cleanTail){
        m_cleanTail->front = node;
    }
    m_cleanHead = node;

    return STATUS_SUCCEED;
}

int LruCache::eraseFromCleanList(LruNode* node)
{
    LruNode* front = NULL;
    LruNode* next = NULL;

    if(node == m_cleanHead) {
        front = m_cleanHead->next;
        if(front) {
            front->front = NULL;
            m_cleanHead = front;
        } else {
            m_cleanHead = m_cleanTail = front;
        }

        return STATUS_SUCCEED;
    }

    if(node == m_cleanTail) {
        next = node->front;
        next->next = NULL;
        m_cleanTail = next;

        return STATUS_SUCCEED;
    }

    front = node->front;
    next = node->next;
    front->next = next;
    next->front = front;

    return STATUS_SUCCEED;
}
