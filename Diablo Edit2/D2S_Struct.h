#pragma once

#include "DataStructs.h"
#include "ItemList.h"

struct CD2S_Struct
{
//members
	BOOL ReadFile(CFile & cf);
	void WriteFile(CFile & cf) const;
private:
	BOOL ReadData(CInBitsStream & bs);
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
	WORD			wCorpses;		//尸体数目

	std::vector<BYTE>	vLeftData;		//剩下的数据
};
