	case E_LOG_GETUUID:
	{
		LOG_GETUUID* p = ((LOG_GETUUID*)data);
		BREAK_IF(LOG_GETUUID)
		xfprintf(fp, "%d,%llu,%s,%s,%s\n", E_LOG_GETUUID, p->eventID,EscapeLogStr2( p->cmdName ).c_str(),GetLogTime( (time_t)p->dwTime ).c_str(),EscapeLogStr2( p->uuid ).c_str());
		break;
	}

	case E_LOG_GMCUPCOUNT:
	{
		LOG_GMCUPCOUNT* p = ((LOG_GMCUPCOUNT*)data);
		BREAK_IF(LOG_GMCUPCOUNT)
		xfprintf(fp, "%d,%llu,%s,%s,%u,%u\n", E_LOG_GMCUPCOUNT, p->eventID,EscapeLogStr2( p->cmdName ).c_str(),GetLogTime( (time_t)p->dwTime ).c_str(),p->dwRoleID,p->count);
		break;
	}

	case E_LOG_CUPCOUNTRYID:
	{
		LOG_CUPCOUNTRYID* p = ((LOG_CUPCOUNTRYID*)data);
		BREAK_IF(LOG_CUPCOUNTRYID)
		xfprintf(fp, "%d,%llu,%s,%s,%u,%u\n", E_LOG_CUPCOUNTRYID, p->eventID,EscapeLogStr2( p->cmdName ).c_str(),GetLogTime( (time_t)p->dwTime ).c_str(),p->dwRoleID,p->byCup);
		break;
	}

	case E_LOG_CUPNEWSEASON:
	{
		LOG_CUPNEWSEASON* p = ((LOG_CUPNEWSEASON*)data);
		BREAK_IF(LOG_CUPNEWSEASON)
		xfprintf(fp, "%d,%llu,%s,%s,%u,%u,%u\n", E_LOG_CUPNEWSEASON, p->eventID,EscapeLogStr2( p->cmdName ).c_str(),GetLogTime( (time_t)p->dwTime ).c_str(),p->dwRoleID,p->byUp,p->byLevel);
		break;
	}

	case E_LOG_CUPCHALLENGE:
	{
		LOG_CUPCHALLENGE* p = ((LOG_CUPCHALLENGE*)data);
		BREAK_IF(LOG_CUPCHALLENGE)
		xfprintf(fp, "%d,%llu,%s,%s,%u,%u,%u\n", E_LOG_CUPCHALLENGE, p->eventID,EscapeLogStr2( p->cmdName ).c_str(),GetLogTime( (time_t)p->dwTime ).c_str(),p->dwRoleID,p->byCupDayCount,p->byCupCount);
		break;
	}

	case E_LOG_SELLITEM:
	{
		LOG_SELLITEM* p = ((LOG_SELLITEM*)data);
		BREAK_IF(LOG_SELLITEM)
		xfprintf(fp, "%d,%llu,%s,%s,%u,%u\n", E_LOG_SELLITEM, p->eventID,EscapeLogStr2( p->cmdName ).c_str(),GetLogTime( (time_t)p->dwTime ).c_str(),p->dwRoleID,p->sellPriceSum);
		break;
	}

	case E_LOG_CREATEBYTYPE:
	{
		LOG_CREATEBYTYPE* p = ((LOG_CREATEBYTYPE*)data);
		BREAK_IF(LOG_CREATEBYTYPE)
		xfprintf(fp, "%d,%llu,%s,%s,%u,%u,%u,%u\n", E_LOG_CREATEBYTYPE, p->eventID,EscapeLogStr2( p->cmdName ).c_str(),GetLogTime( (time_t)p->dwTime ).c_str(),p->dwRoleID,p->boxItemType,p->wItemID,p->dwNums);
		break;
	}

	case E_LOG_USEITEM:
	{
		LOG_USEITEM* p = ((LOG_USEITEM*)data);
		BREAK_IF(LOG_USEITEM)
		xfprintf(fp, "%d,%llu,%s,%s,%u,%u\n", E_LOG_USEITEM, p->eventID,EscapeLogStr2( p->cmdName ).c_str(),GetLogTime( (time_t)p->dwTime ).c_str(),p->dwRoleID,p->type);
		break;
	}

	case E_LOG_MATCHFIGHT:
	{
		LOG_MATCHFIGHT* p = ((LOG_MATCHFIGHT*)data);
		BREAK_IF(LOG_MATCHFIGHT)
		xfprintf(fp, "%d,%llu,%s,%s,%u,%u,%u\n", E_LOG_MATCHFIGHT, p->eventID,EscapeLogStr2( p->cmdName ).c_str(),GetLogTime( (time_t)p->dwTime ).c_str(),p->dwRoleID,p->dwMatchCount,p->byUsedChallengeCount);
		break;
	}

	case E_LOG_ADDEXP:
	{
		LOG_ADDEXP* p = ((LOG_ADDEXP*)data);
		BREAK_IF(LOG_ADDEXP)
		xfprintf(fp, "%d,%llu,%s,%s,%u,%u,%u,%u\n", E_LOG_ADDEXP, p->eventID,EscapeLogStr2( p->cmdName ).c_str(),GetLogTime( (time_t)p->dwTime ).c_str(),p->dwRoleID,p->wLevel,p->dwExp,p->dwAddExp);
		break;
	}

	case E_LOG_CHARGE:
	{
		LOG_CHARGE* p = ((LOG_CHARGE*)data);
		BREAK_IF(LOG_CHARGE)
		xfprintf(fp, "%d,%llu,%s,%s,%u,%u,%u,%u\n", E_LOG_CHARGE, p->eventID,EscapeLogStr2( p->cmdName ).c_str(),GetLogTime( (time_t)p->dwTime ).c_str(),p->dwRoleID,p->dwGold,p->dwCharge,p->dwChargeNum);
		break;
	}

	case E_LOG_MTREEUSE:
	{
		LOG_MTREEUSE* p = ((LOG_MTREEUSE*)data);
		BREAK_IF(LOG_MTREEUSE)
		xfprintf(fp, "%d,%llu,%s,%s,%u,%u,%u,%u\n", E_LOG_MTREEUSE, p->eventID,EscapeLogStr2( p->cmdName ).c_str(),GetLogTime( (time_t)p->dwTime ).c_str(),p->dwRoleID,p->dwGold,p->dwMoney,p->byUsedMoneyTreeTimes);
		break;
	}

	case E_LOG_VISITFRIEND:
	{
		LOG_VISITFRIEND* p = ((LOG_VISITFRIEND*)data);
		BREAK_IF(LOG_VISITFRIEND)
		xfprintf(fp, "%d,%llu,%s,%s,%u,%u,%u,%u\n", E_LOG_VISITFRIEND, p->eventID,EscapeLogStr2( p->cmdName ).c_str(),GetLogTime( (time_t)p->dwTime ).c_str(),p->dwRoleID,p->friendID,p->dwFriendPoints,p->dwHP);
		break;
	}

	case E_LOG_SELLCARD:
	{
		LOG_SELLCARD* p = ((LOG_SELLCARD*)data);
		BREAK_IF(LOG_SELLCARD)
		xfprintf(fp, "%d,%llu,%s,%s,%u,%u,%u,%u\n", E_LOG_SELLCARD, p->eventID,EscapeLogStr2( p->cmdName ).c_str(),GetLogTime( (time_t)p->dwTime ).c_str(),p->dwRoleID,p->wChairID,p->wUniqID,p->sellPriceSum);
		break;
	}

	case E_LOG_ADDCARDEXP:
	{
		LOG_ADDCARDEXP* p = ((LOG_ADDCARDEXP*)data);
		BREAK_IF(LOG_ADDCARDEXP)
		xfprintf(fp, "%d,%llu,%s,%s,%u,%u,%u,%u\n", E_LOG_ADDCARDEXP, p->eventID,EscapeLogStr2( p->cmdName ).c_str(),GetLogTime( (time_t)p->dwTime ).c_str(),p->dwRoleID,p->wChairID,p->wUniqID,p->dwExpGet);
		break;
	}

	case E_LOG_CARDTRAIN:
	{
		LOG_CARDTRAIN* p = ((LOG_CARDTRAIN*)data);
		BREAK_IF(LOG_CARDTRAIN)
		xfprintf(fp, "%d,%llu,%s,%s,%u,%u,%u,%u,%u,%u\n", E_LOG_CARDTRAIN, p->eventID,EscapeLogStr2( p->cmdName ).c_str(),GetLogTime( (time_t)p->dwTime ).c_str(),p->dwRoleID,p->dwMoney,p->wChairID,p->wUniqID,p->dwAddExp,p->wLevel);
		break;
	}

	case E_LOG_CARDRESET:
	{
		LOG_CARDRESET* p = ((LOG_CARDRESET*)data);
		BREAK_IF(LOG_CARDRESET)
		xfprintf(fp, "%d,%llu,%s,%s,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u\n", E_LOG_CARDRESET, p->eventID,EscapeLogStr2( p->cmdName ).c_str(),GetLogTime( (time_t)p->dwTime ).c_str(),p->dwRoleID,p->wChairID1,p->wUniqID1,p->wChairID2,p->wUniqID2,p->wChairID3,p->wUniqID3,p->wChairID4,p->wUniqID4,p->wChairID5,p->wUniqID5,p->wChairID6,p->wUniqID6,p->wChairID7,p->wUniqID7,p->wChairID8,p->wUniqID8,p->wChairID9,p->wUniqID9,p->wChairID10,p->wUniqID10,p->wChairID11,p->wUniqID11);
		break;
	}

	case E_LOG_UPCARDQ:
	{
		LOG_UPCARDQ* p = ((LOG_UPCARDQ*)data);
		BREAK_IF(LOG_UPCARDQ)
		xfprintf(fp, "%d,%llu,%s,%s,%u,%u,%u,%u\n", E_LOG_UPCARDQ, p->eventID,EscapeLogStr2( p->cmdName ).c_str(),GetLogTime( (time_t)p->dwTime ).c_str(),p->dwRoleID,p->wChairID,p->wUniqID,p->byQuality);
		break;
	}

	case E_LOG_EQUIP:
	{
		LOG_EQUIP* p = ((LOG_EQUIP*)data);
		BREAK_IF(LOG_EQUIP)
		xfprintf(fp, "%d,%llu,%s,%s,%u,%u,%u,%u\n", E_LOG_EQUIP, p->eventID,EscapeLogStr2( p->cmdName ).c_str(),GetLogTime( (time_t)p->dwTime ).c_str(),p->dwRoleID,p->wChairID,p->wUniqID,p->wEquipItemID);
		break;
	}

	case E_LOG_UPGRADEEQUIP:
	{
		LOG_UPGRADEEQUIP* p = ((LOG_UPGRADEEQUIP*)data);
		BREAK_IF(LOG_UPGRADEEQUIP)
		xfprintf(fp, "%d,%llu,%s,%s,%u,%u,%u,%u\n", E_LOG_UPGRADEEQUIP, p->eventID,EscapeLogStr2( p->cmdName ).c_str(),GetLogTime( (time_t)p->dwTime ).c_str(),p->dwRoleID,p->wUniqID,p->wEquipItemID,p->byLevel);
		break;
	}

	case E_LOG_UPGRADESKILL:
	{
		LOG_UPGRADESKILL* p = ((LOG_UPGRADESKILL*)data);
		BREAK_IF(LOG_UPGRADESKILL)
		xfprintf(fp, "%d,%llu,%s,%s,%u,%u,%u,%u,%u,%u\n", E_LOG_UPGRADESKILL, p->eventID,EscapeLogStr2( p->cmdName ).c_str(),GetLogTime( (time_t)p->dwTime ).c_str(),p->dwRoleID,p->dwMoney,p->wChairID,p->wUniqID,p->bySkillID,p->byLevel);
		break;
	}

	case E_LOG_RELOAD:
	{
		LOG_RELOAD* p = ((LOG_RELOAD*)data);
		BREAK_IF(LOG_RELOAD)
		xfprintf(fp, "%d,%llu,%s,%s,%u\n", E_LOG_RELOAD, p->eventID,EscapeLogStr2( p->cmdName ).c_str(),GetLogTime( (time_t)p->dwTime ).c_str(),p->dwRoleID);
		break;
	}

	case E_LOG_RANDCARD:
	{
		LOG_RANDCARD* p = ((LOG_RANDCARD*)data);
		BREAK_IF(LOG_RANDCARD)
		xfprintf(fp, "%d,%llu,%s,%s,%u,%u,%u\n", E_LOG_RANDCARD, p->eventID,EscapeLogStr2( p->cmdName ).c_str(),GetLogTime( (time_t)p->dwTime ).c_str(),p->dwRoleID,p->dwDropid,p->dwGold);
		break;
	}

	case E_LOG_UPGRADETAC:
	{
		LOG_UPGRADETAC* p = ((LOG_UPGRADETAC*)data);
		BREAK_IF(LOG_UPGRADETAC)
		xfprintf(fp, "%d,%llu,%s,%s,%u,%u,%u\n", E_LOG_UPGRADETAC, p->eventID,EscapeLogStr2( p->cmdName ).c_str(),GetLogTime( (time_t)p->dwTime ).c_str(),p->dwRoleID,p->dwTid,p->dwToLevel);
		break;
	}

	case E_LOG_SWITCHFORM:
	{
		LOG_SWITCHFORM* p = ((LOG_SWITCHFORM*)data);
		BREAK_IF(LOG_SWITCHFORM)
		xfprintf(fp, "%d,%llu,%s,%s,%u,%u,%u\n", E_LOG_SWITCHFORM, p->eventID,EscapeLogStr2( p->cmdName ).c_str(),GetLogTime( (time_t)p->dwTime ).c_str(),p->dwRoleID,p->dwTeamFightValue,p->byCurFormID);
		break;
	}

	case E_LOG_SWITCHTAC:
	{
		LOG_SWITCHTAC* p = ((LOG_SWITCHTAC*)data);
		BREAK_IF(LOG_SWITCHTAC)
		xfprintf(fp, "%d,%llu,%s,%s,%u,%u,%u\n", E_LOG_SWITCHTAC, p->eventID,EscapeLogStr2( p->cmdName ).c_str(),GetLogTime( (time_t)p->dwTime ).c_str(),p->dwRoleID,p->dwTeamFightValue,p->byCurTactics);
		break;
	}

	case E_LOG_UNLOCKFORM:
	{
		LOG_UNLOCKFORM* p = ((LOG_UNLOCKFORM*)data);
		BREAK_IF(LOG_UNLOCKFORM)
		xfprintf(fp, "%d,%llu,%s,%s,%u,%u,%u\n", E_LOG_UNLOCKFORM, p->eventID,EscapeLogStr2( p->cmdName ).c_str(),GetLogTime( (time_t)p->dwTime ).c_str(),p->dwRoleID,p->dwFormID,p->dwGold);
		break;
	}

	case E_LOG_UNLOCKTAC:
	{
		LOG_UNLOCKTAC* p = ((LOG_UNLOCKTAC*)data);
		BREAK_IF(LOG_UNLOCKTAC)
		xfprintf(fp, "%d,%llu,%s,%s,%u,%u,%u\n", E_LOG_UNLOCKTAC, p->eventID,EscapeLogStr2( p->cmdName ).c_str(),GetLogTime( (time_t)p->dwTime ).c_str(),p->dwRoleID,p->dwTacID,p->dwGold);
		break;
	}

	case E_LOG_SWAPCARD:
	{
		LOG_SWAPCARD* p = ((LOG_SWAPCARD*)data);
		BREAK_IF(LOG_SWAPCARD)
		xfprintf(fp, "%d,%llu,%s,%s,%u,%u,%u\n", E_LOG_SWAPCARD, p->eventID,EscapeLogStr2( p->cmdName ).c_str(),GetLogTime( (time_t)p->dwTime ).c_str(),p->dwRoleID,p->dwCardup,p->dwCarddown);
		break;
	}

	case E_LOG_SHOPEXCHANGE:
	{
		LOG_SHOPEXCHANGE* p = ((LOG_SHOPEXCHANGE*)data);
		BREAK_IF(LOG_SHOPEXCHANGE)
		xfprintf(fp, "%d,%llu,%s,%s,%u,%u,%u\n", E_LOG_SHOPEXCHANGE, p->eventID,EscapeLogStr2( p->cmdName ).c_str(),GetLogTime( (time_t)p->dwTime ).c_str(),p->dwRoleID,p->dwPricetype,p->dwPriceNum);
		break;
	}

	case E_LOG_BUYMATCHCOUNT:
	{
		LOG_BUYMATCHCOUNT* p = ((LOG_BUYMATCHCOUNT*)data);
		BREAK_IF(LOG_BUYMATCHCOUNT)
		xfprintf(fp, "%d,%llu,%s,%s,%u,%u,%u,%u\n", E_LOG_BUYMATCHCOUNT, p->eventID,EscapeLogStr2( p->cmdName ).c_str(),GetLogTime( (time_t)p->dwTime ).c_str(),p->dwRoleID,p->byBuyChallengeCount,p->byUsedChallengeCount,p->wBuyMatchCountCost);
		break;
	}

	case E_LOG_FIGHT:
	{
		LOG_FIGHT* p = ((LOG_FIGHT*)data);
		BREAK_IF(LOG_FIGHT)
		xfprintf(fp, "%d,%llu,%s,%s,%u,%u\n", E_LOG_FIGHT, p->eventID,EscapeLogStr2( p->cmdName ).c_str(),GetLogTime( (time_t)p->dwTime ).c_str(),p->dwRoleID,p->dwSceneID);
		break;
	}

	case E_LOG_CREATETEAM:
	{
		LOG_CREATETEAM* p = ((LOG_CREATETEAM*)data);
		BREAK_IF(LOG_CREATETEAM)
		xfprintf(fp, "%d,%llu,%s,%s,%u,%u\n", E_LOG_CREATETEAM, p->eventID,EscapeLogStr2( p->cmdName ).c_str(),GetLogTime( (time_t)p->dwTime ).c_str(),p->dwRoleID,p->dwTeamID);
		break;
	}

	case E_LOG_ADDFRIEND:
	{
		LOG_ADDFRIEND* p = ((LOG_ADDFRIEND*)data);
		BREAK_IF(LOG_ADDFRIEND)
		xfprintf(fp, "%d,%llu,%s,%s,%u,%u\n", E_LOG_ADDFRIEND, p->eventID,EscapeLogStr2( p->cmdName ).c_str(),GetLogTime( (time_t)p->dwTime ).c_str(),p->dwRoleID,p->dwFriendID);
		break;
	}

	case E_LOG_DELFRIEND:
	{
		LOG_DELFRIEND* p = ((LOG_DELFRIEND*)data);
		BREAK_IF(LOG_DELFRIEND)
		xfprintf(fp, "%d,%llu,%s,%s,%u,%u\n", E_LOG_DELFRIEND, p->eventID,EscapeLogStr2( p->cmdName ).c_str(),GetLogTime( (time_t)p->dwTime ).c_str(),p->dwRoleID,p->dwFriendID);
		break;
	}

