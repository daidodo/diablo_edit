#include "StdAfx.h"
#include "Diablo Edit2.h"
#include "D2Item.h"

#include <fstream>
#include <iterator>
#include <cstring>
#include <deque>

using namespace std;

// struct CEar

CInBitsStream & operator >>(CInBitsStream & bs, CEar & v) {
	bs >> bits(v.iEarClass, 3) >> bits(v.iEarLevel, 7);
	for (auto & b : v.sEarName) {
		bs >> bits(b, 7);
		if (!bs.Good() || b == 0)
			break;
	}
	return bs;
}

COutBitsStream & operator <<(COutBitsStream & bs, const CEar & v) {
	bs << bits(v.iEarClass, 3) << bits(v.iEarLevel, 7);
	for (auto b : v.sEarName) {
		bs << bits(b, 7);
		if (!bs.Good() || b == 0)
			break;
	}
	return bs;
}

// struct CLongName

CInBitsStream & operator >>(CInBitsStream & bs, CLongName & v) {
	bs >> bits(v.iName1, 8)
		>> bits(v.iName2, 8)
		>> v.bPref1;
	if (v.bPref1)
		bs >> bits(v.wPref1, 11);
	bs >> v.bSuff1;
	if (v.bSuff1)
		bs >> bits(v.wSuff1, 11);
	bs >> v.bPref2;
	if (v.bPref2)
		bs >> bits(v.wPref2, 11);
	bs >> v.bSuff2;
	if (v.bSuff2)
		bs >> bits(v.wSuff2, 11);
	bs >> v.bPref3;
	if (v.bPref3)
		bs >> bits(v.wPref3, 11);
	bs >> v.bSuff3;
	if (v.bSuff3)
		bs >> bits(v.wSuff3, 11);
	return bs;
}

COutBitsStream & operator <<(COutBitsStream & bs, const CLongName & v) {
	bs << bits(v.iName1, 8)
		<< bits(v.iName2, 8)
		<< v.bPref1;
	if (v.bPref1)
		bs << bits(v.wPref1, 11);
	bs << v.bSuff1;
	if (v.bSuff1)
		bs << bits(v.wSuff1, 11);
	bs << v.bPref2;
	if (v.bPref2)
		bs << bits(v.wPref2, 11);
	bs << v.bSuff2;
	if (v.bSuff2)
		bs << bits(v.wSuff2, 11);
	bs << v.bPref3;
	if (v.bPref3)
		bs << bits(v.wPref3, 11);
	bs << v.bSuff3;
	if (v.bSuff3)
		bs << bits(v.wSuff3, 11);
	return bs;
}

// struct CGoldQuantity
CInBitsStream & operator >>(CInBitsStream & bs, CGoldQuantity & v) {
	return bs >> v.bNotGold >> bits(v.wQuantity, 12);
}

COutBitsStream & operator <<(COutBitsStream & bs, const CGoldQuantity & v) {
	return bs << v.bNotGold << bits(v.wQuantity, 12);
}

//struct CPropertyList

CInBitsStream & operator >>(CInBitsStream & bs, CPropertyList & v) {
	for (bs >> bits(v.iEndFlag, 9); bs.Good() && v.iEndFlag < 0x1FF; bs >> bits(v.iEndFlag, 9))
		bs >> bits(v.mProperty[v.iEndFlag], ::theApp.PropertyMetaData(v.iEndFlag).Bits());
	return bs;
}

COutBitsStream & operator <<(COutBitsStream & bs, const CPropertyList & v) {
	for (auto & p : v.mProperty)
		bs << bits(p.first, 9) << bits(p.second, ::theApp.PropertyMetaData(p.first).Bits());
	return bs << bits<WORD>(0x1FF, 9);
}

int CPropertyList::ExtSockets() const {
	auto wh = mProperty.find(194);	//194是额外孔属性ID
	return (wh == mProperty.end() ? 0 : wh->second);
}

//pack

template<class V, class T>
static pair<V &, const T &> pack(V & v, const T & t) {
	return pair<V &, const T &>(v, t);
}

// struct CExtItemInfo

template<class T>
CInBitsStream & operator >>(CInBitsStream & bs, pair<CExtItemInfo &, const T &> & p) {
	auto & v = p.first;
	auto & t = p.second;
	bs >> bits(v.nGems, 3)
		>> bits(v.dwGUID, 32)
		>> bits(v.iDropLevel, 7)
		>> bits(v.iQuality, 4)
		>> v.bVarGfx;
	if (v.bVarGfx)
		bs >> bits(v.iVarGfx, 3);
	bs >> v.bClass;
	if (v.bClass)
		bs >> bits(v.wClass, 11);
	switch (v.iQuality) {
		case 1:			//low quality
			bs >> bits(v.loQual, 3);
			break;
		case 2:			//normal
			if (get<0>(t))	//bIsCharm
				bs >> bits(v.wCharm, 12);
			break;
		case 3:			//high quality
			bs >> bits(v.hiQual, 3);
			break;
		case 4:			//magically enhanced
			bs >> bits(v.wPrefix, 11) >> bits(v.wSuffix, 11);
			break;
		case 5:			//part of a set
			bs >> bits(v.wSetID, 12);
			break;
		case 6:			//rare
			bs >> v.pRareName;
			break;
		case 7:			//unique
			bs >> bits(v.wUniID, 12);
			break;
		case 8:			//crafted
			bs >> v.pCraftName;
			break;
		default:
			::MessageBox(0, CSFormat(::theApp.MsgBoxInfo(7), UINT(v.iQuality)), 0, MB_OK);
			throw 0;
	}
	if (get<1>(t))	//bRuneWord
		bs >> bits(v.wRune, 16);
	if (get<2>(t))	//bPersonalized
		for (auto & c : v.sPersonName.ensure()) {
			bs >> c;
			if (!bs.Good() || !c)
				break;
		}
	if (get<3>(t))	//bIsTome
		bs >> bits(v.iTome, 5);
	else if (get<4>(t))	//bHasMonsterID
		bs >> bits(v.wMonsterID, 10);
	else if (get<5>(t))	//bHasSpellID
		bs >> bits(v.bSpellID, 5);
	return bs;
}

template<class T>
COutBitsStream & operator <<(COutBitsStream & bs, pair<const CExtItemInfo &, const T &> & p) {
	auto & v = p.first;
	auto & t = p.second;
	bs << bits(v.nGems, 3)
		<< bits(v.dwGUID, 32)
		<< bits(v.iDropLevel, 7)
		<< bits(v.iQuality, 4)
		<< v.bVarGfx;
	if (v.bVarGfx)
		bs << bits(v.iVarGfx, 3);
	bs << v.bClass;
	if (v.bClass)
		bs << bits(v.wClass, 11);
	switch (v.iQuality) {
		case 1:			//low quality
			bs << bits(v.loQual, 3);
			break;
		case 2:			//normal
			if (get<0>(t))	//bIsCharm
				bs << bits(v.wCharm, 12);
			break;
		case 3:			//high quality
			bs << bits(v.hiQual, 3);
			break;
		case 4:			//magically enhanced
			bs << bits(v.wPrefix, 11) << bits(v.wSuffix, 11);
			break;
		case 5:			//part of a set
			bs << bits(v.wSetID, 12);
			break;
		case 6:			//rare
			bs << v.pRareName;
			break;
		case 7:			//unique
			bs << bits(v.wUniID, 12);
			break;
		case 8:			//crafted
			bs << v.pCraftName;
			break;
		default:
			::MessageBox(0, CSFormat(::theApp.MsgBoxInfo(7), UINT(v.iQuality)), 0, MB_OK);
			throw 0;
	}
	if (get<1>(t))	//bRuneWord
		bs << bits(v.wRune, 16);
	if (get<2>(t))	//bPersonalized
		for (auto c : v.sPersonName) {
			bs << c;
			if (!bs.Good() || !c)
				break;
		}
	if (get<3>(t))	//bIsTome
		bs << bits(v.iTome, 5);
	else if (get<4>(t))	//bHasMonsterID
		bs << bits(v.wMonsterID, 10);
	else if (get<5>(t))	//bHasSpellID
		bs << bits(v.bSpellID, 5);
	return bs;
}

//struct CTypeSpecificInfo

template<class T>
CInBitsStream & operator >>(CInBitsStream & bs, pair<CTypeSpecificInfo &, const T &> & p) {
	auto & v = p.first;
	auto & t = p.second;
	if (get<0>(t))	//bHasDef
		bs >> bits(v.iDefence, 11);
	if (get<1>(t)) {	//bHasDur
		bs >> bits(v.iMaxDurability, 8);
		if (v.iMaxDurability)
			bs >> bits(v.iCurDur, 9);
	}
	if (get<2>(t))	//bSocketed
		bs >> bits(v.iSocket, 4);
	if (get<3>(t))	//bIsStacked
		bs >> bits(v.iQuantity, 9);
	if (get<4>(t)) 	//bIsSet, 这是一个套装
		for (auto & b : v.aHasSetPropList.ensure())
			if(bs.Good())
				bs >> b;
	bs >> v.stPropertyList;
	if (get<4>(t)) 	//bIsSet, 这是一个套装
		for (size_t i = 0; bs.Good() && i < v.aHasSetPropList.size(); ++i)
			if (v.aHasSetPropList[i])
				bs >> v.apSetProperty[i].ensure();
	if (get<5>(t))	//bRuneWord, 有符文之语属性
		bs >> v.stRuneWordPropertyList;
	return bs;
}

template<class T>
COutBitsStream & operator <<(COutBitsStream & bs, pair<const CTypeSpecificInfo &, const T &> & p) {
	auto & v = p.first;
	auto & t = p.second;
	if (get<0>(t))	//bHasDef
		bs << bits(v.iDefence, 11);
	if (get<1>(t)) {	//bHasDur
		bs << bits(v.iMaxDurability, 8);
		if (v.iMaxDurability)
			bs << bits(v.iCurDur, 9);
	}
	if (get<2>(t))	//bSocketed
		bs << bits(v.iSocket, 4);
	if (get<3>(t))	//bIsStacked
		bs << bits(v.iQuantity, 9);
	if (get<4>(t)) 	//bIsSet, 这是一个套装
		for (auto b : v.aHasSetPropList)
			if(bs.Good())
				bs << b;
	bs << v.stPropertyList;
	if (get<4>(t)) 	//bIsSet, 这是一个套装
		for (size_t i = 0; bs.Good() && i < v.aHasSetPropList.size(); ++i)
			if (v.aHasSetPropList[i])
				bs << v.apSetProperty[i];
	if (get<5>(t))	//bRuneWord, 有符文之语属性
		bs << v.stRuneWordPropertyList;
	return bs;
}

int CTypeSpecificInfo::Sockets() const {
	int s = iSocket;
	s += stPropertyList.ExtSockets();
	for (auto & p : apSetProperty)
		if (p.exist())
			s += p->ExtSockets();
	if (stRuneWordPropertyList.exist())
		s += stRuneWordPropertyList->ExtSockets();
	return min(s, 6);	//最多孔数不超过6
}

// struct CItemInfo

const CItemMetaData *  CItemInfo::ReadData(CInBitsStream & bs, BOOL bSimple, BOOL bRuneWord, BOOL bPersonalized, BOOL bSocketed) {
	for (auto & b : sTypeName)
		bs >> bits(b, 8);
	auto pItemData = ::theApp.ItemMetaData(dwTypeID);
	if (!pItemData) {	//本程序不能识别此物品
		if (IsNameValid()) {
			MessageBox(0, CSFormat(::theApp.MsgBoxInfo(6),
				sTypeName[0],
				sTypeName[1],
				sTypeName[2],
				sTypeName[3]),
				::theApp.MsgError(), MB_ICONERROR);
		} else
			MessageBox(0, ::theApp.MsgBoxInfo(18), ::theApp.MsgError(), MB_ICONERROR);
		throw 0;
	}
	if (!bSimple)	//物品有额外属性
		bs >> pack(pExtItemInfo.ensure(),
			make_tuple(pItemData->IsCharm,
			bRuneWord,
			bPersonalized,
			pItemData->IsTome,
			pItemData->HasMonsterID,
			pItemData->HasSpellID));
	//特殊物品类型的额外数据
	if (IsTypeName("gld "))	//gld 的数量域
		bs >> pGold;
	bs >> bHasRand;
	if (bHasRand)
		for (auto & i : pTmStFlag.ensure())
			if (bs.Good())
				bs >> bits(i, 32);
	if (!bSimple)	//Type Specific info
		bs >> pack(pTpSpInfo.ensure(),
			make_tuple(pItemData->HasDef,
				pItemData->HasDur,
				bSocketed,
				pItemData->IsStacked,
				pExtItemInfo->IsSet(),
				bRuneWord));
	return pItemData;
}

void CItemInfo::WriteData(COutBitsStream & bs, const CItemMetaData & itemData, BOOL bSimple, BOOL bRuneWord, BOOL bPersonalized, BOOL bSocketed) const {
	for (auto b : sTypeName)
		bs << bits(b, 8);
	if (!bSimple)	//物品有额外属性
		bs << pack(*pExtItemInfo,
			make_tuple(itemData.IsCharm,
				bRuneWord,
				bPersonalized,
				itemData.IsTome,
				itemData.HasMonsterID,
				itemData.HasSpellID));
	//特殊物品类型的额外数据
	if (IsTypeName("gld "))	//gld 的数量域
		bs << pGold;
	bs << bHasRand;
	if (bHasRand)
		for (auto i : pTmStFlag)
			if (bs.Good())
				bs << i;
	if (!bSimple)	//Type Specific info
		bs << pack(*pTpSpInfo,
			make_tuple(itemData.HasDef,
				itemData.HasDur,
				bSocketed,
				itemData.IsStacked,
				pExtItemInfo->IsSet(),
				bRuneWord));
}

BOOL CItemInfo::IsNameValid() const {
	for(char c : sTypeName)
		if(c != ' ' && (c < 'a' || c > 'z'))
			return FALSE;
	return TRUE;
}

BOOL CItemInfo::IsTypeName(const char * name) const {
	return memcmp(sTypeName, name, size(sTypeName)) == 0;
}

//CD2Item

typedef deque<CString> __Tokens;

static CString text(const __Tokens & tokens) {
	CString ret;
	int i = 0;
	for (auto & t : tokens) {
		if (t.IsEmpty())
			continue;
		if (i++)
			ret += _T(" ");
		ret += t;
	}
	return ret;
}

CString CD2Item::ItemName() const {
	if (bEar)	//耳朵
		return CSFormat(::theApp.ItemSuspendUI(10), CString(pEar->sEarName));
	__Tokens name{ ::theApp.ItemName(MetaData().NameIndex) };
	if (!bSimple) {
		ASSERT(pItemInfo->pExtItemInfo.exist());
		switch (pItemInfo->pExtItemInfo->iQuality) {
			case 1:		//low
				name.push_front(::theApp.ItemSuspendUI(0));
				break;
			case 3:		 //high
				name.push_front(::theApp.ItemSuspendUI(1));
				break;
			case 4:		//magic
				name.push_front(::theApp.MagicPrefix(pItemInfo->pExtItemInfo->wPrefix));
				name.push_back(::theApp.MagicSuffix(pItemInfo->pExtItemInfo->wSuffix));
				break;
			case 5:		//set
				name.push_front(::theApp.SetItemName(pItemInfo->pExtItemInfo->wSetID));
				break;
			case 6:
			{	//rare
				const auto & rare = *pItemInfo->pExtItemInfo->pRareName;
				name.insert(name.begin(), { ::theApp.RareCraftedName(rare.iName1), ::theApp.RareCraftedName(rare.iName2) });
				break;
			}
			case 7:		//unique
				name.push_front(::theApp.UniqueName(pItemInfo->pExtItemInfo->wUniID));
				break;
			case 8:
			{	//crafted
				const auto & craft = *pItemInfo->pExtItemInfo->pCraftName;
				name.insert(name.begin(), { ::theApp.RareCraftedName(craft.iName1), ::theApp.RareCraftedName(craft.iName2) });
				break;;
			}
			default:
				if (IsRuneWord())
					name.push_front(::theApp.RuneWordName(RuneWordId()));
		}
	}
	return text(name);
}

void CD2Item::ReadData(CInBitsStream & bs) {
	bs >> wMajic;
	if (wMajic != 0x4D4A) {
		MessageBox(0, ::theApp.MsgBoxInfo(18), ::theApp.MsgError(), MB_ICONERROR);
		throw 0;
	}
	bs >> bQuest
		>> bits(iUNKNOWN_01, 3)
		>> bIdentified
		>> bits(iUNKNOWN_02, 3)
		>> bIllegalInventory
		>> bits(iUNKNOWN_10, 2)
		>> bSocketed
		>> bits(iUNKNOWN_03, 2)
		>> bBadEquipped
		>> iUNKNOWN_04
		>> bEar
		>> bNewbie
		>> bits(iUNKNOWN_05, 3)
		>> bSimple
		>> bEthereal
		>> iUNKNOWN_06
		>> bPersonalized
		>> iUNKNOWN_07
		>> bRuneWord
		>> bits(iUNKNOWN_08, 5)
		>> bits(wVersion, 10)
		>> bits(iLocation, 3)
		>> bits(iPosition, 4)
		>> bits(iColumn, 4)
		>> bits(iRow, 4)
		>> bits(iStoredIn, 3);
	if(bEar){	//这是一个耳朵
		bs >> pEar;
        pItemData = ::theApp.ItemMetaData(0x20726165);	//"ear "
	} else {	//这是一个物品,但是也可能为"ear "
		pItemData = pItemInfo.ensure().ReadData(bs, bSimple, bRuneWord, bPersonalized, bSocketed);
		ASSERT(pItemData);
	}
	bs.AlignByte();
	aGemItems.resize(Gems());
	for (auto & item : aGemItems)
		item.ReadData(bs);
}

void CD2Item::WriteData(COutBitsStream & bs) const {
	if (!vUnknownItem.empty()) {	//未识别物品数据
		bs << vUnknownItem;
	} else {
		bs << WORD(0x4D4A)
			<< bQuest
			<< bits(iUNKNOWN_01, 3)
			<< bIdentified
			<< bits(iUNKNOWN_02, 3)
			<< bIllegalInventory
			<< bits(iUNKNOWN_10, 2)
			<< bSocketed
			<< bits(iUNKNOWN_03, 2)
			<< bBadEquipped
			<< iUNKNOWN_04
			<< bEar
			<< bNewbie
			<< bits(iUNKNOWN_05, 3)
			<< bSimple
			<< bEthereal
			<< iUNKNOWN_06
			<< bPersonalized
			<< iUNKNOWN_07
			<< bRuneWord
			<< bits(iUNKNOWN_08, 5)
			<< bits(wVersion, 10)
			<< bits(iLocation, 3)
			<< bits(iPosition, 4)
			<< bits(iColumn, 4)
			<< bits(iRow, 4)
			<< bits(iStoredIn, 3);
		if (bEar) {	//这是一个耳朵
			bs << pEar;
		} else		//这是一个物品
			pItemInfo->WriteData(bs, *pItemData, bSimple, bRuneWord, bPersonalized, bSocketed);
	}
	bs.AlignByte();
	for (auto item : aGemItems)
		if(bs.Good())
			item.WriteData(bs);
}

// struct CItemList

CInBitsStream & operator >>(CInBitsStream & bs, CItemList & v){
	bs >> v.wMajic >> v.nItems;
	if (v.wMajic != 0x4D4A) {
		MessageBox(0, ::theApp.MsgBoxInfo(17), ::theApp.MsgError(), MB_ICONERROR);
		throw 0;
	}
	v.vItems.resize(v.nItems);
	for (auto & item : v.vItems) {
		if (!bs.Good())
			break;
		item.ReadData(bs);
	}
	return bs;
}

COutBitsStream & operator <<(COutBitsStream & bs, const CItemList & v){
	bs << WORD(0x4D4A)<<WORD(v.vItems.size());
	const auto off = bs.BytePos();
	for (auto & item : v.vItems) {
		if (!bs.Good())
			break;
		item.WriteData(bs);
	}
	return bs;
}

