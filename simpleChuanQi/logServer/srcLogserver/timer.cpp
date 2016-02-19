#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/param.h>
#include <sys/epoll.h>

#include <string.h>
#include <zlib.h>

#include <list>
#include <string>
#include <vector>
#include <deque>
#include <map>
#include <iostream>
#include <algorithm>

#include <setjmp.h>
#include <semaphore.h>

#include "timer.h"
#include "j2.h"

#define TIME_2038_01_01	2145888000

#define TT(t) (  GetLogTime(t).c_str()  )

Timer* Timer::_obj = NULL;

Timer* Timer::GetInstance()
{
	return _obj == NULL ? (_obj = new Timer()) : _obj;
}

void Timer::Free()
{
	if (_obj != NULL) {
		delete _obj;
		_obj = NULL;
	}
}

Timer::Timer()
{
	Init();
}

Timer::~Timer()
{
	bThdExit = true;
	Sem_post(&m_sem);
	Sem_post(&m_sem_resume);

		
	//sem_destroy(&m_sem);	
	//pthread_mutex_destroy( &mutex );
}

void Timer::Init()
{	
	bThdExit = bThdStop = false;
	
	Sem_init(&m_sem,0,0);
	Sem_init(&m_sem_resume,0,0);
	
	pthread_mutex_init( &mutex, NULL );
	
	//mapTrigger.insert( MAP_TRIGGER::value_type( TIME_2038_01_01, TRIGGER( TIME_2038_01_01 , NULL, NULL, TIME_2038_01_01, true)) );
	Add( TRIGGER(TIME_2038_01_01, NULL, NULL, TIME_2038_01_01, true) );

	Pthread_create_detached( TimerThd, this );
}

void Timer::Resume2()
{
	pthread_mutex_lock( &mutex );
	this->bThdStop = false;
	pthread_mutex_unlock( &mutex );
	Sem_post(&m_sem);

}


void Timer::Stop()
{
	pthread_mutex_lock( &mutex );
	this->bThdStop = true;	
	pthread_mutex_unlock( &mutex );

	Sem_post(&m_sem);
	Sem_wait(&m_sem_resume);
}

void Timer::Del(TIMERID_t id)
{
	pthread_mutex_lock( &mutex );
	m_vecDel.push_back(id);
	pthread_mutex_unlock( &mutex );

	Sem_post(&m_sem);
}

TIMERID_t Timer::Add( const TRIGGER & trigger)
{
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	TIMERID_t id = ( (__int64)ts.tv_sec << 32 )  | ts.tv_nsec;

	
	TRIGGER x = trigger;
	x.id = id;

	pthread_mutex_lock( &mutex );
	m_vecAdd.push_back(x);
	pthread_mutex_unlock( &mutex );
	
	Sem_post(&m_sem);
	return	id;
}


//TIMERID_t 
void Timer::List_unsafe( void )
{
	for( MAP_TRIGGER_IT it = this->mapTrigger.begin(); it!=this->mapTrigger.end(); ++it )  {
		logv("key:%d, id:%lld, trigtime:%s, nInterval:%d, bRestart:%d", 
		it->first, it->second.id, TT(it->second.tTrigger), it->second.nInterval, it->second.bRestart);
		if( it->second.strPara.size() )
			logv("Type:%u, MulFactor:%u", *(BYTE*) it->second.strPara.data(),  *(BYTE*)(it->second.strPara.data()+ 1 ) ); 
	}

	logv("m_vecAdd.size:%d, vecDel.size:%d, mapTrigger.size:%d", m_vecAdd.size(), m_vecDel.size(), mapTrigger.size());
	logv("===============");
}

void* Timer::TimerThd(void* arg)
{
	Timer* timer = (Timer*)arg;
	
	//logv("TimerThd pthread_self:%d %d", (int)pthread_self(), getpid() );

	while(1) {
		
		MAP_TRIGGER_IT it = timer->mapTrigger.begin();
		struct timespec ts = { it->second.tTrigger, 0 };

		TRIGGER &trig = it->second;

		debugv("%s", GetLogTime(it->second.tTrigger).c_str() );
		
		int nWait;
		while ((nWait = sem_timedwait( &timer->m_sem, &ts ) ) == -1 && errno == EINTR)
			continue;
		
		if(timer->bThdExit)
			break;
		else if(timer->bThdStop) {
			
			logv("stop1");
			
			pthread_mutex_lock( &timer->mutex );
			Sem_post(&timer->m_sem_resume);//tell him , i have stopped
			pthread_mutex_unlock( &timer->mutex );

			Sem_wait( &timer->m_sem );
				
			logv("stop2");
		}
		else if (nWait == -1) {
			
			if (errno == ETIMEDOUT) { // trig timer
				//debugv("sem_timedwait ETIMEDOUT! %d", errno);

				if ( trig.funPostMessage ) {
					
					trig.funPostMessage( trig.nWorkThread, trig.nInterval, (void*)trig.cb, trig.strPara );
				}
				else
					trig.cb( &trig.strPara );
				
				if(trig.bRestart) {
					
					//it->second.tTrigger += it->second.nInterval;
					trig.tTrigger = time(NULL) + trig.nInterval;
					timer->mapTrigger.insert( MAP_TRIGGER::value_type(trig.tTrigger, trig) );
					
					//TODO:
					{
						static int foo =0;
						if ( foo == 0 ) {
							foo = 1;
						//	system("date -s '2002-08-07 15:10:00 cst'");
						}
						//system("date");
						//logv("%d %d %lld", time(NULL), trig.nInterval, trig.id);
					}
					
				}
				timer->mapTrigger.erase(it);
			}
			else { // should not come here 
				debug_log("sem_timedwait error! %d", errno);
			}
		}
		else { //Add, Del trigger
			//logv("Add, Del trigger");
			//debugv("sem_timedwait() succeeded");			
			pthread_mutex_lock( &timer->mutex );
			
				size_t i = timer->m_vecAdd.size();
				while( i ) {
					timer->mapTrigger.insert( MAP_TRIGGER::value_type(timer->m_vecAdd[i-1].tTrigger, timer->m_vecAdd[i-1]) );
					timer->m_vecAdd.pop_back();
					i = timer->m_vecAdd.size();
				}

				i = timer->m_vecDel.size();
				while( i ) {
					for( MAP_TRIGGER_IT xit = timer->mapTrigger.begin(); xit!=timer->mapTrigger.end(); ++xit )
						if( xit->second.id == timer->m_vecDel[i-1] ) {
							timer->mapTrigger.erase(xit);
							break;
						}
					timer->m_vecDel.pop_back();
					i = timer->m_vecDel.size();
				}
			pthread_mutex_unlock( &timer->mutex );
		}
	}
	return NULL;
}

