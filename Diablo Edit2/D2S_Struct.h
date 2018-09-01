#pragma once

#include "D2Item.h"

//任务完成信息
struct CQuestInfoData
{
	WORD	wIntroduced1;		//have been introduced (by Warriv) to Act I
	WORD	wActI[6];			//Act I, Bit 0 indicates the quest is complete,下面也一样
	WORD	wTraval1;			//this gets set to a non-zero value after you travel from Act I to Act II.
	WORD	wIntroduced2;		//have been introduced (by Jerhyn) to Act II
	WORD	wActII[6];			//Act II
	WORD	wTraval2;			//this gets set to a non-zero value after you travel from Act II to Act III.
	WORD	wIntroduced3;		//have been introduced (by Hratli) to Act III
	WORD	wActIII[6];			//Act III
	WORD	wTraval3;			//this gets set to a non-zero value after you travel from Act III to Act IV.
	WORD	wIntroduced4;		//have been introduced to Act IV
	WORD	wActIV[3];			//Act IV
	WORD	wTraval4;
	WORD	unkown1[3];
	WORD	wIntroduced5;		//this was set to 1 after completing Terror's End and talking to Cain in act IV
	WORD	unkown2[2];
	WORD	wActV[6];			//Act V
	WORD	unkown3[7];
};

struct CQuestInfo
{
	DWORD	dwMajic;			//0x216F6F57
	DWORD	dwActs;				//似乎总是6
	WORD	wSize;				//Quest Info结构的总长度，0x12A(=298=4+4+2+288)
	CQuestInfoData	QIData[3];
	//Functions:
	void Reset() {}
};

// 小站信息
struct CWaypointData
{
	WORD	unkown;				//0x102;
	BYTE	Waypoints[5];
	BYTE	unkown2[17];		//全0
};

struct CWaypoints
{
	WORD	wMajic;				//0x5357
	DWORD	unkown;
	WORD	wSize;				//0x50
	CWaypointData	wp[3];
	//Functions:
	void Reset() {}
};

//人物属性
struct CPlayerStats
{
	static const int ARRAY_SIZE = 0x10;
	WORD	wMajic;					//0x6667
	DWORD	m_adwValue[ARRAY_SIZE];	/*以下为各个Value的含义：
										0: 9 bits, + 10 bits Strength
										1: 9 bits, + 10 bits Energy
										2: 9 bits, + 10 bits Dexterity
										3: 9 bits, + 10 bits Vitality
										4: 9 bits, + 10 bits Stat Points Remaining
										5: 9 bits, + 8 bits Skill Choices Remaining
										6: 9 bits, + 21 bits Current Life (/256)
										7: 9 bits, + 21 bits Maximum Life (/256)
										8: 9 bits, + 21 bits Current Mana (/256)
										9: 9 bits, + 21 bits Maximum Mana (/256)
										A: 9 bits, + 21 bits Current Stamina (/256)
										B: 9 bits, + 21 bits Maximum Stamina (/256)
										C: 9 bits, + 7 bits Level
										D: 9 bits, + 32 bits Experience
										E: 9 bits, + 25 bits Gold on Person
										F: 9 bits, + 25 bits Gold in Stash */
	WORD iEnd;						//0x1FF: 9 bits, 结束
	//Functions:
	void Reset() {}
};

//人物技能
struct CCharSkills
{
	WORD	wMagic;				//0x6669
	BYTE	bSkillLevel[30];	//技能等级
	//Functions:
	void Reset() {}
};

//尸体数据
struct CCorpseData
{
	BYTE		unknown[12];
	CItemList	stItems;	//尸体身上的装备列表
};

//尸体
struct CCorpse
{
	WORD		wMagic;					//0x4D4A
	WORD		wCount;					//尸体个数, always 0 or 1
	MayExist<CCorpseData> pCorpseData;	//尸体的数据, if wCount == 1
	//Function:
	BOOL HasCorpse() const { return wCount > 0 && pCorpseData.exist(); }
	void Reset() { pCorpseData.reset(); }
};

//雇佣兵
struct CMercenary
{
	WORD				wMagic;		//0x666A, "jf"
	MayExist<CItemList>	stItems;	//雇佣兵的装备列表, if wMercName != 0
	//Function:
	void Reset() { stItems.reset(); }
};

//钢铁石魔
struct CGolem
{
	WORD	wMagic;				//0x666B, "kf"
	BYTE	bHasGolem;
	MayExist<CD2Item>	pItem;	//召唤钢铁石魔的物品, if bHasGolem != 0
	//Function:
	void Reset() { bHasGolem = FALSE; pItem.reset(); }
};

struct CD2S_Struct
{
//members
	void ReadData(CInBitsStream & bs);
	void ReadFile(const CString & path);
	void WriteFile(const CString & path) const;
	BOOL HasCorpse() const { return stCorpse.HasCorpse(); }
	BOOL HasMercenary() const { return wMercName > 0; }
	void Reset();
private:
	BOOL WriteData(COutBitsStream & bs) const;
public:
	//人物信息
	DWORD	dwMajic;			//0xAA55AA55
	DWORD	dwVersion;			//文件版本
								//对于1.10是0x60
								//对于1.09是0x5C
	DWORD	dwSize;				//文件大小bytes
	DWORD	dwCRC;				//数据校验
	DWORD	dwWeaponSet;		//0是否换手?
	BYTE	Name[16];			//人物名字,占用16字符
	BYTE	charType;			//人物类型,Ladder ,Expansion ,Hardcore等
	BYTE	charTitle;			//人物称谓
								//0:	无
								//5:	Slayer
								//0xA:	Champion
								//0xF:	Matriarch或Patriarch
	WORD	unkown1;			//0
	BYTE	charClass;			//人物角色
								//亚马逊Ama:			0x00
								//法师Sorceress:		0x01
								//死灵法师Necromancer:	0x02
								//圣骑士Paladin:		0x03
								//野蛮人Barbarian:		0x04
								//德努伊Druid:			0x05
								//刺客Assassin:			0x06
	WORD	unkown2;			//0x101E
	BYTE	charLevel;			//人物等级
	DWORD	unkown3;			//0
	DWORD	dwTime;				//时间戳
	DWORD	unkown4;			//0xFFFFFFFF
	DWORD	dwSkillKey[16];		//技能快捷键,长度16
	DWORD	dwLeftSkill1;		//换手前左边技能
	DWORD	dwRightSkill1;		//换手前右边技能
	DWORD	dwLeftSkill2;		//换手后左边技能
	DWORD	dwRightSkill2;		//换手后右边技能
	BYTE	outfit[16];			//长度16
	BYTE	colors[16];			//长度16
	BYTE	Town[3];			//最后停留的城镇
	DWORD	dwMapSeed;			//地图随机数种子
	WORD	unkown5;			//0
	BYTE	bMercDead;			//雇佣兵死亡标志
	BYTE	unkown6;			//0
	DWORD	dwMercControl;		//雇佣兵Control seed
	WORD	wMercName;			//雇佣兵名字索引
	WORD	wMercType;			//雇佣兵类型
	DWORD	dwMercExp;			//雇佣兵经验
	BYTE	unkown7[0x90];		//全0,长度0x90

	CQuestInfo		QuestInfo;		//任务完成信息,总长0x12A
	CWaypoints		Waypoints;		//小站信息
	BYTE			NPC[0x34];;		//与NPC的交谈信息
	CPlayerStats	PlayerStats;	//人物状态信息
	CCharSkills		Skills;			//人物技能信息
	CItemList		ItemList;		//物品列表
	CCorpse			stCorpse;		//尸体
	CMercenary		stMercenary;	//雇佣兵
	CGolem			stGolem;		//Golem
};
