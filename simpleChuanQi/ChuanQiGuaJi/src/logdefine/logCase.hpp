	case E_LOG_S_CREATE_ROLE:
	{
		LOG_S_CREATE_ROLE* p = ((LOG_S_CREATE_ROLE*)data);
		BREAK_IF(LOG_S_CREATE_ROLE)
		xfprintf(fp, "%d,%u,%s,%u,%u,%s,%s\n", E_LOG_S_CREATE_ROLE, p->eventID,EscapeLogStr2( p->protocal ).c_str(),p->dwTime,p->dwRoleID,EscapeLogStr2( p->uuid ).c_str(),EscapeLogStr2( p->rolename ).c_str());
		break;
	}

	case E_LOG_S_LOGIN:
	{
		LOG_S_LOGIN* p = ((LOG_S_LOGIN*)data);
		BREAK_IF(LOG_S_LOGIN)
		xfprintf(fp, "%d,%u,%s,%u,%u\n", E_LOG_S_LOGIN, p->eventID,EscapeLogStr2( p->protocal ).c_str(),p->dwTime,p->dwRoleID);
		break;
	}

	case E_LOG_S_CONNECT_PLAYER:
	{
		LOG_S_CONNECT_PLAYER* p = ((LOG_S_CONNECT_PLAYER*)data);
		BREAK_IF(LOG_S_CONNECT_PLAYER)
		xfprintf(fp, "%d,%u,%u,%s\n", E_LOG_S_CONNECT_PLAYER, p->eventID,p->dwTime,EscapeLogStr2( p->ip ).c_str());
		break;
	}

	case E_LOG_S_ONLINE_NUMS:
	{
		LOG_S_ONLINE_NUMS* p = ((LOG_S_ONLINE_NUMS*)data);
		BREAK_IF(LOG_S_ONLINE_NUMS)
		xfprintf(fp, "%d,%u,%u,%u\n", E_LOG_S_ONLINE_NUMS, p->eventID,p->dwTime,p->num);
		break;
	}

	case E_LOG_S_DISCONNECT_PLAYER:
	{
		LOG_S_DISCONNECT_PLAYER* p = ((LOG_S_DISCONNECT_PLAYER*)data);
		BREAK_IF(LOG_S_DISCONNECT_PLAYER)
		xfprintf(fp, "%d,%u,%u,%s,%u\n", E_LOG_S_DISCONNECT_PLAYER, p->eventID,p->dwTime,EscapeLogStr2( p->ip ).c_str(),p->dwRoleID);
		break;
	}

	case E_LOG_S_TICK_PLAYER_INFO:
	{
		LOG_S_TICK_PLAYER_INFO* p = ((LOG_S_TICK_PLAYER_INFO*)data);
		BREAK_IF(LOG_S_TICK_PLAYER_INFO)
		xfprintf(fp, "%d,%u,%u,%u,%s,%u,%u,%u,%u,%u,%u,%u,%u\n", E_LOG_S_TICK_PLAYER_INFO, p->eventID,p->dwTime,p->id,EscapeLogStr2( p->rolename ).c_str(),p->job,p->sex,p->level,p->viplevel,p->exp,p->money,p->gold,p->charge);
		break;
	}

