﻿#include "StdAfx.h"

#include <iterator>

#include "Diablo Edit2.h"
#include "D2S_Struct.h"

using namespace std;

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

//struct CQuestInfoData

CInBitsStream & operator >>(CInBitsStream & bs, CQuestInfoData & v) {
	return bs >> v.wIntroduced1
		>> v.wActI
		>> v.wTraval1
		>> v.wIntroduced2
		>> v.wActII
		>> v.wTraval2
		>> v.wIntroduced3
		>> v.wActIII
		>> v.wTraval3
		>> v.wIntroduced4
		>> v.wActIV
		>> v.wTraval4
		>> v.unknown1
		>> v.wIntroduced5
		>> v.unknown2
		>> v.wActV
		>> v.bResetStats
		>> v.unknown3
		>> v.unknown4;
}

COutBitsStream & operator <<(COutBitsStream & bs, const CQuestInfoData & v) {
	return bs << v.wIntroduced1 << v.wActI << v.wTraval1
		<< v.wIntroduced2 << v.wActII << v.wTraval2
		<< v.wIntroduced3 << v.wActIII << v.wTraval3
		<< v.wIntroduced4 << v.wActIV << v.wTraval4 << v.unknown1
		<< v.wIntroduced5 << v.unknown2 << v.wActV << v.bResetStats 
		<< v.unknown3 << v.unknown4;
}

//struct CQuestInfo

CInBitsStream & operator >>(CInBitsStream & bs, CQuestInfo & v) {
	bs >> v.dwMajic >> v.dwActs >> v.wSize;
	if (v.dwMajic != 0x216F6F57 || v.wSize != 0x12A)
		throw ::theApp.MsgBoxInfo(13);
	return bs >> v.QIData;
}

COutBitsStream & operator <<(COutBitsStream & bs, const CQuestInfo & v) {
	return bs << DWORD(0x216F6F57) << v.dwActs << WORD(0x12A) << v.QIData;
}

//struct CWaypointData

CInBitsStream & operator >>(CInBitsStream & bs, CWaypointData & v) {
	return bs >> v.unkown >> v.Waypoints >> v.unkown2;
}

COutBitsStream & operator <<(COutBitsStream & bs, const CWaypointData & v) {
	return bs << v.unkown << v.Waypoints << v.unkown2;
}

//struct CWaypoints

CInBitsStream & operator >>(CInBitsStream & bs, CWaypoints & v) {
	bs >> v.wMajic >> v.unkown >> v.wSize;
	if (v.wMajic != 0x5357 || v.wSize != 0x50)
		throw ::theApp.MsgBoxInfo(14);
	return bs >> v.wp;
}

COutBitsStream & operator <<(COutBitsStream & bs, const CWaypoints & v) {
	return bs << WORD(0x5357) << v.unkown << WORD(0x50) << v.wp;
}

//CPlayerStats

static const DWORD PLAYER_STATS_BITS_COUNT[CPlayerStats::ARRAY_SIZE] = {
	10,10,10,10,10,8,
	21,21,21,21,21,21,
	7,32,25,25
};

CInBitsStream & operator >>(CInBitsStream & bs, CPlayerStats & v) {
	bs >> v.wMajic;
	if (v.wMajic != 0x6667)
		throw ::theApp.MsgBoxInfo(15);
	::ZeroMemory(v.m_adwValue, sizeof(v.m_adwValue));
	for (bs >> bits(v.iEnd, 9); bs.Good() && v.iEnd < size(v.m_adwValue); bs >> bits(v.iEnd, 9))
		bs >> bits(v.m_adwValue[v.iEnd], PLAYER_STATS_BITS_COUNT[v.iEnd]);
	bs.AlignByte();
	return bs;
}

COutBitsStream & operator <<(COutBitsStream & bs, const CPlayerStats & v) {
	bs << WORD(0x6667);
	for (UINT i = 0; bs.Good() && i < size(v.m_adwValue); ++i)
		if (v.m_adwValue[i])
			bs << bits(WORD(i), 9) << bits(v.m_adwValue[i], PLAYER_STATS_BITS_COUNT[i]);
	bs << bits<WORD>(0x1FF, 9);
	bs.AlignByte();
	return bs;
}

//struct CCharSkills

CInBitsStream & operator >>(CInBitsStream & bs, CCharSkills & v) {
	bs >> v.wMagic;
	if (v.wMagic != 0x6669)
		throw ::theApp.MsgBoxInfo(16);
	return bs >> v.bSkillLevel;
}

COutBitsStream & operator <<(COutBitsStream & bs, const CCharSkills & v) {
	return bs << WORD(0x6669) << v.bSkillLevel;
}

//struct CCorpseData

void CCorpseData::ReadData(CInBitsStream & bs, BOOL isD2R, BOOL isPtr24) {
	bs >> unknown; 
	stItems.ReadData(bs, isD2R, isPtr24);
}

void CCorpseData::WriteData(COutBitsStream & bs, BOOL isD2R, BOOL isPtr24) const {
	bs << unknown;
	stItems.WriteData(bs, isD2R, isPtr24);
}

//struct CCorpse

void CCorpse::ReadData(CInBitsStream & bs, BOOL isD2R, BOOL isPtr24) {
	bs >> wMagic >> wCount;
	if (wMagic != 0x4D4A || wCount > 1)
		throw ::theApp.MsgBoxInfo(19);
	if (wCount)
		pCorpseData.ensure().ReadData(bs, isD2R, isPtr24);
}

void CCorpse::WriteData(COutBitsStream & bs, BOOL isD2R, BOOL isPtr24) const {
	bs << WORD(0x4D4A);
	if (pCorpseData.exist()) {
		bs << WORD(1);
		pCorpseData->WriteData(bs, isD2R, isPtr24);
	} else
		bs << WORD(0);
}

//struct CMercenary

void CMercenary::ReadData(CInBitsStream & bs, BOOL hasMercenary, BOOL isD2R, BOOL isPtr24) {
	bs >> wMagic;
	if (wMagic != 0x666A)
		throw ::theApp.MsgBoxInfo(20);
	if (hasMercenary)
		stItems.ensure().ReadData(bs, isD2R, isPtr24);
}

void CMercenary::WriteData(COutBitsStream & bs, BOOL hasMercenary, BOOL isD2R, BOOL isPtr24) const {
	bs << WORD(0x666A);
	if (hasMercenary)
		stItems->WriteData(bs, isD2R, isPtr24);
}

//struct CGolem

void CGolem::ReadData(CInBitsStream & bs, BOOL isD2R, BOOL isPtr24) {
	bs >> wMagic >> bHasGolem;
	if (wMagic != 0x666B)
		throw ::theApp.MsgBoxInfo(21);
	if (bHasGolem)
		pItem.ensure().ReadData(bs, isD2R, isPtr24);
}

void CGolem::WriteData(COutBitsStream & bs, BOOL isD2R, BOOL isPtr24) const {
	bs << WORD(0x666B) << bHasGolem;
	if (bHasGolem)
		pItem->WriteData(bs, isD2R, isPtr24);
}

//CD2S_Struct

void CD2S_Struct::ReadFile(const CString & path)
{
	CInBitsStream bs;
	bs.ReadFile(CFile(path, CFile::modeRead));
	ReadData(bs);
}

void CD2S_Struct::WriteFile(const CString & path) const {
	COutBitsStream bs;
	if (!WriteData(bs))
		return;
	bs.WriteFile(CFile(path, CFile::modeCreate | CFile::modeWrite));
}

void CD2S_Struct::Reset() {
	QuestInfo.Reset();
	Waypoints.Reset();
	PlayerStats.Reset();
	Skills.Reset();
	ItemList.Reset();
	stCorpse.Reset();
	stMercenary.Reset();
	stGolem.Reset();
}

void CD2S_Struct::ReadData(CInBitsStream & bs) {
	Reset();
	//得到人物信息
	bs >> dwMajic;
	if (dwMajic != 0xAA55AA55)
		throw ::theApp.MsgBoxInfo(11);
	bs >> dwVersion >> dwSize;
	if (bs.DataSize() != dwSize)
		throw ::theApp.MsgBoxInfo(12);
	const DWORD offCrc = bs.BytePos();
	bs >> dwCRC;
	if (!::ValidateCrc(bs.Data(), dwCRC, offCrc))	//校验CRC，会修改bs数据内容
		throw ::theApp.MsgBoxInfo(11);
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
		>> unkown7
		>> NamePTR
		>> unkown8
		>> QuestInfo
		>> Waypoints
		>> NPC
		>> PlayerStats
		>> Skills;
	ItemList.ReadData(bs, isD2R(), isPtr24());
	stCorpse.ReadData(bs, isD2R(), isPtr24());
	if (isExpansion()) {
		stMercenary.ReadData(bs, HasMercenary(), isD2R(), isPtr24());
		stGolem.ReadData(bs, isD2R(), isPtr24());
	}
	bs.AlignByte();
	if (!bs.Good() || bs.DataSize() != bs.BytePos())
		throw ::theApp.MsgBoxInfo(11);
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
		<< unkown7
		<< NamePTR
		<< unkown8
		<< QuestInfo
		<< Waypoints
		<< NPC
		<< PlayerStats
		<< Skills;
	ItemList.WriteData(bs, isD2R(), isPtr24());
	stCorpse.WriteData(bs, isD2R(), isPtr24());
	if (isExpansion()) {
		stMercenary.WriteData(bs, HasMercenary(), isD2R(), isPtr24());
		stGolem.WriteData(bs, isD2R(), isPtr24());
	}
	bs.AlignByte();
	//Data size
	bs << offset_value(offSize, bs.BytePos());
	//CRC
	auto & data = bs.Data();
	const DWORD dwCrc = ::ComputCRC(&data[0], data.size(), 0);
	bs << offset_value(offCrc, dwCrc);
	return bs.Good();
}

void CD2S_Struct::name(const CString & name) {
	CStringA utf8 = EncodeCharName(name);
	// Copy data
	BYTE * dest = isPtr24() ? NamePTR : Name;
	int destLen = isPtr24() ? sizeof NamePTR : sizeof Name;
	::ZeroMemory(dest, destLen);
	::CopyMemory(dest, utf8, min(utf8.GetLength(), destLen));
}
