
#ifndef LRUCACHE_INCLUDE_LOCK_H_
#define LRUCACHE_INCLUDE_LOCK_H_

#include <pthread.h>
#include <assert.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>

#include "LRU_Global.h"

#define LOCK_BUSY		1

class LRU_Lock
{
public:
    LRU_Lock();
	virtual ~LRU_Lock();

	virtual void* getLock() { return m_pLock; }
	virtual bool destroyCond(){return !pthread_cond_destroy(&m_stCond);}

	/*virtual void deleteLock()
	{
		if(m_pLock) {
			delete m_pLock;
			m_pLock = NULL;
		}
	}

	virtual void deleteAttr()
	{
		if(m_pAttr) {
			delete m_pAttr;
			m_pAttr = NULL;
		}
	}*/

protected:
	void* m_pLock;
	void* m_pAttr;
	pthread_cond_t m_stCond;
private:
};

class LRU_MtLock : public LRU_Lock
{
public:
    LRU_MtLock(void* pLock = NULL);
	virtual ~LRU_MtLock();

	bool initLock();
	bool initAttr();
	bool destroyAttr();
	bool setProcessShare();
	bool destroyLock();

	bool mtLock();
	int mtTryLock();
	bool mtUnlock();
	void mtWait();
	void mtNotify();
protected:
private:

};

class LRU_RWLock : public LRU_Lock
{
public:
    LRU_RWLock(void* pLock = NULL);
	virtual ~LRU_RWLock();

	bool initLock();
	bool initAttr();
	bool destroyAttr();
	bool destroyLock();

	bool readLock();
	bool writeLock();
	bool rwUnLock();
protected:
private:
};

class LRU_Mutex
{
public:
    LRU_Mutex() { pthread_mutex_init(&mu_, NULL); }
    ~LRU_Mutex() { pthread_mutex_destroy(&mu_); }

    void Lock() { pthread_mutex_lock(&mu_); }
    void Unlock() { pthread_mutex_unlock(&mu_); }

private:
    pthread_mutex_t mu_;

    LRU_Mutex(const LRU_Mutex&);
    void operator=(const LRU_Mutex&);
};

class LRU_MutexLock
{
public:
    LRU_MutexLock(LRU_Mutex *mu) : mu_(mu) {
        this->mu_->Lock();
    }
    ~LRU_MutexLock() {this->mu_->Unlock(); }

private:
    LRU_Mutex * const mu_;

    LRU_MutexLock(const LRU_MutexLock&);
    void operator=(const LRU_MutexLock&);
};

#endif /* _LRUCACHE_INCLUDE_LOCK_H_ */
