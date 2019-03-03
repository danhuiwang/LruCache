
#ifndef _LRUCACHE_INCLUDE_TIME_H
#define _LRUCACHE_INCLUDE_TIME_H

#include "LRU_Global.h"

#include <sys/time.h>
#include <stdio.h>
#include <string.h>

class LRU_Time
{
public:
    LRU_Time();
	virtual ~LRU_Time();

	void timeOn();
	void timeOff();
	void displayTime();
	uint32_t getSeconds(){return m_uiSeconds;};
	uint32_t getMicSeconds(){return m_uiMicSeconds;};
	struct timeval getTimeval(){return m_stOn;};

private:
	struct timeval m_stOn;
	struct timeval m_stOff;

	uint32_t m_uiSeconds;
	uint32_t m_uiMicSeconds;
};

#endif /* _LRUCACHE_INCLUDE_TIME_H */
