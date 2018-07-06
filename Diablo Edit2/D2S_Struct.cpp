#include "StdAfx.h"
#include "Diablo Edit2.h"
#include "D2S_Struct.h"

//CPlayerStats
const DWORD	CPlayerStats::BITS_COUNT[CPlayerStats::ARRAY_SIZE] = {
	10,10,10,10,10,8,
	21,21,21,21,21,21,
	7,32,25,25
};

//CD2S_Struct
CD2S_Struct::CD2S_Struct(){}

CD2S_Struct::~CD2S_Struct(){}

BOOL CD2S_Struct::ReadFile(CFile & cf)
{
	try{
		CBinDataStream	bs;		//人物的所有数据
		bs.ReadFile(cf);
		//得到人物信息
		bs>>dwMajic;
		if(dwMajic != 0xAA55AA55)
			if(MessageBox(0,::theApp.String(13),::theApp.String(5),MB_YESNO | MB_ICONWARNING) == IDNO)
				return FALSE;
		bs>>dwVersion>>dwSize;
		if(cf.GetLength() != dwSize)
			if(MessageBox(0,::theApp.String(14),::theApp.String(5),MB_YESNO | MB_ICONWARNING) == IDNO)
				return FALSE;
		bs>>dwCRC>>dwWeaponSet>>Name>>charType>>charTitle>>unkown1>>charClass>>unkown2>>charLevel>>unkown3>>dwTime
			>>unkown4>>dwSkillKey>>dwLeftSkill1>>dwRightSkill1>>dwLeftSkill2>>dwRightSkill2>>outfit>>colors>>Town>>dwMapSeed
			>>unkown5>>bMercDead>>unkown6>>dwMercControl>>wMercName>>wMercType>>dwMercExp>>unkown7;
		QuestInfo.ReadData(bs);
		Waypoints.ReadData(bs);
		bs>>NPC;
		PlayerStats.ReadData(bs);
		Skills.ReadData(bs);
		ItemList.ReadData(bs);
		bs>>wCorpses>>vLeftData;
	}catch(...){
		return FALSE;
	}
	return TRUE;
}
void CD2S_Struct::WriteFile(CFile & cf)
{}
/*
void CD2S_Struct::WriteFile(CFile & cf)
{
	PlayerStats.MakeData();
//	ItemList.MakeData();
	//人物数据的实际长度
	*dwSize = DATA_LEN
		+ PlayerStats.Size()
		+ Skills.Size()
	//	+ ItemList.Size()

		+ DWORD(LeftData.Size());
	GetChecksum();
	//以下语句不能对人物数据进行改写，否则CRC要重算
	cf.Write(bData,DATA_LEN);
	cf.Flush();
	PlayerStats.WriteFile(cf);
	Skills.WriteFile(cf);
//	ItemList.WriteFile(cf);

	LeftData.WriteFile(cf);
}



void CD2S_Struct::Map(CQuestInfo & qi,UINT & pos)
{
	Map(qi.dwMajic,pos);
	pos += 4;	//dwActs
	Map(qi.wSize,pos);
	*qi.wSize = 0x12A;
	for(int i = 0;i < 3;++i){
		pos += 2;	//wIntroduced1
		Map(qi.QIData[i].wActI,pos);
		Map(qi.QIData[i].wTraval1,pos);
		pos += 2;	//wIntroduced2
		Map(qi.QIData[i].wActII,pos);
		Map(qi.QIData[i].wTraval2,pos);
		pos += 2;	//wTraval2,wIntroduced3
		Map(qi.QIData[i].wActIII,pos);
		Map(qi.QIData[i].wTraval3,pos);
		pos += 2;	//wTraval3,wIntroduced4
		Map(qi.QIData[i].wActIV,pos);
		Map(qi.QIData[i].wTraval4,pos);
		pos += 12;	//wTraval4,unkown1,wIntroduced5
		Map(qi.QIData[i].wActV,pos);
		pos += 14;	//unkown3
	}
}

void CD2S_Struct::Map(CWaypoints & wp,UINT & pos){
	Map(wp.wMajic,pos);
	*wp.wMajic = 0x5357;
//	Map(wp.unkown,pos);
//	*wp.unkown = 1;
	pos += 4;
	Map(wp.wSize,pos);
	*wp.wSize = 0x50;
	for(int i = 0;i < 3;++i){
	//	Map(wp.wp[i].unkown1,pos);
		pos += 2;
		Map(wp.wp[i].Waypoints,pos);
		Map(wp.wp[i].unkown2,pos);
	}
}

void CD2S_Struct::GetChecksum()
{
	//暗黑d2s文件的校验和算法
	//注意：文件校验和*dwCRC（data_[0xC]~data_[0xF]）应先修改成0
	//the checksum algorithm is a relatively simple one.
	//After clearing the checksum field, you add up the values of all the bytes in the file,
	//rotating the running total one bit to the left before adding each byte.
	//Then store the result in this field.
	//Thanks go to the Shadowmaster for publishing the checksum code.
	*dwCRC = 0;
	DWORD crc = ComputCRC(bData,DATA_LEN,0);
	crc = ComputCRC(PlayerStats.GetData(),DWORD(PlayerStats.Size()),crc);
	crc = ComputCRC(Skills.GetData(),DWORD(Skills.Size()),crc);
//	crc = ComputCRC(ItemList.GetData(),DWORD(ItemList.Size()),crc);
	

	if(LeftData.bData.size())
		crc = ComputCRC(LeftData.GetData(),DWORD(LeftData.Size()),crc);
	*dwCRC = crc;
}

DWORD CD2S_Struct::ComputCRC(const BYTE * source,DWORD len,DWORD init)
{
	ASSERT(source && len && "CD2S_Struct::ComputCRC()");
	for(UINT i = 0,add;i < len;++i,init = (init << 1) + add)
		add = (init & 0x80000000 ? 1 : 0) + source[i];
	return init;
}
//*/