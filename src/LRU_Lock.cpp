
#include "LRU_Lock.h"

#include <stdexcept>
#include <errno.h>

LRU_Lock::LRU_Lock() {
	// TODO Auto-generated constructor stub

	if( 0 != pthread_cond_init(&m_stCond, 0)) {
		throw std::runtime_error("pthread_cond_init error.");
	}

}

LRU_Lock::~LRU_Lock() {
	// TODO Auto-generated destructor stub
	destroyCond();
}

LRU_MtLock::LRU_MtLock(void* pLock)
{
	if(pLock) {
		m_pLock = pLock;
	} else {
		m_pAttr = new pthread_mutexattr_t;
		if(!initAttr()) {
			throw std::runtime_error("initialization mutex lock attribute error.");
		}

		m_pLock = new pthread_mutex_t;
		if(!initLock()) {
			throw std::runtime_error("initialization mutex lock error.");
		}
	}
}

LRU_MtLock::~LRU_MtLock()
{
	destroyAttr();
	destroyLock();

	if(m_pLock) {
		delete (pthread_mutex_t*)m_pLock;
		m_pLock = NULL;
	}

	if(m_pAttr) {
		delete (pthread_mutexattr_t*)m_pAttr;
		m_pAttr = NULL;
	}
}

bool LRU_MtLock::initLock()
{
	return !pthread_mutex_init((pthread_mutex_t*)m_pLock, (pthread_mutexattr_t*)m_pAttr);
}

bool LRU_MtLock::mtLock()
{
	if(!m_pLock) {
		return false;
	}

	return !pthread_mutex_lock((pthread_mutex_t*)m_pLock);
}

bool LRU_MtLock::mtUnlock()
{
	if(!m_pLock) {
		return false;
	}

	return !pthread_mutex_unlock((pthread_mutex_t*)m_pLock);
}

void LRU_MtLock::mtWait()
{
	if(!m_pLock) {
		return;
	}

	pthread_cond_wait(&m_stCond, (pthread_mutex_t*)LRU_Lock::m_pLock);
}

void LRU_MtLock::mtNotify()
{
	pthread_cond_signal(&m_stCond);
}

bool LRU_MtLock::initAttr()
{
	if(!m_pAttr) {
		return false;
	}
	return !pthread_mutexattr_init((pthread_mutexattr_t*)m_pAttr);
}


bool LRU_MtLock::destroyAttr()
{
	if(!m_pAttr) {
		return false;
	}
	return !pthread_mutexattr_destroy((pthread_mutexattr_t*)m_pAttr);
}

bool LRU_MtLock::setProcessShare()
{
	if(!m_pAttr) {
		return false;
	}
	return !pthread_mutexattr_setpshared((pthread_mutexattr_t*)m_pAttr, PTHREAD_PROCESS_SHARED);
}

bool LRU_MtLock::destroyLock()
{
	if(!m_pLock) {
		return false;
	}
	return !pthread_mutex_destroy((pthread_mutex_t*)m_pLock);
}

/*
 * 1 busy 0 success   other error.
 */
int LRU_MtLock::mtTryLock()
{
	if(!m_pLock) {
		return STATUS_FAILED;
	}

	//0success  1busy
	int ret = pthread_mutex_trylock((pthread_mutex_t*)m_pLock);

	return ret;
}

LRU_RWLock::LRU_RWLock(void* pLock)
{
	if(pLock) {
		m_pLock = pLock;
	} else {
		m_pAttr = new pthread_rwlockattr_t;
		if(!initAttr()) {
			throw std::runtime_error("initialization rwlock attribute error.");
		}

		m_pLock = (pthread_rwlock_t*)new pthread_rwlock_t;
		if(!initLock()) {
			throw std::runtime_error("initialization rwlock error.");
		}
	}
}

LRU_RWLock::~LRU_RWLock()
{
	destroyAttr();
	destroyLock();

	if(m_pLock) {
		delete (pthread_rwlock_t*)m_pLock;
		m_pLock = NULL;
	}

	if(m_pAttr) {
		delete (pthread_rwlockattr_t*)m_pAttr;
		m_pAttr = NULL;
	}
}

bool LRU_RWLock::initLock()
{
	return !pthread_rwlock_init((pthread_rwlock_t*)m_pLock, (pthread_rwlockattr_t*)m_pAttr);
}

bool LRU_RWLock::initAttr()
{
	if(!m_pAttr) {
		return false;
	}
	return !pthread_rwlockattr_init((pthread_rwlockattr_t*)m_pAttr);
}

bool LRU_RWLock::destroyAttr()
{
	if(!m_pAttr) {
		return false;
	}
	return !pthread_rwlockattr_destroy((pthread_rwlockattr_t*)m_pAttr);
}

bool LRU_RWLock::destroyLock()
{
	if(!m_pLock) {
		return false;
	}
	return !pthread_rwlock_destroy((pthread_rwlock_t*)m_pLock);
}

bool LRU_RWLock::readLock()
{
	if (NULL == m_pLock) {
		return false;
	}
	return !pthread_rwlock_rdlock((pthread_rwlock_t*)LRU_Lock::m_pLock);
}

bool LRU_RWLock::writeLock()
{
	if (NULL == m_pLock) {
		return false;
	}
	return !pthread_rwlock_wrlock((pthread_rwlock_t*)LRU_Lock::m_pLock);
}

bool LRU_RWLock::rwUnLock()
{
	if (NULL == m_pLock) {
		return false;
	}
	return !pthread_rwlock_unlock((pthread_rwlock_t*)LRU_Lock::m_pLock);
}
