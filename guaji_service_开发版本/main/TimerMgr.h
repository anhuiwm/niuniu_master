#pragma once
#include "base.h"
#include "log.h"

// 定时触发的定时器结构
struct StaticTimer
{
	DWORD					m_timer;	// 每天的啥时候触发
	bool					m_bPass;	// 是否过了此时间
    std::function<void ()>	m_func;		// 触发函数
};

#define MINUTE	60
#define HOUR	(60*60)
#define	DAY		(24*HOUR)
#define TEN_SECOND 10
#define TWENTY_SECOND (2 * TEN_SECOND)

// 整点触发器实现
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
        time_t        m_end_time; // 截止时间

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
    // 添加一次性定时器 tTrigger触发时间 func回调函数对象
    DWORD AddOnceTimer(time_t tTrigger,TimerFunction func);

    DWORD AddOnceTimerFromNow(time_t tTrigger, TimerFunction func);

    // 添加循环定时器, tStart开始时间, dwInterval间隔时间, func回调函数对象
    DWORD AddCircleTimer(time_t tStart,TimerFunction func, DWORD dwInterval);

	DWORD AddCircleTimer(time_t tStart, time_t tEnd, TimerFunction func, DWORD dwInterval);	

	// 每天定点定时器
	DWORD AddinDayStaticTimer(TimerFunction triggerFunc, DWORD tTrigger);

};



//定时器
class timer_member
{
private:
	int tm_flag;
	DWORD team;
	struct event tm_ev;
public:
	timer_member(unsigned int millisecond, DWORD team, int tmflag = 1);
	void del_tm_ev();
private:
	timer_member(const timer_member& tm);
	timer_member& operator=(const timer_member& tm);
	static void timeout_cb(int fd, short event_cmd, void *arg);
};
//定时器管理器
class timer_mgr
{
public:
	timer_mgr(){}
	~timer_mgr(){}

	static timer_mgr& getInstance()
	{
		static timer_mgr s_instance;
		return s_instance;
	}
public:
	void add_timer(unsigned int millisecond, DWORD team, int tmflag = 0);
	void del_timer(DWORD team);

private:
	timer_mgr(const timer_mgr& tm);
	timer_mgr& operator=(const timer_mgr& tm);
	map<DWORD, timer_member*> map_timer_members;
};


