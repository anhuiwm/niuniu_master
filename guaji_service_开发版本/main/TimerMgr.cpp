#include "pch.h"
#include "TimerMgr.h"
#include "protocal.h"
#include "service.h"
#include "team.h"

TimerMgr::TimerMgr() 
: m_dwCurSeq(1)
{
    m_tCurTime = time(nullptr);
}

void TimerMgr::Update()
{
	m_tCurTime = time(nullptr);

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
	return ( tmNow.tm_hour * 60 * 60 + tmNow.tm_min * 60 + tmNow.tm_sec );
}

static DWORD getTodayStartSecond(time_t now)
{
	return now - getTodayNowSecond(now);
}


DWORD TimerMgr::AddinDayStaticTimer(TimerFunction triggerFunc, DWORD tTrigger)
{
	m_tCurTime = time(nullptr);

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
	return m_tCurTime = time(nullptr) + 15 * 365 * 24 * 3600; 
}

DWORD TimerMgr::AddOnceTimer(time_t tTrigger,TimerFunction func)
{
    m_tCurTime = time(nullptr);

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
	m_tCurTime = time(nullptr);

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
	m_tCurTime = time(nullptr);

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


timer_member::timer_member(unsigned int millisecond, DWORD team, int tmflag)
{
	this->team = team;
	this->tm_flag = tm_flag;
	struct timeval tm_tv;
	long lsec = (long)millisecond / 1000l;
	long lms = (long)millisecond % 1000l;
	evutil_timerclear(&tm_tv);
	tm_tv.tv_sec = lsec;
	tm_tv.tv_usec = lms * 1000l;

	evtimer_set(&this->tm_ev, timeout_cb, this);
	event_base_set(service::event_handler, &this->tm_ev);
	evtimer_add(&this->tm_ev, &tm_tv);
}

void timer_member::del_tm_ev()
{
	evtimer_del(&this->tm_ev);
}
void timer_member::timeout_cb(int fd, short event_cmd, void *arg)
{
	timer_member* ptimer = (timer_member*)arg;
	if (ptimer == nullptr)
	{
		return;
	}

	DWORD team = ptimer->team;

	SINGJIANGE()->startTeamFight(team);
	//CC_SAFE_DELETE(ptimer);
	//timer_mgr::map_timer_members.erase(team);
	timer_mgr::getInstance().del_timer(team);
}

void timer_mgr::add_timer(unsigned int millisecond, DWORD team, int tmflag)
{
	timer_member* ptimer = new timer_member(millisecond, team, tmflag);
	if (ptimer == nullptr)
	{
		return;
	}
	map_timer_members[team] = ptimer;
}
void timer_mgr::del_timer(DWORD team)
{
	if (map_timer_members.find(team) == map_timer_members.end())
	{
		return;
	}
	timer_member* ptimer = map_timer_members[team];
	if (ptimer)
	{
		ptimer->del_tm_ev();
	}
	CC_SAFE_DELETE(ptimer);
	map_timer_members.erase(team);
}

