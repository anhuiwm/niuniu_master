//#include <pthread.h>
#include "j2.h"

#include <string.h>
#include <limits.h>


#include <dirent.h>
#include <sys/dir.h>

#include <unistd.h>
#include <stdarg.h>


#include <sys/time.h>
#include <sys/resource.h>


#include <iostream>
#include <sstream>
#include <fstream>


#include <sys/types.h>
#include <sys/wait.h>

static const char *m_szProgramName;
static char m_szProgramdir[128];
static char m_szConfigFile[128];
static char m_szConfigFileExtra[128];

string& StringTrim(string& str, const char* trim_char)
{
	string::size_type pos = str.find_last_not_of(trim_char);

	if(pos != string::npos) {
		str.erase(pos + 1);
		pos = str.find_first_not_of(trim_char);
		if(pos != string::npos) str.erase(0, pos);
	}
	else str.erase(str.begin(), str.end());
	
	return str;
}


string& StringReplaceAll(string & str, const string & strsrc, const string &strdst) 
{
	string::size_type pos=0;
	string::size_type srclen=strsrc.size();
	string::size_type dstlen=strdst.size();
	while( (pos=str.find(strsrc, pos)) != string::npos){
		str.replace(pos, srclen, strdst);
		pos += dstlen;
	}
	
	return str;
}
string& StringTrimAll(string& str, const char* trim_char)
{
	string::size_type pos;
	while( (pos=str.find_first_of(trim_char)) != string::npos){
		str.erase(pos,1);
	}
	return str;
}

int Strtoken2Vec( const char* str, const char* split, vector<string> &vec, TRIM_FUNC trim_func, const char* trim_char)
{
	char *token, *saveptr;
	char * s = strdup(str);
	char * ss =s;

	int i=0;
	while(	token = strtok_r(s, split, &saveptr) ) {
		vec.push_back(token);

		if( trim_func && trim_char )
			trim_func( vec[ vec.size()-1 ], trim_char);
			
		s = NULL;
		i++;
	}

	free(ss);
	return i;
}

int EachLineToBigIP_Star_func(const char*from, const char*to, void* para)
{
	string str(from, to);
	STC_IP_CHECK x;
	list<STC_IP_CHECK> *lst = ( list<STC_IP_CHECK> *)para;
	
	if( str.empty() ) {

		return 0;
	}

	vector<string> vecString;
	if ( 4 != Strtoken2Vec(str.c_str(), ".", vecString, StringTrimAll, " \t") ) {
		return 0;
	}

	x.dwBigEndIP=0;

	for( int i=0;i<vecString.size();i++) {

		BYTE b = (BYTE) SSTRTOUL(vecString[i]);
		if ( vecString[i] == "*" ) {
			x.vecStar.push_back(i);
		}

		x.dwBigEndIP |= b << ( (i) *8);
	}

	lst->push_back( x );
	return 1;

}
DWORD CStr2BigendianIP(const char* ip)
{
	struct in_addr a ;
	inet_aton(ip, &a);
	return a.s_addr;
}

static bool ParseIniFile(const char*fileee, SECTION_t& section_map, bool bReadLines=false)
{
	ifstream infile(fileee,  std::ios::binary);	
	if(!infile.is_open()) 
		return false;
	
	char line[MAX_LINE_LEN];
	string current_sec;
	while( infile.getline(line, MAX_LINE_LEN, '\n') ) { //not include 0a , may be include 0d

		string str(line);
		StringTrim(str,"\t\r");		
		StringReplaceAll(str, "\xef\xbb\xbf", "");

		string sec;
		string::size_type pos1, pos2,pos;

//password=abcd#123
#if 0
		if(  ( pos = str.find_first_of('#') ) == 0) 
			str.erase(pos);
#else
		if(  ( pos = str.find_first_of('#') ) != string::npos ) 
			str.erase(pos);
#endif


        if( str.empty() ) continue;
		
#if 0
		if(  ( pos1 = str.find_first_of('[') ) != string::npos ) {
			
			debugv("pos1: %d", pos1);

			if(  ( pos2 = str.find_first_of(']') ) != 	string::npos ) {
				debugv("pos2: %d", pos2);
				if(pos2-pos1 >1)
					sec = str.substr(pos1+1,pos2-pos1-1);
			}

		}
#else
		if(  ( pos1 = str.find_first_of('[') ) != string::npos ) {	
			if(  ( pos2 = str.find_first_of(']') ) != 	string::npos ) {
				
				if(pos2-pos1 >1) {

					bool bNotSec = true;
					for ( int i=0; i<pos1; i++) {
						debugv("%c", str[i]);
						if ( str[i] != '\t' || str[i] != 0x20 ) {//multi byte ?
							bNotSec = false;
							break;
						}						
					}
					if (bNotSec)
						sec = str.substr(pos1+1,pos2-pos1-1);
				}
			}
		}
#endif

		if( !sec.empty() ) 
			current_sec = sec;
		else {
			if( !bReadLines && (pos = str.find_first_of("=")) != string::npos ) {
				string key = str.substr(0, pos);
				string val = str.substr(pos+1);
				StringTrim(key, " \t");
				StringTrim(val, " \t");
				section_map[current_sec][key] = val;
			}
			else if( bReadLines )
				section_map[current_sec][INI_SECTION_LINES_KEY] += str + "\n";
		}		
	}//while

	//fclose(fp);
	return true;
}



#define GetIniStr(szSection, szKey, szDefault, strVal) GetPrivateProfileString_(szSection, szKey, szDefault, strVal, szFileName)

#define GetIniStr__(szSection, szKey, szDefault, func) \
(\
{\
	string strVal;\
	func( GetPrivateProfileString_(szSection, szKey, szDefault, strVal, szFileName) == 0 ? \
			szDefault : strVal.c_str() );\
}\
)

#define Str2BigendianIP(ip) CStr2BigendianIP( (ip).c_str() )


int EachLineToBigIP_func(const char*from, const char*to, void* para)
{
	string str(from, to);
	StringTrim(str," \t\r");
	if( !str.empty() ) {
		
		list<DWORD> *lst = ( list<DWORD> *)para;
		lst->push_back( Str2BigendianIP(str) );
		
		return 1;
	}
	
	return 0;
}

//parse error : -1; no such section/key : 0;  else 1
static int FindSectionKey(const char* section, const char* key, const char* file, string &val)//map<string,string>::iterator& itt)
{
//one file only
	//static 
	SECTION_t section_map;
	//static 
	SECTION_t::iterator it;
	//static 
	bool parse_ok =  ParseIniFile(file, section_map, STREQL(key, INI_SECTION_LINES_KEY) );

	if(!parse_ok) {
		return -1;
		}

	map<string,string>::iterator itt;
	if ( (it = section_map.find(section)) != section_map.end() &&  
		 (itt = it->second.find(key)) != it->second.end() ) 
	{
		//return 1;
		val.assign( itt->second );
		return 1;
	}

	return 0;
}


int GetPrivateProfileInt(const char* section, const char* key, int nDefault, const char* file)
{
	string val;
	switch ( FindSectionKey(section, key, file, val) ) {
		case -1:
		case  0:
			return nDefault;
		case  1:
			return atoi(val.c_str());
	}
}


//return count of lines
int ReadSectionLines(const string& strFileName, const string& strSection, EACH_LINE_FUNC each_line_func, void* para)
{

	string strLines;
	GetPrivateProfileString_(strSection.c_str(), INI_SECTION_LINES_KEY, "", strLines, strFileName.c_str());

	const char *begin, *p;
	begin = strLines.c_str();
	int count = 0;
	while ( p = strchr(begin, '\n') ) {
		count += each_line_func(begin, p, para);
		begin = p+1;	
	}

	return count;
}


int GetPrivateProfileString_(const char* section, const char* key, const char* strDefault, string &strVal, const char* file)
{
	strVal.clear();
	switch ( FindSectionKey(section, key, file, strVal) ) {
		case -1:			
			return 0;
		case  0:
			strVal = strDefault;
			return strVal.size();
		case  1:
			return strVal.size();
	}

}

const char* GetConfigFile(void)
{
	return m_szConfigFile;
}


ssize_t Readlink(const char *path, char *buf, size_t bufsiz=PATH_LENGTH)
{
	// readlink() does not append a null byte to buf. 

	int nRead = readlink(path, buf, bufsiz-1);

	if ( nRead != -1 )
		buf [ nRead ] = 0;
	else
		;
	
	return nRead;
}


bool ProcessAlreadyStartup2(pid_t pid, vector<pid_t>* pid_vec)
{
	bool ret = false;

	struct direct *ent;

	char current_process_name[256]={0};

	if ( -1 == Readlink("/proc/self/exe", current_process_name) )
		exit(9);

	//debug_log("%s",current_process_name);
	DIR *proc = opendir("/proc");


#if 1
	
	while(( ent=readdir(proc) )) {
	
		char link[128]={0};
		
		pid_t pidx;

		if (!isdigit(ent->d_name[0]))
			continue;

		pidx = STRTOUL(ent->d_name);
		if ( pidx==getpid() )
			continue;

		//if ( -1 == GetProcessName(pidx, str) )
		//	continue;

		sprintf(link, "/proc/%d/exe", pidx);

		if ( -1 == Readlink(link, link ) )
			continue;


		
		//debug_log("[%s],[%s]", link, current_process_name);
		
		if( STREQL(current_process_name, link) ) {
			ret = true;
			if( pid_vec )
				pid_vec->push_back(pidx);
		}
		
		//debugf("%s\n",str);
	}


#endif

	
	closedir(proc);

	return ret;
}

int GetProgramDir(char *program_dir)
{
	char source_link[PATH_LENGTH];
	char path[PATH_LENGTH];
	int ret;
	
	sprintf(path,"/proc/%d/exe",getpid());
	
	if ( (ret = readlink(path, source_link, PATH_LENGTH-1) )!= -1 ) {
		source_link[ret]=0;

		*(rindex(source_link, '/') +1)=0;
		
		//*(rindex(source_link, '/') )=0;
		
		strcpy(program_dir, source_link);		
		return 0;
	}
	return -1;
}


void LogInit(int argc, char** argv, bool bCheckIni = true)
{
	memset(m_szProgramdir, 0, sizeof(m_szProgramdir) );
	GetProgramDir(m_szProgramdir);

//printf_log("m_szProgramdir:%s", m_szProgramdir);
	
	( m_szProgramName = strrchr( argv[ 0 ], '/' ) ) ? m_szProgramName++ :  m_szProgramName = argv[ 0 ];

	sprintf(m_szConfigFile, "%s/%s.ini",	m_szProgramdir, m_szProgramName);
	sprintf(m_szConfigFileExtra, "%s/roledb_extra.ini",	m_szProgramdir);

//printf_log("m_szConfigFile:%s", m_szConfigFile);
//printf_log("m_szConfigFileExtra:%s", m_szConfigFileExtra);


	
	srandom( time(NULL) + getpid() );
	srand( time(NULL) + getpid() );
}

typedef void (*SPECIAL_KILL)( vector<pid_t>& pid_vec, const char* prog );

static void print_usage(const char* msg)
{
	printf("Usage: \n");
	printf("	 %s start\n", msg);
	printf("	 %s stop\n", msg);
	printf("	 %s status\n", msg);
	printf("	 %s reload\n", msg);
	printf("	 %s -v\n", msg);
}
pid_t GetPPid(pid_t x)
{
	char buf[512] = {};
	char file[256];

	int pid;
	char comm[512] = {};
	char state;
	int ppid = 0;

	sprintf(file, "/proc/%d/stat", x );

    int fd = open(file, O_RDONLY);

    read(fd, buf, sizeof(buf)-1);

	char *p1 = strstr(buf, "(");
	char *p2 = strstr(buf, ")");

	if(p1&&p2)
		memset(p1+1, 'X', p2-p1-1 );

    sscanf(buf, "%d %s %c %d", &pid, comm, &state, &ppid);
    close(fd);
	
	return ppid;	
}

vector<pid_t> Paternity(vector<pid_t> &pid_vec)
{
	vector<pid_t> v;
	
	for (int i=0; i<pid_vec.size(); i++) {

		for (int j=0; j<pid_vec.size(); j++) {
			
			if ( pid_vec[i] == GetPPid(pid_vec[j]) ) {
				
				v.push_back(pid_vec[i]);
				v.push_back(pid_vec[j]);
				return v;

			}
			else if( pid_vec[j] == GetPPid(pid_vec[i]) ) {
				
				v.push_back(pid_vec[j]);
				v.push_back(pid_vec[i]);
				return v;
			}
		}
	}

	return 	pid_vec;
}



void Usage(int argc, char** argv, const char* prog, const char* VERSION, SPECIAL_KILL stop_fun, SPECIAL_KILL reload_fun)
{
	vector<pid_t> pid_vec;
	ProcessAlreadyStartup2(getpid(), &pid_vec);

//syslog(1, "Usage");

	//PPP;	
	pid_vec = Paternity(pid_vec);
	//PPP;

	if(argc!=2) {
		print_usage(argv[0]);
		exit(EXIT_WRONG_PARA);
	}
	
	if( STREQL("start", argv[1]) ) {
		if ( !pid_vec.empty() ) {
			printf("There is an instance of %s already running!%s", prog, FAIL );
			exit(EXIT_FAILURE);
		}
		printf("Starting %s ...", prog);
		fflush(stdout);
	}
	else if( STREQL("stop", argv[1]) ) {
		if ( pid_vec.empty() ) {
			printf("%s is not running!%s", prog, FAIL);
			exit(EXIT_FAILURE);
		}
		printf("Shutting down %s ...", prog);

		if(stop_fun)
		stop_fun(pid_vec, prog);
		
		printf(SUCCESS);
		exit(0);
	}
	else if( STREQL("status", argv[1]) ) {
		if( pid_vec.empty() )
			printf("%s is stopped!\n", prog);
		else {
			printf("%s running (", prog);
			for(vector<pid_t>::const_iterator it=pid_vec.begin(); it!=pid_vec.end(); ++it) 
				printf(" %d ", *it);			
			//for_each(pid_vec.begin(), pid_vec.end(), bind1st( printf(" %d ") ) ) ;
			printf(")\n");
		}
		exit(0);
	}
	else if( STREQL("reload", argv[1]) ) {
		if( pid_vec.empty() )
			printf("%s is stopped!\n", prog);
		else if(reload_fun) {
			printf("%s reloading ...\n", prog);
			reload_fun(pid_vec, prog);
		}
		exit(0);
	}
	else if( STREQL("-v", argv[1]) ) {
		printf("version:%s\n", VERSION);
		exit(0);
	}
	
	else if( STREQL("-VV", argv[1]) ) {
		printf("%s\n", 	BUILD_TIMESTAMP);
		exit(0);
	}
	else {
		print_usage(argv[0]);
		exit(EXIT_WRONG_PARA);
	}
}

void SleepUninteruptable( time_t sec, long milli_sec)
{
	struct timespec req ;
	struct timespec rem ;

	req.tv_sec  = sec;
	req.tv_nsec = milli_sec * 1000000;

	//assert(req.tv_nsec>0 && req.tv_nsec<999999999);
	while( -1 == nanosleep(&req, &rem) ) {
		if(errno != EINTR )
			break;
		//debugf("%u--%u\n", rem.tv_sec, rem.tv_nsec);
		req = rem;
	}
}
pid_t Fork(void)
{
	pid_t pid = fork();
	if(pid == -1) {
		logx("FORK ERROR!");
		EXIT(EXIT_FAILURE);
	}
	return pid;
}
int MakePipeAndDaemon(void)
{
	int pfd[2];
	pipe(pfd);

	if ( Fork() > 0 ) {
		close(pfd[1]);
		char result[36]={0};
		read(pfd[0], result, 36);
		printf(result);
		exit( STREQL(result, FAIL) ?  EXIT_INIT : 0 );
	}

	close(pfd[0]);
	chdir("/tmp/"); //core dump to here, and please ummount 
	
	daemon(1,1);	
	return pfd[1];
}

//strictly, it isn't reentrant
static void print_killer ( int sn , siginfo_t  *si , void *sc)//struct ucontext *sc)
{
	logx("signal = %10s, senders' pid = %08u, sender's uid = %08u MYPID:%d", sys_siglist[si->si_signo], si->si_pid, si->si_uid,getpid());
}

void ReigsterSig(int reg)
{
	static struct sigaction orig_s[65];// 0 unuse, index 1..64

	struct sigaction s,olds;
	s.sa_flags = SA_SIGINFO;
	s.sa_sigaction = print_killer;

	for(int sig=1;sig<=64;sig++) {
		if(sig==SIGCHLD||sig==SIGKILL||sig==SIGSTOP||sig==SIGTSTP||sig==32||sig==33)
			continue;

		if(reg) {
			if(sigaction (sig, &s, &orig_s[sig])) {
				logx("Sigaction returned error = %d, signo:%s", errno, sys_siglist[sig]);
				EXIT(EXIT_FAILURE);
			}
		}
		else {
			if(sigaction (sig ,&orig_s[sig], NULL)) {
				logx("Sigaction returned error = %d, signo:%s", errno, sys_siglist[sig]);
				EXIT(EXIT_FAILURE);
			}
		}
	}
}

void MonitorInit(int nOutputPipe, const char* szProgName)
{
	ReigsterSig(1);
	while(1) {
		int status;
		pid_t worker;

		if( (worker = Fork())==0 ) {
			logx("monitor PID is %u, worker PID is %u (%s start normally.)",  getppid(), getpid(), szProgName );
			ReigsterSig(0);
			return;
		}
		else if(worker>0) {
			do {
				worker = wait(&status);
			} while ((worker == -1) && (errno == EINTR));

			if (WIFEXITED(status)) {
				int exit_code = WEXITSTATUS(status);
				logx("worker %u exited, status=%d", worker, exit_code);

				if(exit_code==EXIT_INIT) 
					write(nOutputPipe, FAIL, sizeof(FAIL)-1 );

				exit(exit_code);
			}
			else if (WIFSIGNALED(status)) {
				logx("worker %u killed by signal %s", worker, sys_siglist[WTERMSIG(status)]);
				//take care: killnew xxx; ./xxx stop
				//close(nOutputPipe);
			}
		}		
	}
}



void MonitorWorkerStop(vector<pid_t>& pid_vec, const char* szProgName)
{

	logx("monitor PID is %u (%s exit normally.)", pid_vec[0], szProgName);
	kill(pid_vec[0], SIGKILL);//kill monitor
	kill(pid_vec[1], SIGTERM);

	while( -1 != kill(pid_vec[1], 0) ) {
		//printf(". %d ", pid_vec[1]);
		printf(".");
		fflush(stdout);
		SleepUninteruptable(1,0);
	}
}

void MonitorWorkerReload(vector<pid_t>& pid_vec, const char* szProgName)
{
	if( pid_vec.size()>1 ) {	
		kill(pid_vec[1], SIGHUP);
	}

	
}


sighandler_t Signal(int signum, sighandler_t handler)
{
	if (SIG_ERR == signal(signum, handler) ) {
		int e = errno;
		fprintf(stderr, "signal install error: %m...\n");
		EXIT(EXIT_INIT);
	}
}


Sigfunc * signalll(int signo, Sigfunc *func)
{
	struct sigaction	act, oact;

	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

//	//if (signo == SIGALRM) {
//	if (signo == SIGUSR1) {
//		act.sa_flags |= SA_INTERRUPT;
//	} else {
//		act.sa_flags |= SA_RESTART;
//	}

	act.sa_flags |=  SA_RESTART;
//	act.sa_flags |= SA_INTERRUPT;

	if (sigaction(signo, &act, &oact) < 0)
		return(SIG_ERR);
	return(oact.sa_handler);
}



int Setrlimit(int resource, int SIZE)
{
	struct rlimit x = { (rlim_t)SIZE, (rlim_t)SIZE };
	int ret = setrlimit(resource, &x);
	if( ret == -1 ) {
		logv("setrlimit resource:%d, size:%d error(%d).", resource, SIZE, errno);
		//EXIT(EXIT_INIT);
	}

	{
		struct rlimit rl;
		if ( getrlimit(resource, &rl) == 0 )
		{
			logx("resource:%d	cur:%d	max:%d",  resource, rl.rlim_cur, rl.rlim_max );	
		}
	}

	return ret;	
}


void Setrlimit_()
{
	Setrlimit(RLIMIT_CORE, CORE_SIZE);
	Setrlimit(RLIMIT_MSGQUEUE, MSGQUEE_SIZE);
	Setrlimit(RLIMIT_NOFILE, OPEN_FILES_NUM*2);// *(long*)0 = 9;

	struct rlimit rl;
	if ( getrlimit(RLIMIT_NOFILE, &rl) == 0 )
	{
		//logx("RLIMIT_NOFILE: cur:%d   max:%d   ",  rl.rlim_cur, rl.rlim_max );	
	}

	if ( getrlimit(RLIMIT_STACK, &rl) == 0 )
	{
		//logx("RLIMIT_STACK: cur:%d   max:%d   ",  rl.rlim_cur, rl.rlim_max );	
	}
	
}

int Server_Main(int argc, char** argv, const char* prog, const char* VERSION, 
					SERVER_INIT_UINIT init_fun, SERVER_INIT_UINIT uninit_fun, 
					SERVER_INIT_UINIT oneshot_fun, SERVER_INIT_UINIT main_loop,
					sighandler_t term_handler, sighandler_t hup_handler, sighandler_t quit_handler)
{

	/*setlocale (LC_ALL, "");
    bindtextdomain (prog, LOCALEDIR);
    textdomain (prog);
    printf(_("GetText!\n"));*/

	LogInit(argc, argv);

	if( oneshot_fun )
		oneshot_fun(argc, argv);
	
	//=====================================
#ifndef _DEBUG
	Usage(argc, argv, prog, VERSION, MonitorWorkerStop, MonitorWorkerReload);
	//logx( "Program md5sum %s",  Md5Self() );

	int nOutputPipe = MakePipeAndDaemon();
	MonitorInit( nOutputPipe, prog );

#endif

	Setrlimit_();

	Signal(SIGTERM, term_handler);

//	Signal(SIGHUP, hup_handler);



	if (signalll(SIGHUP, hup_handler) == SIG_ERR) {
		fprintf (stderr, "Cannot handle hup_handler!\n");
		exit (8);
	}

	
	Signal(SIGQUIT, quit_handler);
	Signal(SIGPIPE, SIG_IGN);

	init_fun(argc, argv);

#ifndef _DEBUG
	//logv("nOutputPipe:%d", nOutputPipe );
	/*logv("%d, %d,", */write(nOutputPipe, SUCCESS, sizeof(SUCCESS)-1 );//);
	close(nOutputPipe);
#endif

	//CRoleDBCtrl::ThdEpollRead( &CRoleDBCtrl::m_SocketPair[SOCK_PAIR_CMD_READ] );
	main_loop(argc, argv);

	uninit_fun(argc, argv);

	logx("worker  PID is %u (%s exit normally.)",  getpid(), prog);
	exit(EXIT_GRACEFULLY);
}




string GetLogTime(time_t nTime)
{
	char szTimeBuf[MAX_FORMAT_LEN];
	struct tm tm;
	localtime_r(&nTime, &tm);

	strftime(szTimeBuf, MAX_FORMAT_LEN, "%Y-%m-%d %H:%M:%S", &tm);

	return szTimeBuf;//copy out
}


//inline  , => \,       \ => \\     \n => \E   
char* EscapeLogStr(char *szTgt, const char* szSrc)
{
	char* p = szTgt;
	char c;
	
	while( (c = *szSrc++) ) {
		
		if( c == ',' ) {
			*p++ = '\\';
			*p++ = ',';
		}
		else if( c == '\\' ) {
			*p++ = '\\';
			*p++ = '\\';
		}
		else if( c == '\n' ) {
			//*p++ = '\t';
			*p++ = '\\';
			*p++ = 'E';
		}
		/*else if( *szSrc == '\t' ) {
			*p++ = '\t';
			*p++ = '\t';
		}*/
		else
			*p++ = c;

	}

	return szTgt;
}




#define MAX_READ_LEN ( (1<<20) ) //max packet head len, don't use the len less than 64k

void EXIT(int exit_code)
{
	//bt(BACKTRACE_LEVEL);
	exit(exit_code);
}




void *Malloc(size_t size)
{
	void * m = malloc(size);
	if(m==NULL)
		EXIT(EXIT_INIT);

	return m;
}

#define TIME_2038_01_01	2145888000


void Allocate_Sock_Pro(MAP_SOCK_PRO &m, SOCKET sock)
{
	//never erase , always

	MAP_SOCK_PRO_IT it = m.find(sock);
	if( it != m.end() && it->second.szBuffer != NULL ) { //listen sock does not alloc 
		it->second.szCur = it->second.szBuffer + MAX_READ_LEN;
		return;
	}

	STC_SOCK_PRO	stc;
	stc.szBuffer	= (char*) Malloc( MAX_READ_LEN*2 );
	stc.szCur		= stc.szBuffer + MAX_READ_LEN;

#if USE_EPOLLWR_IOVEC
#error USE_EPOLLWR_IOVEC
	struct msghdr *msghdr	= ( struct msghdr * ) Malloc( sizeof(struct msghdr) );
	struct iovec* iov		= ( struct iovec* ) Malloc( IOV_MAX * sizeof(struct iovec) );
	memset(msghdr, 0, sizeof(struct msghdr) );
	msghdr->msg_iov = iov;
	msghdr->msg_iovlen = 0;

	stc.offset		= 0;
	stc.msghdr		= msghdr;

#elif USE_EPOLLWR_SENDBUF
#error USE_EPOLLWR_SENDBUF
	stc.szFrom		= stc.szTo = stc.szSendBuf = (char*) Malloc(CIRCLE_BUF_LEN);
	stc.bFull		= false;
#endif

	pthread_mutex_init( &stc.lock, NULL );
	stc.nTick = time(NULL);//TIME_2038_01_01;

	//m[sock] = stc;
	m.insert( MAP_SOCK_PRO::value_type(sock, stc) );
	
}


int Recv_Until_EAGAIN(SOCKET nSock, char *szBuf, char * &szCur, ForgePktAndPost_t ForgePktAndPost)
{
	//static int count;
	while(1) {

		int nGet = recv(nSock, szBuf+MAX_READ_LEN, MAX_READ_LEN, 0);//at most 65535
		//debugf("nGet:%d\n", nGet);
		//count++;
		//debugf("recv count : %d \n", count);
		if( nGet < 0 ) {	
			if( errno == EAGAIN )
				return 0;
			else if ( errno != EINTR ) {
				//debugf("[%s:%d]===========%m\n",__FUNCTION__, __LINE__ );
				return -1;
			}
		}
		else if( nGet == 0 ) {
			//debugf("[%s:%d]===========\n",__FUNCTION__, __LINE__ );
			return -1;
		}
		else { //nGet >0, at most 65535
			DWORD nLen = szBuf+MAX_READ_LEN - szCur + nGet;
			const char* pLeft = ForgePktAndPost(szCur, nLen, nSock);
			szCur = szBuf+MAX_READ_LEN-nLen;
			memmove( szCur, pLeft, nLen);

			if(nGet!=MAX_READ_LEN)
				return 0;
		}
		
	}
	
}




int Epoll_create(int size)
{
	int ret = epoll_create(size);
	if( ret == -1 )
		EXIT(EXIT_INIT);
	return ret;
}


int Sem_wait(sem_t *sem)
{
	int s;
	
	while ((s = sem_wait(sem)) == -1 && errno == EINTR) {		
		logv("sem_wait EINTR");
		continue;
	}

	if ( s == -1 ) {
		debugx_logx("sem_wait error %m:%d", errno);
		fprintf(stderr,"%m\n");
		EXIT(EXIT_INIT);
	}
		
	return 0;
}


void Sem_post(sem_t *sem)
{
	if( sem_post(sem) != 0 ) {
		fprintf(stderr,"%m\n");
		debugx_logx("sem_post error: %d", errno);
		EXIT(EXIT_INIT);
	}
}

void Sem_init(sem_t *sem, int pshared, unsigned int value)
{
	if(sem_init(sem,pshared, value)!=0){
		fprintf(stderr,"%m\n");
		debugx_logx("sem_init error: %d", errno);
		EXIT(EXIT_INIT);
	}
}


size_t GetTime(char *szTimeBuf, const char* szFormat, int nMinInterval )
{
	time_t tNow;
	time(&tNow);
	
	if( nMinInterval!=0 ) {
		int secs = nMinInterval * 60;
		tNow = ( tNow ) / secs * secs;
	}
	
	struct tm tm;
	localtime_r(&tNow, &tm);

	return strftime(szTimeBuf, MAX_FORMAT_LEN, szFormat, &tm);
}


int CreateDir(char* szDirName)
{
	char *p = szDirName;//strdupa(szDirName);
	char *head = p;

	while( p = strchr( ++p, '/' ) ) {

		*p = 0;
		if ( -1 == mkdir(head, 0755) && errno != EEXIST )
			return -1;
		*p = '/';
	}
	
	return 0;
}


//2009-05-12-13:59:30"
//2009-05-12-13.txt"
#define Y_OFFSET (szXXXX+0)
#define M_OFFSET (szXXXX+5)
#define D_OFFSET (szXXXX+8)
#define HH_OFFSET (szXXXX+11)
#define MM_OFFSET (szXXXX+14)
#define SS_OFFSET (szXXXX+17)

void GetLogFileName(char* szLogfile, char* szHeadFromat)
{
	char szXXXX[MAX_FORMAT_LEN];
	GetTime(szXXXX, "%Y-%m-%d-%H:%M:%S");
	
	*(D_OFFSET-1) = 0;

	char *p = szLogfile;
	p += sprintf(p, "%s/Log/", m_szProgramdir);
	p += sprintf(p, "%s/", szXXXX);

	if (CreateDir(szLogfile) < 0)
		return;

	*(D_OFFSET-1)	= '-';
	*(HH_OFFSET-1)	= 0;
	sprintf(p,"%s.txt",szXXXX);
	
	strcpy(szHeadFromat, HH_OFFSET);

}


int WriteNBytes(int fd, char *buf, unsigned int nbyte)
{
	int ret;
	char *p = buf;

	for (; nbyte; p += ret, nbyte -= ret) {
		
		ret = write(fd, p, nbyte);

		if ( ret < 0 ) {
			
			if ( errno == EINTR || errno == EAGAIN ) {
				ret = 0;
				continue;
			}
			return -1;
		}
	}
	
	return (p - buf);
}

void X_Log(const char* fmt, ... )
{
	char szLogfile[MAX_PATH_LEN];
	char szHeadFormat[MAX_PATH_LEN];
	GetLogFileName(szLogfile, szHeadFormat);

	int fd = open(szLogfile, O_WRONLY|O_CREAT|O_APPEND|O_LARGEFILE, 0666);
	if ( fd == -1)
		return;

	char buf[4096];
//	char buf[40960];
	char* p = buf;

	p += sprintf(p, "[%s]: ", szHeadFormat);

	va_list	ap;
	va_start(ap, fmt);
	
	int n = vsnprintf(p, sizeof(buf)-MAX_PATH_LEN, fmt, ap);

	//Thus, a return value of size or more means that the output was truncated. 
	if ( n >= sizeof(buf)-MAX_PATH_LEN ) {
		p += sizeof(buf)-MAX_PATH_LEN - 1;
		*(p-1) = '\n'; // 0 -> \n
	}
	else
		p += n;

	va_end(ap);

	//write(fd, buf, p-buf);
	WriteNBytes(fd, buf, p-buf);
	close(fd);
}



PSTC_Socket_And_CMD_GS MallocGSPacket(SOCKET nSock, WORD wCmdType, const char* data, int nDataLen)
{
	//PSTC_Socket_And_CMD_GS pkt = (PSTC_Socket_And_CMD_GS) malloc( sizeof(STC_Socket_And_CMD_GS) + nDataLen + 1 );
	PSTC_Socket_And_CMD_GS pkt = (PSTC_Socket_And_CMD_GS) malloc( sizeof(STC_Socket_And_CMD_GS) + nDataLen );
	if(pkt==NULL)
		return NULL;

	pkt->from = nSock;
	pkt->stc_cmd_gs.wLen = 4+nDataLen;
	pkt->stc_cmd_gs.wCmdType  = wCmdType;

	//pkt->stc_cmd_gs.parentheses = '(';
	//*(pkt->stc_cmd_gs.data + nDataLen) = ')';
	memcpy(pkt->stc_cmd_gs.data, data, nDataLen);

	return pkt;	
}



void OnClose_(SOCKET fd, MAP_SOCK_PRO &m)
{
	//logx("socket %d closed.", fd);

	close(fd);//auto rm from epfd	

	MAP_SOCK_PRO_IT it = m.find(fd);
	if ( it == m.end() )
		return;
		
	it->second.nTick = TIME_2038_01_01; //the body still in m
	if ( it->second.msghdr ==NULL )
		return;
	
    for( int i=0; i< it->second.msghdr->msg_iovlen; i++)
		free( it->second.msghdr->msg_iov[i].iov_base );

	it->second.msghdr->msg_iovlen = 0;

}

void OnClose_(SOCKET fd, int epfd, STC_SOCK_PRO &sp)
{
	logx("socket %d closed.", fd);
	//EPOLL_DEL(epfd, fd);

	close(fd);//auto rm from epfd	

	sp.nTick = TIME_2038_01_01; //the body still in m
	if ( sp.msghdr ==NULL )
		return;
	
    for( int i=0; i< sp.msghdr->msg_iovlen; i++)
		free( sp.msghdr->msg_iov[i].iov_base );

	sp.msghdr->msg_iovlen = 0;

}


bool IPInList(DWORD dwBigEndIP, const list<STC_IP_CHECK>& lstStcIPCheck)
{

	for( list<STC_IP_CHECK>::const_iterator it = lstStcIPCheck.begin(); it!= lstStcIPCheck.end(); ++it) {

		if ( it->dwBigEndIP == dwBigEndIP )
			return true;

		if ( !it->vecStar.empty() ) {

			DWORD dwAddr = dwBigEndIP;
			BYTE * y = (BYTE*) &dwAddr;
			BYTE * yy = (BYTE*) &it->dwBigEndIP;
			
			
			for ( int i=0; i<it->vecStar.size(); i++) {
				y[ it->vecStar[i] ]=0;
			}
			
			if ( it->dwBigEndIP == dwAddr )
				return true;
		}
		
	}
	
	return false;
}



int Accept(SOCKET nSock, const list<STC_IP_CHECK>& lstStcIPCheck)
{
	SOCKET nAccept;
	struct sockaddr_in saPeer;
	socklen_t nLen = sizeof( struct sockaddr_in );
	if( (nAccept = accept( nSock, ( struct sockaddr * )&saPeer, &nLen )) == -1 ){
		debug_log("errno:%d, sock:%d, len:%d, %m.", errno, nSock, nLen);
		return -1;
	}

	struct sockaddr_in saLocal;
	if( getsockname(nSock, ( struct sockaddr *)&saLocal, &nLen) == -1) {
		close(nAccept);
		return -1;
	}

	unsigned char *p= (unsigned char *)&saPeer.sin_addr.s_addr;
	unsigned char *q= (unsigned char *)&saLocal.sin_addr.s_addr;

	bool bOK = IPInList(saPeer.sin_addr.s_addr, lstStcIPCheck);

	if (!bOK) {
		debug_logx("Peer(%u.%u.%u.%u) dosen't in trustip list! Local(%u.%u.%u.%u:%u)", p[0], p[1], p[2], p[3], 
									q[0], q[1], q[2], q[3], ntohs(saLocal.sin_port) );
		close(nAccept);
		return -1;
	}
//fuck
#if 0
	//if( getsockname(nSock, ( struct sockaddr *)&saLocal, &nLen) == 0) {
	debugx_logx("Connected OK: Peer(%u.%u.%u.%u:%u), Local(%u.%u.%u.%u:%u), sock(%d)", 
						p[0], p[1], p[2], p[3], ntohs(saPeer.sin_port),
						q[0], q[1], q[2], q[3], ntohs(saLocal.sin_port),
						nAccept);
	//}
#endif
	return nAccept;

}



int pthread_create_detached(pthread_t * thread, void *(*start_routine)(void*), void * arg)
{
	pthread_attr_t attr;

	return  pthread_attr_init( &attr ) ||
			 pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED ) ||
			 pthread_create( thread, &attr, start_routine, arg ) ||
			 pthread_attr_destroy( &attr );

}

void Pthread_create_detached( void *(*start_routine)(void*), void * arg )
{
	static pthread_t hThread;

	if ( pthread_create_detached(&hThread, start_routine, arg) != 0 ) {
		fprintf(stderr,"%m\n");
		logx("pthread_create_detached error :%d", errno);
		EXIT(EXIT_INIT);
	}
}


string BigendianIP2Str(DWORD bigend_ip)
{
	struct in_addr a ;
	a.s_addr = bigend_ip;

	string str; 
	char cstr[INET_ADDRSTRLEN];
	if ( NULL != inet_ntop(AF_INET, &a, cstr, sizeof(cstr)) )
		str.assign(cstr);
	return str;
}



SOCKET TCPServerListen(short start_port, int bind_try_times, unsigned int BigEndianIP)
{
	struct sockaddr_in addr; 
	SOCKET listen_sock;
	int bind_count = 0;

	if( (listen_sock = socket(AF_INET, SOCK_STREAM, 0))== -1) {
		printf_log("socket error! %d.", errno);
		return -1;
	}

	const int on = 1; 
	if ( setsockopt( listen_sock, SOL_SOCKET, SO_REUSEADDR,  ( char * )&on, sizeof on ) == -1 ) {
		printf_log("setsockopt error! %d.", errno);
		goto close_sock_out;
    }

	while( bind_count<bind_try_times ) {
		memset(&addr, 0x00, sizeof(addr));
		addr.sin_family = AF_INET;

		addr.sin_addr.s_addr = BigEndianIP; //htonl(INADDR_ANY);
		addr.sin_port = htons(start_port++);

		if (bind(listen_sock, (struct sockaddr*)&addr, sizeof(addr)) != -1)
			break;
		
		if ( EADDRINUSE != errno ) {
			printf_log("bind %s:%u error!", BigendianIP2Str(addr.sin_addr.s_addr).c_str(), ntohs(addr.sin_port));
			goto close_sock_out;
		}
		bind_count++;
	}
	
	if (bind_count == bind_try_times) {
		printf_log("bind %s:%u error!", BigendianIP2Str(addr.sin_addr.s_addr).c_str(), ntohs(addr.sin_port));
		goto close_sock_out;
	}
	
	if (listen(listen_sock, 1024) == -1) {
		printf_log("listen error! %d.", errno);
		goto close_sock_out;
	}
	
	return listen_sock;

close_sock_out:
	close(listen_sock);
	return -1;

}


void GetEventLogFileName(char* szLogfile, int nChannel, int nMinInterval)
{
	char szXXXX[MAX_FORMAT_LEN];
	GetTime(szXXXX, "%Y-%m-%d-%H:%M:%S", nMinInterval);

	*(M_OFFSET-1) = *(D_OFFSET-1) = *(HH_OFFSET-1) = *(MM_OFFSET-1) = *(SS_OFFSET-1) = 0;
	
	char *p = szLogfile;
	p += sprintf(p, "%s/Log/%s%s%s/", m_szProgramdir, Y_OFFSET, M_OFFSET, D_OFFSET);

	if (CreateDir(szLogfile) < 0)
		return;

	//sprintf(p,"%d_%s%s%s_%s%s.txt", nChannel, Y_OFFSET, M_OFFSET, D_OFFSET, HH_OFFSET, MM_OFFSET );
	sprintf(p,"%s%s%s_%s%s.txt", Y_OFFSET, M_OFFSET, D_OFFSET, HH_OFFSET, MM_OFFSET );

}

//=====================================================================

int RecvNBytes(SOCKET hSocket, void* szBuf, int nLen)
{
	int	nLeft = nLen;
	char* pBuf = (char*) szBuf;
	while ( nLeft > 0 ) {
		
		int nGet = recv(hSocket, pBuf, nLeft, 0);
		if ( nGet  == -1 ) 	{
			if ( errno != EAGAIN && errno != EINTR ) {//when EAGAIN , busy write
				debugf("[%s:%d]===========%m\n",__FUNCTION__, __LINE__ );

				logv("RecvNBytes:errno:%d, hSocket:%d, nLen:%d, nLeft:%d", errno, hSocket, nLen, nLeft);
				return -1;
			}
			
			nGet = 0;
		}
		else if (nGet == 0)
			break;

		nLeft	-= nGet;
		pBuf	+= nGet;
	}
	
	return (nLen - nLeft);
}


void STC_CMD_EXTRA::Init(jmp_buf *pJmpBuf, MAP_SOCK_PRO	*pMapSockPro, int nHeartBeat, E_SOCK_PROPERTY eTimeOutCare, void *pvIni, int nEpollFd)
{
	this->pJmpBuf			= pJmpBuf;
	this->pMapSockPro		= pMapSockPro;
	this->nHeartBeat		= nHeartBeat;
	this->pvIni				= pvIni;
	this->eTimeOutCare		= eTimeOutCare;
	this->nEpollFd			= nEpollFd;
}


void STC_CMD_EXTRA::Reg_Cmd_Func(BYTE byCmd, CMD_EXEC func)
{
	mapCmdFunc[byCmd] = func;
}

void STC_CMD_EXTRA::ShutDown_No_HeartBeat(MAP_SOCK_PRO *m, int nHeartBeat, E_SOCK_PROPERTY e)
{
	for( MAP_SOCK_PRO_IT it = m->begin(); it!= m->end(); ++it ) {
		if ( ( it->second.sock_flag & e )  && time(NULL) - it->second.nTick > nHeartBeat )	{
			
			struct sockaddr_in saPeer={0};
			socklen_t nLen = sizeof( struct sockaddr_in );
			unsigned char *p= (unsigned char *)&saPeer.sin_addr.s_addr;
			getpeername(it->first, ( struct sockaddr *)&saPeer, &nLen);	
			debug_logx("sock(%d) no heart beat in %d seconds, flag:%x, Peer(%u.%u.%u.%u:%u)", 
								it->first, nHeartBeat, it->second.sock_flag, p[0], p[1], p[2], p[3], ntohs(saPeer.sin_port) );

			shutdown(it->first, SHUT_RDWR);//don't erase it
			it->second.nTick = TIME_2038_01_01; //the body still in m

		}
	}
}

//void STC_CMD_EXTRA::CMD(SOCKET fd, int epfd, pthread_rwlock_t *lock)
int STC_CMD_EXTRA::CMD(SOCKET fd, int epfd, pthread_rwlock_t *lock)
{
	BYTE byCmd = 0;
	RecvNBytes( fd, &byCmd, sizeof(BYTE) );
	
	if( byCmd == E_SOCKETPAIR_EXIT_THREAD) {
		//longjmp( *(pJmpBuf), 1 );
		return -1;
	}
	else if ( byCmd == E_SOCKETPAIR_CLEAR_CONN ) {
		
		pthread_rwlock_rdlock(lock);
		ShutDown_No_HeartBeat(pMapSockPro, nHeartBeat, eTimeOutCare);
		pthread_rwlock_unlock(lock);
	}
	else {

		MAP_CMD_FUNC::iterator it = mapCmdFunc.find(byCmd);
		if (it != mapCmdFunc.end()) {
		
			if ( byCmd == E_SOCKETPAIR_UPDATE_TRUSTIP ) {
				(*it->second)(this->pvIni);
			}
			else {
				(*it->second)(this);
			}
		}
	}

	/*else if ( wCmd == E_SOCKETPAIR_ADD_WRITE_EVENT) {
		EPOLL_MOD(epfd, (EPOLLIN|EPOLLOUT), wFd);
	}*/

	return 0;
}

//=====================================================================



