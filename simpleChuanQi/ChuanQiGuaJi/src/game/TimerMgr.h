#pragma once

#include <time.h>
#include <utility>
#include <map>
#include <functional>
#include <unordered_map>
#include "base.h"
#include "log.h"

// ��ʱ�����Ķ�ʱ���ṹ
struct StaticTimer
{
	DWORD					m_timer;	// ÿ���ɶʱ�򴥷�
	bool					m_bPass;	// �Ƿ���˴�ʱ��
    std::function<void ()>	m_func;		// ��������
};

#define MINUTE	60
#define HOUR	(60*60)
#define	DAY		(24*HOUR)
#define TEN_SECOND 10
#define TWENTY_SECOND (2 * TEN_SECOND)

// ���㴥����ʵ��
class TimerMgr
{
public:
	virtual ~TimerMgr(){};

	TimerMgr();

	static TimerMgr& getMe()
	{
		static TimerMgr s_me;
		return s_me;
	}

public:
    void Update();
    void CancelTimer(DWORD iTimer);

protected:
    DWORD		m_dwCurSeq;
	time_t  m_tCurTime;

protected:
    typedef std::function<void ()> TimerFunction;

    struct S_TimerItem
    {
        DWORD         m_dwSeq;
        time_t	       m_timer;	
        TimerFunction  m_func;
        bool           m_bCircle;
        DWORD         m_dwInterval;
        time_t        m_end_time; // ��ֹʱ��

        S_TimerItem()
        {
            m_timer = 0;
            m_bCircle = false;
            m_dwInterval = 0;
            m_dwSeq = 0;
           	m_end_time = 0;
        }
    };

    typedef std::map<int, S_TimerItem> TimerItemMap;

    TimerItemMap m_mapTimerItems;
	
	time_t default_end_time();
public:
    // ���һ���Զ�ʱ�� tTrigger����ʱ�� func�ص���������
    DWORD AddOnceTimer(time_t tTrigger,TimerFunction func);

    DWORD AddOnceTimerFromNow(time_t tTrigger, TimerFunction func);

    // ���ѭ����ʱ��, tStart��ʼʱ��, dwInterval���ʱ��, func�ص���������
    DWORD AddCircleTimer(time_t tStart,TimerFunction func, DWORD dwInterval);

	DWORD AddCircleTimer(time_t tStart, time_t tEnd, TimerFunction func, DWORD dwInterval);	

	// ÿ�춨�㶨ʱ��
	DWORD AddinDayStaticTimer(TimerFunction triggerFunc, DWORD tTrigger);

	// ÿСʱ���㶨ʱ��
	//DWORD AddHourStaticTimer(TimerFunction triggerFunc, DWORD tTrigger);
};
