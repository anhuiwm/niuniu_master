#ifndef TIMER_H 
#define TIMER_H

typedef long long TIMERID_t;

//typedef void (*TIMER_CALL_BACK)(void * para);
typedef void (*TIMER_CALL_BACK)(const std::string *strPara);


typedef void (*PostTimerMessage_t)(int i, int nIntv, void* cb, const std::string& strPara);


typedef struct _TRIGGER {
	
	time_t			tTrigger;
	TIMER_CALL_BACK	cb;

//	void			*para;
	std::string 			strPara;
	
	int				nInterval;
	bool			bRestart;


	TIMERID_t		id;//allocate when add to timer list

	PostTimerMessage_t funPostMessage;

	int nWorkThread;


	//_TRIGGER(int id, time_t tTrigger, TIMER_CALL_BACK cb, const string &strPara, int nInterval, bool bRestart = true)
	_TRIGGER(time_t tTrigger, TIMER_CALL_BACK cb, std::string *strPara, int nInterval, bool bRestart = true, PostTimerMessage_t funPostMessage = NULL, int nWorkThread = 0 )
	{
		this->id		= (TIMERID_t)0;
		this->tTrigger	= tTrigger;
		this->cb		= cb;

		if(strPara)
			this->strPara.assign(*strPara);
		
		this->nInterval	= nInterval;
		this->bRestart	= bRestart;
		
		this->funPostMessage	= funPostMessage;
		this->nWorkThread		= nWorkThread;
		
	}
	
	_TRIGGER()
	{
		//free(para);
	}

}TRIGGER;

#define NEW_TIMER 0

#if NEW_TIMER


typedef map<time_t, TRIGGER> MAP_TRIGGER;
typedef map<time_t, TRIGGER>::iterator MAP_TRIGGER_IT;

#else

typedef std::multimap<time_t, TRIGGER> MAP_TRIGGER;
typedef std::multimap<time_t, TRIGGER>::iterator MAP_TRIGGER_IT;

#endif

class Timer 
{

public:
    static Timer * GetInstance();
    static void Free();
	void Stop();
	void Resume2(void);
	void Del(TIMERID_t id);
	TIMERID_t Add( const TRIGGER & trigger );
//	TIMERID_t 
	void List_unsafe( void );

private:
	
    static Timer * _obj;
	static void* TimerThd(void* arg);
	
	pthread_mutex_t mutex;
	sem_t m_sem;
	sem_t m_sem_resume;
	
	bool bThdExit, bThdStop;
	MAP_TRIGGER mapTrigger;

	std::vector<TRIGGER> m_vecAdd;
	std::vector<TIMERID_t> m_vecDel;

	
	Timer();
    ~Timer();
	
    void Init();	
};

#define TADD( x )		Timer::GetInstance()->Add( x )
#define TDEL( x )		(Timer::GetInstance()->Del( x ) , x=0)


#endif

