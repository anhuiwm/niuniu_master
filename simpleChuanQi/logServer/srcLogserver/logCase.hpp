	case E_LOG_S_CREATE_ROLE:
	{
		LOG_S_CREATE_ROLE* p = ((LOG_S_CREATE_ROLE*)data);
		BREAK_IF(LOG_S_CREATE_ROLE)
		xfprintf(fp, "%d,%llu,%s,%llu,%u,%s,%s\n", E_LOG_S_CREATE_ROLE, p->eventID,EscapeLogStr2( p->protocal ).c_str(),p->dwTime,p->dwRoleID,EscapeLogStr2( p->uuid ).c_str(),EscapeLogStr2( p->rolename ).c_str());
		break;
	}

	case E_LOG_S_LOGIN:
	{
		LOG_S_LOGIN* p = ((LOG_S_LOGIN*)data);
		BREAK_IF(LOG_S_LOGIN)
		xfprintf(fp, "%d,%llu,%s,%llu,%u\n", E_LOG_S_LOGIN, p->eventID,EscapeLogStr2( p->protocal ).c_str(),p->dwTime,p->dwRoleID);
		break;
	}

	case E_LOG_S_REQUEST_BUY_INGOT:
	{
		LOG_S_REQUEST_BUY_INGOT* p = ((LOG_S_REQUEST_BUY_INGOT*)data);
		BREAK_IF(LOG_S_REQUEST_BUY_INGOT)
		xfprintf(fp, "%d,%llu,%s,%llu,%u,%u\n", E_LOG_S_REQUEST_BUY_INGOT, p->eventID,EscapeLogStr2( p->protocal ).c_str(),p->dwTime,p->dwRoleID,p->dwChargeNum);
		break;
	}

	case E_LOG_S_PACKAGE_EXTEND:
	{
		LOG_S_PACKAGE_EXTEND* p = ((LOG_S_PACKAGE_EXTEND*)data);
		BREAK_IF(LOG_S_PACKAGE_EXTEND)
		xfprintf(fp, "%d,%llu,%s,%llu,%u,%u,%u,%u,%u,%u\n", E_LOG_S_PACKAGE_EXTEND, p->eventID,EscapeLogStr2( p->protocal ).c_str(),p->dwTime,p->dwRoleID,p->dwNeedCoin,p->dwNeedGold,p->dwRoleCoin,p->dwRoleGold,p->byCurEquipPkgLimit);
		break;
	}

	case E_LOG_S_SELL_ITEMS:
	{
		LOG_S_SELL_ITEMS* p = ((LOG_S_SELL_ITEMS*)data);
		BREAK_IF(LOG_S_SELL_ITEMS)
		xfprintf(fp, "%d,%llu,%s,%llu,%u,%u,%u,%u,%u\n", E_LOG_S_SELL_ITEMS, p->eventID,EscapeLogStr2( p->protocal ).c_str(),p->dwTime,p->dwRoleID,p->dwGetCoin,p->dwRoleCoin,p->wItemID,p->wItemNum);
		break;
	}

	case E_LOG_S_CONFIRM_STRENGTHEN:
	{
		LOG_S_CONFIRM_STRENGTHEN* p = ((LOG_S_CONFIRM_STRENGTHEN*)data);
		BREAK_IF(LOG_S_CONFIRM_STRENGTHEN)
		xfprintf(fp, "%d,%llu,%s,%llu,%u,%u,%u,%u,%u\n", E_LOG_S_CONFIRM_STRENGTHEN, p->eventID,EscapeLogStr2( p->protocal ).c_str(),p->dwTime,p->dwRoleID,p->dwNeedCoin,p->dwRoleCoin,p->wItemID,p->byStrengthLevel);
		break;
	}

	case E_LOG_S_CONNECT_PLAYER:
	{
		LOG_S_CONNECT_PLAYER* p = ((LOG_S_CONNECT_PLAYER*)data);
		BREAK_IF(LOG_S_CONNECT_PLAYER)
		xfprintf(fp, "%d,%llu,%llu,%s\n", E_LOG_S_CONNECT_PLAYER, p->eventID,p->dwTime,EscapeLogStr2( p->ip ).c_str());
		break;
	}

	case E_LOG_S_ONLINE_NUMS:
	{
		LOG_S_ONLINE_NUMS* p = ((LOG_S_ONLINE_NUMS*)data);
		BREAK_IF(LOG_S_ONLINE_NUMS)
		xfprintf(fp, "%d,%llu,%llu,%u\n", E_LOG_S_ONLINE_NUMS, p->eventID,p->dwTime,p->num);
		break;
	}

	case E_LOG_S_DISCONNECT_PLAYER:
	{
		LOG_S_DISCONNECT_PLAYER* p = ((LOG_S_DISCONNECT_PLAYER*)data);
		BREAK_IF(LOG_S_DISCONNECT_PLAYER)
		xfprintf(fp, "%d,%llu,%llu,%s,%u\n", E_LOG_S_DISCONNECT_PLAYER, p->eventID,p->dwTime,EscapeLogStr2( p->ip ).c_str(),p->dwRoleID);
		break;
	}

