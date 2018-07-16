#pragma once

#include "BinDataStream.h"

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
	void ReadData(CInBitsStream & bs){
		bs >> wIntroduced1 >> wActI >> wTraval1
			>> wIntroduced2 >> wActII >> wTraval2
			>> wIntroduced3 >> wActIII >> wTraval3
			>> wIntroduced4 >> wActIV >> wTraval4 >> unkown1
			>> wIntroduced5 >> unkown2 >> wActV >> unkown3;
	}
	void WriteData(COutBitsStream & bs) const {
		bs << wIntroduced1 << wActI << wTraval1
			<< wIntroduced2 << wActII << wTraval2
			<< wIntroduced3 << wActIII << wTraval3
			<< wIntroduced4 << wActIV << wTraval4 << unkown1
			<< wIntroduced5 << unkown2 << wActV << unkown3;
	}
};

struct CQuestInfo
{
	DWORD	dwMajic;			//0x216F6F57
	DWORD	dwActs;				//似乎总是6
	WORD	wSize;				//Quest Info结构的总长度，0x12A(=298=4+4+2+288)
	CQuestInfoData	QIData[3];
	void ReadData(CInBitsStream & bs){
		bs>>dwMajic>>dwActs>>wSize;
		if (dwMajic != 0x216F6F57 || wSize != 0x12A) {
			MessageBox(0, ::theApp.MsgBoxInfo(13), ::theApp.MsgError(), MB_ICONERROR);
			throw 0;
		}
		for (auto & q : QIData)
			q.ReadData(bs);
	}
	void WriteData(COutBitsStream & bs) const {
		bs << DWORD(0x216F6F57) << dwActs << WORD(0x12A);
		for (auto & q : QIData)
			q.WriteData(bs);
	}
};

// 小站信息
struct CWaypointData
{
	WORD	unkown;			//0x102;
	BYTE	Waypoints[5];
	BYTE	unkown2[17];		//全0
	void ReadData(CInBitsStream & bs) {
		bs >> unkown >> Waypoints >> unkown2;
	}
	void WriteData(COutBitsStream & bs) const {
		bs << unkown << Waypoints << unkown2;
	}
};

struct CWaypoints
{
	WORD	wMajic;				//0x5357
	DWORD	unkown;
	WORD	wSize;				//0x50
	CWaypointData	wp[3];
	void ReadData(CInBitsStream & bs){
		bs>>wMajic>>unkown>>wSize;
		if (wMajic != 0x5357 || wSize != 0x50) {
			MessageBox(0, ::theApp.MsgBoxInfo(14), ::theApp.MsgError(), MB_ICONERROR);
			throw 0;
		}
		for (auto & w : wp)
			w.ReadData(bs);
	}
	void WriteData(COutBitsStream & bs) const {
		bs << WORD(0x5357) << unkown << WORD(0x50);
		for (auto & w : wp)
			w.WriteData(bs);
	}
};

//人物技能
class CCharSkills
{
//members
public:
	CCharSkills(){}
	~CCharSkills(){}
	void ReadData(CInBitsStream & bs){
		bs>>wMagic;
		if (wMagic != 0x6669) {
			MessageBox(0, ::theApp.MsgBoxInfo(16), ::theApp.MsgError(), MB_ICONERROR);
			throw 0;
		}
		bs>>bSkillLevel;
	}
	void WriteData(COutBitsStream & bs) const {
		bs << WORD(0x6669) << bSkillLevel;
	}
//fields
public:
	//人物信息
	WORD	wMagic;				//0x6669
	BYTE	bSkillLevel[30];	//技能等级
};

//人物属性
struct CPlayerStats
{
//members
	CPlayerStats(void){}
	~CPlayerStats(void){}
	void ReadData(CInBitsStream & bs);
	void WriteData(COutBitsStream & bs) const;
private:
//fields
	static const DWORD	ARRAY_SIZE = 0x10;
	static const DWORD	BITS_COUNT[ARRAY_SIZE];
public:
//人物属性
	WORD	wMajic;					//0x6667
	DWORD	m_dwValue[ARRAY_SIZE];	/*以下为各个Value的含义：
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
										F: 9 bits, + 25 bits Gold in Stash
									0x1FF: 9 bits, 结束
									//*/
};



