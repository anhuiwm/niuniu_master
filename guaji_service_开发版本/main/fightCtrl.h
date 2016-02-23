#ifndef FIGHTCTRL_H
#define FIGHTCTRL_H


enum eFIGHTTYPE
{
	E_FIGHT_NONE,//
	E_FIGHT_MON = 1,		   //打怪
	E_FIGHT_BOSS = 2,		   //BOSS战
	E_FIGHT_ARENA_PLAYER = 3,		   //竞技场PK
	E_FIGHT_HERO = 4,		   //精英BOSS战
	E_FIGHT_MEET_PLAYER = 5,		   //偶遇PK
	E_FIGHT_BODYGUARD = 6,		   // 镖局系统 偶遇镖师
};

struct TFightDrop
{
	list<uint> lstDropMonster;
	byte byDropType;//最近需要掉落类型
	uint wDropMap;//最近需要掉落的地图
	uint wBossID;//boss
	void resetDrop();
	void setDrop(list<uint16>& lstMons, const byte dropType, const uint16 map, const uint16 boss = 0);

	TFightDrop()
	{
		resetDrop();
	}
};


struct EFFECT_ROUND
{
	uint16 level;
	byte type;
	byte round;

	EFFECT_ROUND(const EFFECT_ROUND& dst)
	{
		this->level = dst.level;
		this->type = dst.type;
		this->round = dst.round;
	}

	EFFECT_ROUND& operator = (const EFFECT_ROUND& dst)
	{
		if (this == &dst)
		{
			return *this;
		}

		this->level = dst.level;
		this->type = dst.type;
		this->round = dst.round;

		return *this;
	}

	EFFECT_ROUND(uint16 level, byte type, byte round)
	{
		this->level = level;
		this->type = type;
		this->round = round;
	}
};
#pragma pack(push, 1)

struct SELF_EFFECT_NET
{
	byte type;
	byte round;
	int hp;
	int mp;
	SELF_EFFECT_NET(byte type, byte round, int hp = 0, int mp = 0)
	{
		this->type = type;
		this->round = round;
		this->hp = hp;
		this->mp = mp;
	}
};

struct EFFECT_NET
{
	byte type;
	byte round;
	EFFECT_NET(byte type, byte round)
	{
		this->type = type;
		this->round = round;
	}
	EFFECT_NET(list<EFFECT_ROUND>::iterator& effRound)
	{
		this->round = effRound->round;
		this->type = effRound->type;
	}
};

struct FIGHT_HURT
{
	vector<EFFECT_NET> vecEff;//释放技能附加效果
	byte  pos;
	uint leftHp;
	uint leftMp;
	int   hurtValue;
	int   mpValue;
	byte  hit;
	byte  crit;
	byte  luck;
	byte  effNum;
	byte  bydouble;

	FIGHT_HURT(byte pos, uint leftHp, uint leftMp, int hurtValue, int mpValue, byte  hit, byte crit, byte luck, vector<EFFECT_NET>& vecEff, byte  bydouble = 0)
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
		this->vecEff.assign(vecEff.begin(), vecEff.end());
		this->bydouble = bydouble;
	}
};

struct FIGHT_SEQ
{
	byte  pos;
	byte  round;
	uint16  skillID;
	uint leftHp;
	uint leftMp;
	vector<SELF_EFFECT_NET> vecSelfEff;//自身附加效果
	vector<FIGHT_HURT> victimSeq;
	FIGHT_SEQ(byte pos, uint16 skillID, byte  round, uint leftHp, uint leftMp, vector<SELF_EFFECT_NET>& vecSelfEff, vector<FIGHT_HURT>& victimSeq)
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
	uint16 itemID;
	uint16 itemNum;
	byte itemQuality;
	byte isSell;
	DWORD price;
	ZERO_CONS(DROP_SEQ);
	DROP_SEQ(uint16 itemID, uint16 itemNum, byte itemQuality, byte isSell, uint16 price)
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
	byte clientEnterMap(ROLE* role, const uint16& wMapIndex);
	int clientFightRequest( ROLE* role, input_stream& stream);
	int clientFightEnd(ROLE* role, unsigned char * data, size_t dataLen);
	//byte clientReportMap(ROLE* role, vector<DROP_SEQ>& dropSeqs, const uint16& mapID, const byte& dropType, uint* pvAdd);// 作废
	void fightWorldBoss(ROLE* role, uint roleAttr[E_ATTR_MAX], bool bChance, bool inst_battle, uint16 boss_level, uint16 skillid, uint boss_cur_hp, uint& boss_remain_hp);
}
#endif