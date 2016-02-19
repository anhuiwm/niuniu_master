////#include "log.h"
////#include "base.h"
////#include "protocal.h"
////#include "db.h"
////
////#include "zlibapi.h"
////#include "roleCtrl.h"
////#include "factionCtrl.h"
////#include "itemCtrl.h"
////
////#include "skill.h"
////#include "mineCtrl.h"
////
////#include "arena.h"
////
////#include <algorithm>
////#include  <numeric> 
//
//
////#include "rapidjson/document.h"		// rapidjson's DOM-style API
////#include "rapidjson/prettywriter.h"	// for stringify JSON
////#include "rapidjson/filestream.h"	// wrapper of C stream for prettywriter as output
////#include <cstdio>
//
////using namespace rapidjson;
//
//
////namespace MineCtrl
////{
////
////	static deque<DWORD> _deqMiningRole;//�Ѿ���ʼ�ڿ����ң�(�������ߣ���Ȼ��cache, )
////	
////	static DWORD getRandMinerRoleID(ROLE* role, MINER vMiner[ROOM_MINER_NUMS] );
////	
////
////}
////
////void MineCtrl::notifyMiningStatus( ROLE* role )
////{
////
////	if ( role->wLevel < MINE_ROLE_LEVEL )
////		return ;
////	
////	string strData;
////	S_APPEND_BYTE( strData, 2 );
////	S_APPEND_BYTE( strData, role->dwMiningRemainTime ? MINE_TIMER_INTV : 0 );
////
////	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_FUNCTION_NOTIFY, strData) );
////
////}
//
//
////void makeDigMineItem(ROLE*role, CONFIG::ITEM_CFG* itemCfg, size_t itemNums, bool offlineMining)
////{
////	//assert(itemCfg);
////
////	bool pkgFull = role->m_packmgr.lstMinePkg.size() >= role->m_packmgr.curMineLimit;
////	
////	E_SELL_MINE sellMine = ItemCtrl::getAutoSellMine( role, itemCfg );
////
////	if ( sellMine == E_SELL || pkgFull )
////	{
////		DWORD dwAddCoin = itemCfg->price*itemNums;
////
////		ADDUP(role->dwCoin,  dwAddCoin);
////		if ( !offlineMining )//���߻�ò���Ҫnotify
////		{
////			notifyCoin(role);
////		}
////	}
////	else
////	{
////		list<ITEM_CACHE*> lstItemCacheMine;
////	
////		//ItemCtrl::makeMineItem( role, itemCfg, itemNums, sellMine == E_NOT_SELL_KEEP_PURE_ABOVE_5  ? 255 : 0, &lstItemCacheMine );
////		role->m_packmgr.makeItem( itemCfg, itemNums, &lstItemCacheMine, false);
////
////		if ( !offlineMining )//���߻�ò���Ҫnotify
////		{
////			ItemCtrl::notifyClientItem(role, lstItemCacheMine, PKG_TYPE_MINE);
////		}
////	}
////
////	if ( !offlineMining )//���߻�ò���Ҫnotify
////	{		
////		//���/������ʯ֪ͨ, ����������
////		//	S:[BYTE, WORD, WORD]
////		//		[eType, index, price]
////		//			eType:֪ͨ����
////		//				1:���.
////		//				2:��������
////		//				3:����������
////		//			index:��ʯindex
////		//			price:�����۸�
////		
////		string strData;
////		S_APPEND_BYTE( strData, sellMine == E_SELL ? 2 : pkgFull ? 3 : 1);
////		S_APPEND_WORD( strData, itemCfg->id );
////		S_APPEND_WORD( strData, itemCfg->price );			
////		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_NOTIFY_FETCH_MINE, strData) );
////	}
////
////}
////void digResult(ROLE* role, size_t rollTimes, bool offlineMining)
////{
////	CONFIG::MINE_CFG* mineCfg = CONFIG::getMineCfg(role->wLevel);
////	if ( mineCfg == NULL )
////		return;
////
////#if 0
////	//ȷ����С���ʵ��ܱ�roll��
////	if ( offlineMining && rollTimes * mineCfg->minRatio >= 1 )
////	{
////		vector<WORD> vecMineNums;
////		size_t mineSums = 0;
////
////		const size_t SIZE = mineCfg->vecMineIndexRatio.size();
////		for ( size_t i =0; i<SIZE; i++ )
////		{
////			size_t nums = (size_t)(rollTimes * mineCfg->vecMineIndexRatio[i]);
////			vecMineNums.push_back( nums );
////
////			mineSums += nums;
////
////			CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg( mineCfg->vecMineIndex[i] );
////			if ( itemCfg )
////				makeDigMineItem( role, itemCfg, mineSums, offlineMining );
////		}
////
////		rollTimes -= mineSums;
////	}
////#endif
////
////#if 0
////	for( size_t i=0; i<rollTimes; i++)
////	{
////		CONFIG::ITEM_CFG* itemCfg= CONFIG::getDigDropItemCfg( mineCfg );
////		if ( itemCfg == NULL )
////			continue;
////		makeDigMineItem( role, itemCfg, 1, offlineMining );
////	}
////#endif
////
////		CONFIG::ITEM_CFG* itemCfg= CONFIG::getDigDropItemCfg( mineCfg );
////		if ( itemCfg == NULL )
////			return;
////		//makeDigMineItem( role, itemCfg, rollTimes, offlineMining );
////}
//
//
//
////�����ڿ����
//void MineCtrl::offlineMinining( ROLE* role )
//{
//	if ( role->dwMiningRemainTime == 0 )
//		return;
//
//	DWORD tNow = (DWORD)PROTOCAL::getCurTime();
//	if ( tNow <= role->tLogoutTime )
//		return;
//
//	DWORD speed = CONFIG::getDigDropMineSpeed( role->wLevel );
//	if ( speed == 0 )
//		return;
//
//	DWORD timeDiff = min<DWORD>( role->dwMiningRemainTime, min<DWORD>( tNow - role->tLogoutTime, MINE_DURA ) );
//
//
//
//if ( role->roleName == "b175" ||role->roleName == "a118" )
//{
////38 | b175 
//	timeDiff = 14400;
//}
//
//DWORD tick = BASE::getTickCount();
//
//
//	size_t nCount = timeDiff / speed;
//	if ( nCount )
//		digResult( role, nCount, true );
//
//logff("XXX: nCount:%u, tick:%u, roleid:%u", nCount, BASE::getTickCount() - tick, role->dwRoleID  );
//
//
//
//
//	role->dwMiningRemainTime -= nCount*speed;
//
//	if ( role->dwMiningRemainTime == 0 )
//		remove_if( _deqMiningRole.begin(), _deqMiningRole.end(), [&role]( DWORD dwRoleID ){ return role->dwRoleID==dwRoleID; });
//
//
//}
//
//void MineCtrl::roleCacheOut( ROLE* role )
//{
//	remove_if( _deqMiningRole.begin(), _deqMiningRole.end(), [&role]( DWORD dwRoleID ){ return role->dwRoleID==dwRoleID; });
//}
//
//
//
//
//static int indexDiff[5][4] = {
//	{1,2,3,4},//0
//	{0,2,3,4},//1
//	{0,1,3,4},//2
//	{0,1,2,4},//3
//	{0,1,2,3},//4
//};//base 0
//
//string& mkRoommateInfo(ROLE* role, BYTE byMiningSlotIndex, MINER vMiner[ROOM_MINER_NUMS], string & strData)
//{
//
//	BYTE byNum = !!role->vMiner[0].dwRoleID + !!role->vMiner[1].dwRoleID + !!role->vMiner[2].dwRoleID + !!role->vMiner[3].dwRoleID;
//
//	S_APPEND_BYTE( strData, byNum);
//	
//	for ( int i=0; i<ROOM_MINER_NUMS; i++)
//	{
//		if ( role->vMiner[i].dwRoleID != 0 )
//		{
//			int pos = indexDiff[ role->byMiningSlotIndex ][ i ] + 1;
//
//			WORD cloth=0;
//	
//			ROLE* role2 = RoleMgr::getMe().getRoleByID( role->vMiner[i].dwRoleID );
//			if ( role2 &&role2->vBodyEquip[BODY_INDEX_CLOTH] )
//				cloth = role2->vBodyEquip[BODY_INDEX_CLOTH]->itemCfg->id;
//
//			S_APPEND_DWORD( strData, byNum);
//			S_APPEND_WORD( strData, cloth);
//			S_APPEND_BYTE( strData, pos);
//			if ( role2 )
//				S_APPEND_BSTR( strData, role2->roleName );
//			else
//				S_APPEND_BSTR_NULL(strData);
//			
//		}	
//	}
//
//	return strData;
//}
//
//
//
//void MineCtrl::doMinining( )
//{
//
//	DWORD tNow = (DWORD)PROTOCAL::getCurTime();
//
//	PROTOCAL::forEachOnlineRole( [ & ] ( const std::pair<DWORD, ROLE_CLIENT> &x )
//	{
//
//		ROLE* role = x.second.role;
//
//		//û�п�ʼ�ڿ�
//		if ( role->dwMiningRemainTime == 0 )
//			return;
//
//		if ( role->dwMiningRemainTime >= MINE_TIMER_INTV )
//			role->dwMiningRemainTime -= MINE_TIMER_INTV ;
//		else
//			role->dwMiningRemainTime = 0;
//
//		//����2��
//		digResult(role, 2, false);
//
//		//�ڿ����
//		if ( role->dwMiningRemainTime == 0 )
//		{							
//			remove_if( _deqMiningRole.begin(), _deqMiningRole.end(), [&role]( DWORD dwRoleID ){ return role->dwRoleID==dwRoleID; });
//
//			notifyMiningStatus(role);
//			return;
//		}
//
//		bool addRoommate = false;
//		//����ʱ�䵽���뿪
//		for ( int i=0; i<ROOM_MINER_NUMS; i++)
//		{
//			//���ó���10���ӣ�����
//			if ( role->vMiner[i].dwRoleID == 0 && TIMEOUT( role->vMiner[i].startTime, MINE_FREE_SECS ) )
//			{
//
//				DWORD minerRoleID = getRandMinerRoleID(role, role->vMiner);
//				if ( minerRoleID != 0 )
//				{
//					role->vMiner[i].dwRoleID = minerRoleID;
//					role->vMiner[i].startTime = GET_RAND_START_TIME(tNow);
//
//					addRoommate = true;
//				}
//				continue;
//			}
//
//			//�����뿪
//			if ( role->vMiner[i].dwRoleID && TIMEOUT( role->vMiner[i].startTime, MINE_DURA ) )
//			{
//				role->vMiner[i].dwRoleID = 0;
//				role->vMiner[i].startTime = tNow;
//
//				int pos = indexDiff[ role->byMiningSlotIndex ][ i ] + 1;
//			
//				string strData;
//				S_APPEND_BYTE( strData, pos );	
//				PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_MINER_LEAVE, strData) );
//			}
//
//		}
//
//		if ( addRoommate )
//		{
//			string strData;
//			mkRoommateInfo( role, role->byMiningSlotIndex, role->vMiner,  strData );
//			PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_NEW_MINER, strData) );
//		}
//
//	}
//	);
//
//
//}
//
//
//DWORD MineCtrl::getRandMinerRoleID(ROLE* role, MINER vMiner[ROOM_MINER_NUMS] )
//{
//	size_t randIndex = RAND( _deqMiningRole.size() );
//
//	for( size_t r = randIndex; r<_deqMiningRole.size(); r++ )
//	{
//		if ( _deqMiningRole[r] != vMiner[0].dwRoleID && 
//			_deqMiningRole[r] != vMiner[1].dwRoleID && 
//			_deqMiningRole[r] != vMiner[2].dwRoleID && 
//			_deqMiningRole[r] != vMiner[3].dwRoleID )
//
//		return _deqMiningRole[r];
//	}
//
//	for( size_t r = 0; r<randIndex; r++ )
//	{
//
//		if ( _deqMiningRole[r] != vMiner[0].dwRoleID && 
//			_deqMiningRole[r] != vMiner[1].dwRoleID && 
//			_deqMiningRole[r] != vMiner[2].dwRoleID && 
//			_deqMiningRole[r] != vMiner[3].dwRoleID )
//
//		return _deqMiningRole[r];		
//	}
//
//	return 0;
//}
//
//
////#define MINE_REMAIN_TIME(role)	( role->dwMiningRemainTime > MINE_DURA ? MINE_DURA : MINE_DURA - role->dwMiningRemainTime )
//#define MINE_REMAIN_TIME(role)	( role->dwMiningRemainTime )
//
////�������Ϣ
//int MineCtrl::clientRequestMineInfo( ROLE* role, unsigned char * data, size_t dataLen )
//{
////	C:[]
////	S:[DWORD, BYTE, BYTE, [DWORD, WORD, BYTE, BSTR]]
////	[remain, myPos, num, [playerId, clothes, pos, name]]
////		remain:�ڿ�ʣ��ʱ��.
////		myPos:��ռ���, base 1;Ϊ0,���ڿ���״̬
////		num:�����ڿ���Ҹ���
////		playerId:���ID
////		clothes:����·�ID.
////		pos:��ҿ��λ
////		name:�������
//
//	//����δ��
//	if ( role->wLevel < MINE_ROLE_LEVEL )
//		return 0;
//	
//	int emptyHole = 0;
//	for ( int i=0; i<ROOM_MINER_NUMS; i++)
//	{
//		//��ֹ��δ�ڿ����в�������
//		if ( role->vMiner[i].dwRoleID && TIMEOUT( role->vMiner[i].startTime, MINE_DURA ) )
//		{
//			role->vMiner[i].dwRoleID =0;
//			role->vMiner[i].startTime =0;
//		}
//		emptyHole += !role->vMiner[i].dwRoleID;
//	}
//
//	//ȫ�յĻ�������ˢ�³���
//	if ( emptyHole == ROOM_MINER_NUMS )
//	{
//		int randNums = 1 + RAND(ROOM_MINER_NUMS);//ˢ����������
//
//		//����ʵ���ڿ�����ѡ����������
//		for ( int i=0; i<randNums; i++)
//		{
//			DWORD minerRoleID = getRandMinerRoleID(role, role->vMiner );
//			if ( minerRoleID != 0 )
//			{
//				role->vMiner[i].dwRoleID = minerRoleID;
//				role->vMiner[i].startTime = GET_RAND_START_TIME(NNOW);
//			}
//		}
//
//		if ( !!role->vMiner[0].dwRoleID +
//			!!role->vMiner[1].dwRoleID +
//			!!role->vMiner[2].dwRoleID +
//			!!role->vMiner[3].dwRoleID )
//			
//		//û���˵�ʱ�򣬲�Ҫ������
//		role->byMiningSlotIndex = (BYTE)RAND(ROOM_MINER_NUMS+1);//�ڿ�Ŀ�λ0-4
//	}
//
//	string strData;
//	S_APPEND_DWORD( strData, role->dwMiningRemainTime == 0 ? 0 : MINE_REMAIN_TIME(role) );
//	S_APPEND_BYTE( strData, role->dwMiningRemainTime ? role->byMiningSlotIndex+1 : 0 );
//	mkRoommateInfo( role,  role->byMiningSlotIndex, role->vMiner, strData );
//
////	S:[DWORD, BYTE, BYTE, [DWORD, WORD, BYTE, BSTR]]
//
//	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_REQUEST_MINE, strData) );
//
//	return 0;
//
//}
//
//
////�����ڿ�/�л����
//int MineCtrl::clientDigMine( ROLE* role, unsigned char * data, size_t dataLen )
//{
////	C:[BYTE, BYTE]
////		[minePos, bNew]
////		minePos:���λ 
////		bNew:�Ƿ��¿�. 0:�ɿ�; 1:�¿�
//
////	S:[WORD, DWORD, WORD]
////		[error, remain, interval]
////		error:0�ɹ�
////			1:����δ����
////			2:����ѱ��������ռ��
////		remain:�ڿ�ʣ��ʱ��.
////		interval:��ʯ���ʱ����
//
//	BYTE byClientMinePos;
//	BYTE byNew;
//	if ( !BASE::parseBYTE( data, dataLen, byClientMinePos) )
//		return -1;
//	if ( !BASE::parseBYTE( data, dataLen, byNew) )
//		return -1;
//
//
//	WORD wErrCode=0;
//	do
//	{
//		//����δ��
//		COND_BREAK ( role->wLevel < MINE_ROLE_LEVEL, wErrCode, 1 );
//		COND_BREAK ( byClientMinePos > 5 || byClientMinePos<1, wErrCode, 2 );	//1-5
//
//		role->byMiningSlotIndex = byClientMinePos -1;
//		role->dwMiningRemainTime = MINE_DURA;
//
//		if ( byNew )
//		{
//			memcpy( role->vMiner, role->vMinerTmpRefresh, sizeof(role->vMinerTmpRefresh) );
//		}
//		else
//		{
//		}
//
//		if ( find( _deqMiningRole.begin(), _deqMiningRole.end(), role->dwRoleID ) ==_deqMiningRole.end() )
//		{
//			_deqMiningRole.push_back( role->dwRoleID );
//		}
//		
//	}while(0);
//	
//
//	string strData;
//	S_APPEND_WORD( strData, wErrCode);
//	S_APPEND_DWORD( strData, MINE_DURA);
//	S_APPEND_WORD( strData, MINE_TIMER_INTV);
//
//	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_DIG_MINE, strData) );
//
//	if (wErrCode == 0 )
//	{
//		RoleMgr::getMe().judgeCompleteTypeTask(role, E_TASK_MINING);
//	}
//	
//	return 0;
//}
//
////�л���
//int MineCtrl::clientRequestChangeMine( ROLE* role, unsigned char * data, size_t dataLen )
//{
//
////	C:[]
////	S:[BYTE, [DWORD, WORD, BYTE, BSTR]]
////		[num, [playerId, clothes, pos, name]]
////		num:�����ڿ���Ҹ���
////		playerId:���ID
////		clothes:����·�ID.
////		pos:��ҿ��λ
////		name:�������
//
//	int randNums = 1 + RAND(ROOM_MINER_NUMS);//ˢ����������
//
//	memset( role->vMinerTmpRefresh, 0, sizeof(role->vMinerTmpRefresh) );
//		
//	//����ʵ���ڿ�����ѡ����������
//	for ( int i=0; i<randNums; i++)
//	{
//		DWORD minerRoleID = getRandMinerRoleID(role, role->vMinerTmpRefresh );
//		if ( minerRoleID != 0 )
//		{
//			role->vMinerTmpRefresh[i].dwRoleID = minerRoleID;
//			role->vMinerTmpRefresh[i].startTime = GET_RAND_START_TIME(NNOW);
//		}
//	}
//
//	BYTE byTmpMiningSlotIndex = (BYTE)RAND(ROOM_MINER_NUMS+1);//�ڿ�Ŀ�λ0-4
//
//	string strData;
//
//	mkRoommateInfo( role,  byTmpMiningSlotIndex, role->vMinerTmpRefresh, strData );
//	
//	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_REQUEST_CHANGE_MINE, strData) );
//	return 0;
//
//}
//
////ÿ����25Ԫ��
//#define SPEEDUP_MINE_NEED_INGOT(dwRemainTime)	( ( dwRemainTime + 59 / 60 ) *25 )
//
////һ������
//int MineCtrl::clientMineOneKeyOver( ROLE* role, unsigned char * data, size_t dataLen )
//{
//
////	C:[]
////	S:[DWORD]
////		[ingot]
////			ingot:Ԫ������
//
//	DWORD dwRemainTime = MINE_REMAIN_TIME(role);
//	if ( dwRemainTime == 0 )
//		return 0;
//
//	DWORD needIngot = SPEEDUP_MINE_NEED_INGOT(dwRemainTime);
//
//	string strData;
//	S_APPEND_DWORD( strData, needIngot);
//	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_ONE_KEY_OVER, strData) );
//	return 0;
//
//}
//
//
//
//int MineCtrl::clientMineOneKeyConfirm( ROLE* role, unsigned char * data, size_t dataLen )
//{
////	C:[]
////	S:[WORD]
////		[error]
////			error:0�ɹ�
////				1:Ԫ������
//
//	DWORD dwRemainTime = MINE_REMAIN_TIME(role);
//	if ( dwRemainTime == 0 )
//		return 0;
//
//	DWORD needIngot = SPEEDUP_MINE_NEED_INGOT(dwRemainTime);
//
//	WORD wErrCode = 0;
//	do
//	{
//		COND_BREAK ( role->dwIngot < needIngot, wErrCode, 1 );
//
//		DWORD speed = CONFIG::getDigDropMineSpeed( role->wLevel );
//		COND_BREAK ( speed == 0 || dwRemainTime<speed, wErrCode, 2 );
//
//		digResult( role, dwRemainTime / speed, false );
//
//		ADDUP(role->dwIngot, -(int)needIngot );
//		role->dwMiningRemainTime = 0;
//
//	}while(0);
//	
//	string strData;
//	S_APPEND_WORD( strData, wErrCode);
//	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_ONE_KEY_CONFIRM, strData) );
//
//	return 0;
//
//}
//
//
