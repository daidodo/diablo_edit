#include "StdAfx.h"
#include "Diablo Edit2.h"
#include "D2S_Struct.h"

//d2s文件的CRC算法
static DWORD ComputCRC(const BYTE * source, DWORD len, DWORD init) {
	ASSERT(source && len);
	for (UINT i = 0, add; i < len; ++i, init = (init << 1) + add)
		add = (init & 0x80000000 ? 1 : 0) + source[i];
	return init;
}

//检验数据的CRC，会修改数据内容
static BOOL ValidateCrc(std::vector<BYTE> & data, DWORD dwCrc, DWORD dwOff) {
	ASSERT(dwOff + 4 <= data.size());
	*reinterpret_cast<DWORD *>(&data[dwOff]) = 0;
	return (dwCrc == ::ComputCRC(&data[0], data.size(), 0));
}

//CD2S_Struct

BOOL CD2S_Struct::ReadFile(CFile & cf)
{
	try{
		CInBitsStream bs;
		bs.ReadFile(cf);
		return ReadData(bs);
	}catch(...){}
	return FALSE;
}

void CD2S_Struct::WriteFile(CFile & cf) const
{
	COutBitsStream	bs;
	if (WriteData(bs)) 
		bs.WriteFile(cf);
}

BOOL CD2S_Struct::ReadData(CInBitsStream & bs) {
	//得到人物信息
	bs >> dwMajic;
	if (dwMajic != 0xAA55AA55)
		if (MessageBox(0, ::theApp.String(13), ::theApp.String(5), MB_YESNO | MB_ICONWARNING) == IDNO)
			return FALSE;
	bs >> dwVersion >> dwSize;
	if (bs.DataSize() != dwSize)
		if (MessageBox(0, ::theApp.String(14), ::theApp.String(5), MB_YESNO | MB_ICONWARNING) == IDNO)
			return FALSE;
	const DWORD offCrc = bs.BytePos();
	bs >> dwCRC;
	if(!::ValidateCrc(bs.Data(), dwCRC, offCrc))	//校验CRC，会修改bs数据内容
		if (MessageBox(0, ::theApp.String(13), ::theApp.String(5), MB_YESNO | MB_ICONWARNING) == IDNO)
			return FALSE;
	bs >> dwWeaponSet
		>> Name
		>> charType
		>> charTitle
		>> unkown1
		>> charClass
		>> unkown2
		>> charLevel
		>> unkown3
		>> dwTime
		>> unkown4
		>> dwSkillKey
		>> dwLeftSkill1
		>> dwRightSkill1
		>> dwLeftSkill2
		>> dwRightSkill2
		>> outfit
		>> colors
		>> Town
		>> dwMapSeed
		>> unkown5
		>> bMercDead
		>> unkown6
		>> dwMercControl
		>> wMercName
		>> wMercType
		>> dwMercExp
		>> unkown7;
	QuestInfo.ReadData(bs);
	Waypoints.ReadData(bs);
	bs >> NPC;
	PlayerStats.ReadData(bs);
	Skills.ReadData(bs);
	ItemList.ReadData(bs);
	bs >> wCorpses >> vLeftData;
	return bs.Good();
}

BOOL CD2S_Struct::WriteData(COutBitsStream & bs) const {
	// Character data
	bs << DWORD(0xAA55AA55)	//dwMajic
		<< dwVersion;
	const DWORD offSize = bs.BytePos();
	bs << DWORD(0);			//data size
	const DWORD offCrc = bs.BytePos();
	bs << DWORD(0)			//CRC
		<< dwWeaponSet
		<< Name
		<< charType
		<< charTitle
		<< unkown1
		<< charClass
		<< unkown2
		<< charLevel
		<< unkown3
		<< dwTime
		<< unkown4
		<< dwSkillKey
		<< dwLeftSkill1
		<< dwRightSkill1
		<< dwLeftSkill2
		<< dwRightSkill2
		<< outfit
		<< colors
		<< Town
		<< dwMapSeed
		<< unkown5
		<< bMercDead
		<< unkown6
		<< dwMercControl
		<< wMercName
		<< wMercType
		<< dwMercExp
		<< unkown7;
	QuestInfo.WriteData(bs);
	Waypoints.WriteData(bs);
	bs << NPC;
	PlayerStats.WriteData(bs);
	Skills.WriteData(bs);
	ItemList.WriteData(bs);
	bs << wCorpses << vLeftData;
	bs.AlignByte();
	//Data size
	bs << offset_value(offSize, bs.BytePos());
	//CRC
	auto & data = bs.Data();
	const DWORD dwCrc = ::ComputCRC(&data[0], data.size(), 0);
	bs << offset_value(offCrc, dwCrc);
	return bs.Good();
}