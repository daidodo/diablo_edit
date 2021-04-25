#include "StdAfx.h"

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

CInBitsStream & operator >>(CInBitsStream & bs, CCorpseData & v) {
	return bs >> v.unknown >> v.stItems;
}

COutBitsStream & operator <<(COutBitsStream & bs, const CCorpseData & v) {
	return bs << v.unknown << v.stItems;
}

//struct CCorpse

CInBitsStream & operator >>(CInBitsStream & bs, CCorpse & v) {
	bs >> v.wMagic >> v.wCount;
	if (v.wMagic != 0x4D4A || v.wCount > 1)
		throw ::theApp.MsgBoxInfo(19);
	if (v.wCount)
		bs >> v.pCorpseData;
	return bs;
}

COutBitsStream & operator <<(COutBitsStream & bs, const CCorpse & v) {
	bs << WORD(0x4D4A);
	if (v.pCorpseData.exist())
		bs << WORD(1) << v.pCorpseData;
	else
		bs << WORD(0);
	return bs;
}

//struct CMercenary

CInBitsStream & operator >>(CInBitsStream & bs, pair<CMercenary &, bool> & p) {
	auto & v = p.first;
	bs >> v.wMagic;
	if (v.wMagic != 0x666A)
		throw ::theApp.MsgBoxInfo(20);
	if(p.second)
		bs >> v.stItems;
	return bs;
}

COutBitsStream & operator <<(COutBitsStream & bs, const pair<const CMercenary &, bool> & p) {
	auto & v = p.first;
	bs << WORD(0x666A);
	if (p.second)
		bs << v.stItems;
	return bs;
}

//struct CGolem

CInBitsStream & operator >>(CInBitsStream & bs, CGolem & v) {
	bs >> v.wMagic >> v.bHasGolem;
	if (v.wMagic != 0x666B)
		throw ::theApp.MsgBoxInfo(21);
	if (v.bHasGolem)
		v.pItem.ensure().ReadData(bs);
	return bs;
}

COutBitsStream & operator <<(COutBitsStream & bs, const CGolem & v) {
	bs << WORD(0x666B) << v.bHasGolem;
	if (v.bHasGolem)
		v.pItem->WriteData(bs);
	return bs;
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
		>> QuestInfo
		>> Waypoints
		>> NPC
		>> PlayerStats
		>> Skills
		>> ItemList
		>> stCorpse;
	if (isExpansion())
		bs >> pair<CMercenary &, bool>(stMercenary, HasMercenary())
			>> stGolem;
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
		<< QuestInfo
		<< Waypoints
		<< NPC
		<< PlayerStats
		<< Skills
		<< ItemList
		<< stCorpse;
	bs << pair<const CMercenary &, bool>(stMercenary, HasMercenary())
		<< stGolem;
	bs.AlignByte();
	//Data size
	bs << offset_value(offSize, bs.BytePos());
	//CRC
	auto & data = bs.Data();
	const DWORD dwCrc = ::ComputCRC(&data[0], data.size(), 0);
	bs << offset_value(offCrc, dwCrc);
	return bs.Good();
}