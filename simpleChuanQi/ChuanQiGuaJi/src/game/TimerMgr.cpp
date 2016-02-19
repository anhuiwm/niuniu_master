#include "TimerMgr.h"
#include "protocal.h"
#include <list>


TimerMgr::TimerMgr() 
: m_dwCurSeq(1)
{
    m_tCurTime = PROTOCAL::getCurTime();
}

void TimerMgr::Update()
{
	m_tCurTime = PROTOCAL::getCurTime();

    std::list<int> lstDelTimers;
    std::list<int> lstTimers;

	for(auto itr=m_mapTimerItems.begin();itr!=m_mapTimerItems.end();itr++)
    {
        lstTimers.push_back(itr->first);
    }

    for(auto itr=lstTimers.begin(); itr!=lstTimers.end();itr++)
    {
    	const int nTimerId = *itr;
        TimerItemMap::iterator itrFind = m_mapTimerItems.find(nTimerId);
        if (itrFind != m_mapTimerItems.end())
        {
            S_TimerItem& sTimerItem = itrFind->second;

            //std::string strfunc = sTimerItem.m_strFunc;

            if( m_tCurTime >= sTimerItem.m_timer)
            {
                if (sTimerItem.m_bCircle)
                {
                    sTimerItem.m_timer += sTimerItem.m_dwInterval;
                    if(sTimerItem.m_timer > sTimerItem.m_end_time)
					{
						lstDelTimers.push_back(itrFind->first);
					}
                }
                else
                {
                    lstDelTimers.push_back(itrFind->first);
                }

                sTimerItem.m_func();
            }
        }
    }

    for(auto itr=lstDelTimers.begin();itr!=lstDelTimers.end();itr++)
    {
    	const int nTimerId = *itr;
        m_mapTimerItems.erase(nTimerId);
    }
}

void TimerMgr::CancelTimer(DWORD iTimer)
{
    m_mapTimerItems.erase(iTimer);
}

static DWORD getTodayNowSecond( time_t time)//ΩÒÃÏ√Î ˝
{
	struct tm tmNow;
	LOCAL_TIME(time, tmNow);
	DWORD nowSecond = tmNow.tm_hour * 60 * 60 + tmNow.tm_min * 60 + tmNow.tm_sec;
	return nowSecond;
}

static DWORD getTodayStartSecond(time_t now)
{
	return now - getTodayNowSecond(now);
}


DWORD TimerMgr::AddinDayStaticTimer(TimerFunction triggerFunc, DWORD tTrigger)
{
	m_tCurTime = PROTOCAL::getCurTime();

    time_t tStart = getTodayStartSecond(m_tCurTime) + tTrigger;

    if (tStart < m_tCurTime)
    {
        tStart += DAY;
    }

    return AddCircleTimer(tStart, triggerFunc, DAY);
}
/*
DWORD TimerMgr::AddHourStaticTimer(TimerFunction triggerFunc, DWORD tTrigger)
{
    time_t tStart = getTodayStartSecond(m_tCurTime) + tTrigger;

    if (tStart < m_tCurTime)
    {
        tStart += HOUR;
    }

    return AddCircleTimer(tStart, triggerFunc, HOUR);
}
*/

time_t TimerMgr::default_end_time()
{
	return m_tCurTime = PROTOCAL::getCurTime() + 15 * 365 * 24 * 3600; 
}

DWORD TimerMgr::AddOnceTimer(time_t tTrigger,TimerFunction func)
{
    m_tCurTime = PROTOCAL::getCurTime();

    if(m_tCurTime > tTrigger)
	{
		return 0;
	}
    
    S_TimerItem timer;
    timer.m_func = func;
    timer.m_timer = tTrigger;
    timer.m_bCircle = false;
    timer.m_dwInterval = 0;
    timer.m_dwSeq = m_dwCurSeq;
	timer.m_end_time = default_end_time();

    m_mapTimerItems[m_dwCurSeq] = timer;

    return m_dwCurSeq++;
}

DWORD TimerMgr::AddOnceTimerFromNow(time_t tTrigger, TimerFunction func)
{
    tTrigger += m_tCurTime;
    return AddOnceTimer(tTrigger, func);
}

DWORD TimerMgr::AddCircleTimer(time_t tStart,TimerFunction func, DWORD dwInterval)
{
	m_tCurTime = PROTOCAL::getCurTime();

	while(true)
	{
		if(tStart > m_tCurTime)
			break;

		tStart += dwInterval;
	}

    S_TimerItem timer;
    timer.m_timer = tStart;
    timer.m_bCircle = true;
    timer.m_dwInterval = dwInterval;
    timer.m_dwSeq = m_dwCurSeq;
    timer.m_end_time = default_end_time();
	timer.m_func = func;

    m_mapTimerItems[m_dwCurSeq] = timer;

    return m_dwCurSeq++;
}

DWORD TimerMgr::AddCircleTimer(time_t tStart, time_t tEnd, TimerFunction func, DWORD dwInterval)
{
	m_tCurTime = PROTOCAL::getCurTime();

	while(true)
	{   
		if(tStart > m_tCurTime)
			break;

		tStart += dwInterval;
	}   

	if(tStart > tEnd)
	{
		return 0;
	}

	S_TimerItem timer;
	timer.m_timer = tStart;
	timer.m_bCircle = true;
	timer.m_dwInterval = dwInterval;
	timer.m_dwSeq = m_dwCurSeq;
	timer.m_end_time = tEnd;
	timer.m_func = func;

	m_mapTimerItems[m_dwCurSeq] = timer;

	return m_dwCurSeq++;
}

