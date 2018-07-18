#include "StdAfx.h"
#include "Diablo Edit2.h"
#include "D2Item.h"
#include "DataStructs.h"
#include <fstream>

//CPlayerStats
const DWORD	CPlayerStats::BITS_COUNT[CPlayerStats::ARRAY_SIZE] = {
	10,10,10,10,10,8,
	21,21,21,21,21,21,
	7,32,25,25
};

void CPlayerStats::ReadData(CInBitsStream & bs) {
	bs >> wMajic;
	if (wMajic != 0x6667) {
		MessageBox(0, ::theApp.MsgBoxInfo(15), ::theApp.MsgError(), MB_ICONERROR);
		throw 0;
	}
	::ZeroMemory(m_dwValue, sizeof(m_dwValue));
	DWORD index;
	bs.ReadBits(index, 9);
	for (DWORD i = 0; bs.Good() && index < ARRAY_SIZE; ++i) {
		if (i > index)	//Data format error
			throw 0;
		bs.ReadBits(m_dwValue[index], BITS_COUNT[index]);
		bs.ReadBits(index, 9);
	}
	bs.AlignByte();
}

void CPlayerStats::WriteData(COutBitsStream & bs) const {
	bs << WORD(0x6667);
	for (DWORD i = 0; i < ARRAY_SIZE; ++i)
		if (m_dwValue[i])
			bs << bits(WORD(i), 9) << bits(m_dwValue[i], BITS_COUNT[i]);
	bs << bits<WORD>(0x1FF, 9);
	bs.AlignByte();
}

// struct CEar
void CEar::ReadData(CInBitsStream & bs) {
	bs >> bits(iEarClass, 3) >> bits(iEarLevel, 7);
	for (auto & b : sEarName) {
		bs >> bits(b, 7);
		if (!bs.Good() || b == 0)
			break;
	}
}

void CEar::WriteData(COutBitsStream & bs) const {
	bs << bits(iEarClass, 3) << bits(iEarLevel, 7);
	for (auto b : sEarName) {
		bs << bits(b, 7);
		if (b == 0)
			break;
	}
}

// struct CLongName
void CLongName::ReadData(CInBitsStream & bs) {
	bs >> bits(iName1, 8)
		>> bits(iName2, 8)
		>> bPref1;
	if (bPref1)
		bs >> bits(*wPref1, 11);
	bs >> bSuff1;
	if (bSuff1)
		bs >> bits(*wSuff1, 11);
	bs >> bPref2;
	if (bPref2)
		bs >> bits(*wPref2, 11);
	bs >> bSuff2;
	if (bSuff2)
		bs >> bits(*wSuff2, 11);
	bs >> bPref3;
	if (bPref3)
		bs >> bits(*wPref3, 11);
	bs >> bSuff3;
	if (bSuff3)
		bs >> bits(*wSuff3, 11);
}

void CLongName::WriteData(COutBitsStream & bs) const {
	bs << bits(iName1, 8)
		<< bits(iName2, 8)
		<< bPref1;
	if (bPref1)
		bs << bits(*wPref1, 11);
	bs << bSuff1;
	if (bSuff1)
		bs << bits(*wSuff1, 11);
	bs << bPref2;
	if (bPref2)
		bs << bits(*wPref2, 11);
	bs << bSuff2;
	if (bSuff2)
		bs << bits(*wSuff2, 11);
	bs << bPref3;
	if (bPref3)
		bs << bits(*wPref3, 11);
	bs << bSuff3;
	if (bSuff3)
		bs << bits(*wSuff3, 11);
}


// struct CGoldQuantity
void CGoldQuantity::ReadData(CInBitsStream & bs) {
	bs >> bNotGold >> bits(wQuantity, 12);
}

void CGoldQuantity::WriteData(COutBitsStream & bs) const {
	bs << bNotGold << bits(wQuantity, 12);
}

// struct CExtItemInfo
void CExtItemInfo::ReadData(CInBitsStream & bs, BOOL bIsCharm, BOOL bRuneWord, BOOL bPersonalized, BOOL bIsTome, BOOL bHasMonsterID, BOOL bHasSpellID) {
	bs >> bits(nGems, 3)
		>> bits(dwGUID, 32)
		>> bits(iDropLevel, 7)
		>> bits(iQuality, 4)
		>> bVarGfx;
	if (bVarGfx)
		bs >> bits(*iVarGfx, 3);
	bs >> bClass;
	if (bClass)
		bs >> bits(*wClass, 11);
	switch (iQuality) {
		case 1:			//low quality
			bs >> bits(*loQual, 3);
			break;
		case 2:			//normal
			if (bIsCharm)
				bs >> bits(*wCharm, 12);
			break;
		case 3:			//high quality
			bs >> bits(*hiQual, 3);
			break;
		case 4:			//magically enhanced
			bs >> bits(*wPrefix, 11)
				>> bits(*wSuffix, 11);
			break;
		case 5:			//part of a set
			bs >> bits(*wSetID, 12);
			break;
		case 6:			//rare
			pRareName->ReadData(bs);
			break;
		case 7:			//unique
			bs >> bits(*wUniID, 12);
			break;
		case 8:			//crafted
			pCraftName->ReadData(bs);
			break;
		default:
			::MessageBox(0, CSFormat(::theApp.MsgBoxInfo(7), UINT(iQuality)), 0, MB_OK);
			throw 0;
	}
	if (bRuneWord)
		bs >> bits(*wRune, 16);
	if (bPersonalized)
		for (int i = 0; i < 16; ++i) {
			bs >> bits(sPersonName[i], 7);
			if (sPersonName[i] == 0)
				break;
		}
	if (bIsTome)
		bs >> bits(*iTome, 5);
	else if (bHasMonsterID)
		bs >> bits(*wMonsterID, 10);
	else if (bHasSpellID)
		bs >> bits(*bSpellID, 5);
}

void CExtItemInfo::WriteData(COutBitsStream & bs, BOOL bIsCharm, BOOL bRuneWord, BOOL bPersonalized, BOOL bIsTome, BOOL bHasMonsterID, BOOL bHasSpellID) const {
	bs << bits(nGems, 3)
		<< bits(dwGUID, 32)
		<< bits(iDropLevel, 7)
		<< bits(iQuality, 4)
		<< bVarGfx;
	if (bVarGfx)
		bs << bits(*iVarGfx, 3);
	bs << bClass;
	if (bClass)
		bs << bits(*wClass, 11);
	switch (iQuality) {
		case 1:			//low quality
			bs << bits(*loQual, 3);
			break;
		case 2:			//normal
			if (bIsCharm)
				bs << bits(*wCharm, 12);
			break;
		case 3:			//high quality
			bs << bits(*hiQual, 3);
			break;
		case 4:			//magically enhanced
			bs << bits(*wPrefix, 11)
				<< bits(*wSuffix, 11);
			break;
		case 5:			//part of a set
			bs << bits(*wSetID, 12);
			break;
		case 6:			//rare
			pRareName->WriteData(bs);
			break;
		case 7:			//unique
			bs << bits(*wUniID, 12);
			break;
		case 8:			//crafted
			pCraftName->WriteData(bs);
			break;
		default:
			::MessageBox(0, CSFormat(::theApp.MsgBoxInfo(7), UINT(iQuality)), 0, MB_OK);
			throw 0;
	}
	if (bRuneWord)
		bs << bits(*wRune, 16);
	if (bPersonalized)
		for (int i = 0; i < 16; ++i) {
			bs << bits(sPersonName[i], 7);
			if (sPersonName[i] == 0)
				break;
		}
	if (bIsTome)
		bs << bits(*iTome, 5);
	else if (bHasMonsterID)
		bs << bits(*wMonsterID, 10);
	else if (bHasSpellID)
		bs << bits(*bSpellID, 5);
}


//struct CTypeSpecificInfo
void CTypeSpecificInfo::ReadData(CInBitsStream & bs, BOOL bHasDef, BOOL bHasDur, BOOL bSocketed, BOOL bIsStacked, BOOL bIsSet, BOOL bRuneWord) {
	if (bHasDef)
		bs >> bits(*iDefence, 11);
	if (bHasDur) {
		bs >> bits(*iMaxDurability, 8);
		if (iMaxDurability.Value())
			bs >> bits(*iCurDur, 9);
	}
	if (bSocketed)
		bs >> bits(*iSocket, 4);
	if (bIsStacked)
		bs >> bits(*iQuantity, 9);
	if (bIsSet) {	//这是一个套装
		//暂时不支持.......................
	}
	if (bRuneWord) {		//有符文之语属性
		//暂时不支持.......................
	}
	//接下来是额外属性列表
	for (bs >> bits(iEndFlag, 9); bs.Good() && iEndFlag < 0x1FF; bs >> bits(iEndFlag, 9))
		bs >> bits(mProperty[iEndFlag], ::theApp.PropertyData(iEndFlag).Bits());
}

void CTypeSpecificInfo::WriteData(COutBitsStream & bs, BOOL bHasDef, BOOL bHasDur, BOOL bSocketed, BOOL bIsStacked, BOOL bIsSet, BOOL bRuneWord) const {
	if (bHasDef)
		bs << bits(*iDefence, 11);
	if (bHasDur) {
		bs << bits(*iMaxDurability, 8);
		if (*iMaxDurability)
			bs << bits(*iCurDur, 9);
	}
	if (bSocketed)
		bs << bits(*iSocket, 4);
	if (bIsStacked)
		bs << bits(*iQuantity, 9);
	if (bIsSet) {	//这是一个套装
		//暂时不支持.......................
	}
	if (bRuneWord) {		//有符文之语属性
		//暂时不支持.......................
	}
	//接下来是额外属性列表
	for (const auto & p : mProperty)
		bs << bits(p.first, 9) << bits(p.second, ::theApp.PropertyData(p.first).Bits());
	bs << bits<WORD>(0x1FF, 9);
}

// struct CItemInfo
const CItemDataStruct *  CItemInfo::ReadData(CInBitsStream & bs, BOOL bSimple, BOOL bRuneWord, BOOL bPersonalized, BOOL bSocketed) {
	for (auto & b : sTypeName)
		bs >> bits(b, 8);
	auto pItemData = ::theApp.ItemData(dwTypeID);
	if (!pItemData)		//本程序不能识别此物品
		return 0;
	if (!bSimple)	//物品有额外属性
		pExtItemInfo->ReadData(bs,
			pItemData->IsCharm,
			bRuneWord,
			bPersonalized,
			pItemData->IsTome,
			pItemData->HasMonsterID,
			pItemData->HasSpellID);
	//特殊物品类型的额外数据
	if (IsSameType(sTypeName, "gld "))	//gld 的数量域
		pGold->ReadData(bs);
	bs >> bHasRand;
	if (bHasRand)
		for (int i = 0; i < 3; ++i)
			bs >> bits(pTmStFlag[i], 32);
	if (!bSimple) {	//Type Specific info
		pTpSpInfo->ReadData(bs,
			pItemData->HasDef,
			pItemData->HasDur,
			bSocketed,
			pItemData->IsStacked,
			pExtItemInfo->IsSet(),
			bRuneWord);
	}
	return pItemData;
}

void CItemInfo::WriteData(COutBitsStream & bs, const CItemDataStruct & itemData, BOOL bSimple, BOOL bRuneWord, BOOL bPersonalized, BOOL bSocketed) const {
	for (auto b : sTypeName)
		bs << bits(b, 8);
	if (!bSimple)	//物品有额外属性
		pExtItemInfo->WriteData(bs,
			itemData.IsCharm,
			bRuneWord,
			bPersonalized,
			itemData.IsTome,
			itemData.HasMonsterID,
			itemData.HasSpellID);
	//特殊物品类型的额外数据
	if (IsSameType(sTypeName, "gld "))	//gld 的数量域
		pGold->WriteData(bs);
	bs << bHasRand;
	if (bHasRand)
		for (int i = 0; i < 3; ++i)
			bs << bits(pTmStFlag[i], 32);
	if (!bSimple) {	//Type Specific info
		pTpSpInfo->WriteData(bs,
			itemData.HasDef,
			itemData.HasDur,
			bSocketed,
			itemData.IsStacked,
			pExtItemInfo->IsSet(),
			bRuneWord);
	}
}

BOOL CItemInfo::IsNameValid() const {
	for(char c : sTypeName)
		if(c != ' ' && (c < 'a' || c > 'z'))
			return FALSE;
	return TRUE;
}

//CD2Item

void CD2Item::findUnknownItem(CInBitsStream & bs) {
	if (!pItemInfo->IsNameValid())
		throw 0;
	DWORD from = bs.BytePos() - 13;
	bs.AlignByte();
	bs.SkipUntil("JM");
	bs.Restore(vItemData, from, bs.BytePos());
	if (!vItemData.empty()) {	//把未知的物品存储成物品文件
		CFile outf(::theApp.AppPath() + _T("unknown.d2i"), CFile::modeCreate | CFile::modeWrite);
		outf.Write(&vItemData[0], UINT(vItemData.size()));
	}
	if (MessageBox(0, CSFormat(::theApp.MsgBoxInfo(6),
			pItemInfo->sTypeName[0],
			pItemInfo->sTypeName[1],
			pItemInfo->sTypeName[2],
			pItemInfo->sTypeName[3]),
		::theApp.MsgWarning(), MB_YESNO | MB_ICONWARNING) == IDNO)
		throw 0;
}

void CD2Item::ReadData(CInBitsStream & bs)
{
	bs>>wMajic;
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
		pEar->ReadData(bs);
		char buf[4] { 'e', 'a', 'r', ' ' };
        DWORD * type = reinterpret_cast<DWORD *>(buf);
        pItemData = ::theApp.ItemData(*type);
	} else {	//这是一个物品,但是也可能为"ear "
		pItemData = pItemInfo->ReadData(bs, bSimple, bRuneWord, bPersonalized, bSocketed);
		if (!pItemData)
			findUnknownItem(bs);
	}
	bs.AlignByte();
}

void CD2Item::WriteData(COutBitsStream & bs) const {
	if (!vItemData.empty()) {	//未识别物品数据
		bs << vItemData;
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
			pEar->WriteData(bs);
		} else {	//这是一个物品
			pItemInfo->WriteData(bs, *pItemData, bSimple, bRuneWord, bPersonalized, bSocketed);
		}
	}
	bs.AlignByte();
}
