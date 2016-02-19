#ifndef FIGHTCTRL_H
#define FIGHTCTRL_H


enum eFIGHTTYPE
{
	E_FIGHT_MON = 1,		   //打怪
	E_FIGHT_BOSS = 2,		   //BOSS战
	E_FIGHT_ARENA_PLAYER = 3,		   //竞技场PK
	E_FIGHT_HERO = 4,		   //精英BOSS战
	E_FIGHT_MEET_PLAYER = 5,		   //偶遇PK
};

struct TFightDrop
{

	list<WORD> lstDropMonster;

	BYTE byDropType;//最近需要掉落类型

	WORD wDropMap;//最近需要掉落的地图

	WORD wBossID;//boss
	void resetDrop();

	void setDrop( list<WORD>& lstMons, const BYTE dropType, const WORD map, const WORD boss = 0 );

	TFightDrop()
	{
		resetDrop();
	}

};


	struct EFFECT_ROUND
	{
		WORD level;
		BYTE type;
		BYTE round;

		EFFECT_ROUND( const EFFECT_ROUND& dst )
		{
			this->level = dst.level;
			this->type = dst.type;
			this->round = dst.round;
		}

		EFFECT_ROUND& operator = ( const EFFECT_ROUND& dst )
		{
			if ( this == &dst )
			{
				return *this;
			}

			this->level = dst.level;
			this->type = dst.type;
			this->round = dst.round;

			return *this;
		}

		EFFECT_ROUND( WORD level, BYTE type, BYTE round )
		{
			this->level = level;
			this->type = type;
			this->round = round;
		}
	};
#pragma pack(push, 1)

	struct SELF_EFFECT_NET
	{
		BYTE type;
		BYTE round;
		int hp;
		int mp;
		SELF_EFFECT_NET( BYTE type, BYTE round, int hp = 0, int mp = 0 )
		{
			this->type = type;
			this->round = round;
			this->hp = hp;
			this->mp = mp;
		}
	};

	struct EFFECT_NET
	{
		BYTE type;
		BYTE round;
		EFFECT_NET( BYTE type, BYTE round )
		{
			this->type = type;
			this->round = round;
		}
		EFFECT_NET( list<EFFECT_ROUND>::iterator& effRound )
		{
			this->round = effRound->round;
			this->type = effRound->type;
		}
	};

	struct FIGHT_HURT 
	{
		vector<EFFECT_NET> vecEff;//释放技能附加效果
		BYTE  pos;
		DWORD leftHp;
		DWORD leftMp;
		int   hurtValue;
		int   mpValue;
		BYTE  hit;
		BYTE  crit;
		BYTE  luck;
		BYTE  effNum;
		BYTE  bydouble;
		FIGHT_HURT(  BYTE pos, DWORD leftHp, DWORD leftMp, int hurtValue, int mpValue, BYTE  hit, BYTE crit, BYTE luck, vector<EFFECT_NET>& vecEff, BYTE  bydouble = 0 )
		{
			this->pos = pos;
			this->leftHp = leftHp;
			this->leftMp = leftMp;
			this->hurtValue = hurtValue;
			this->mpValue = mpValue;
			this->hit = hit;
			this->crit = crit;
			this->luck = luck;
			this->effNum = vecEff.size();
			this->vecEff.assign( vecEff.begin(), vecEff.end() );
			this->bydouble = bydouble;
		}
	};

	struct FIGHT_SEQ 
	{
		BYTE  pos;
		BYTE  round;
		WORD  skillID;
		DWORD leftHp;
		DWORD leftMp;
		vector<SELF_EFFECT_NET> vecSelfEff;//自身附加效果
		vector<FIGHT_HURT> victimSeq;
		FIGHT_SEQ( BYTE pos, WORD skillID, BYTE  round, DWORD leftHp, DWORD leftMp, vector<SELF_EFFECT_NET>& vecSelfEff, vector<FIGHT_HURT>& victimSeq )
		{
			this->pos = pos;
			this->skillID = skillID;
			this->round = round;
			this->leftHp = leftHp;
			this->leftMp = leftMp;
			this->vecSelfEff.assign(vecSelfEff.begin(), vecSelfEff.end());
			this->victimSeq.assign(victimSeq.begin(), victimSeq.end());
		}
	};

	struct DROP_SEQ 
	{
		WORD itemID;
		WORD itemNum;
		BYTE itemQuality;
		BYTE isSell;
		DWORD price;
		ZERO_CONS(DROP_SEQ);
		DROP_SEQ( WORD itemID, WORD itemNum, BYTE itemQuality, BYTE isSell, DWORD price )
		{
			this->itemID = itemID;
			this->itemNum = itemNum;
			this->itemQuality = itemQuality;
			this->isSell = isSell;
			this->price = price;
		}
	};
#pragma pack(pop)

namespace FightCtrl
{
	int clientEnterMap( ROLE* role, unsigned char * data, size_t dataLen );
	int clientFightRequest( ROLE* role, unsigned char * data, size_t dataLen );
	int clientFightEnd( ROLE* role, unsigned char * data, size_t dataLen );
	void fightWorldBoss(ROLE* role, DWORD roleAttr[E_ATTR_MAX], bool bChance, bool inst_battle, WORD boss_level, WORD skillid, DWORD boss_cur_hp,DWORD& boss_remain_hp);
}
#endif