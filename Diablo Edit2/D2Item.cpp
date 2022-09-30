#include "StdAfx.h"
#include "Diablo Edit2.h"
#include "D2Item.h"

#include <fstream>
#include <iterator>
#include <cstring>
#include <deque>
#include <random>
#include <ctime>
#include <map>

using namespace std;

class CRandom {
	mt19937 r_;
	uniform_int_distribution<DWORD>	dw_;
public:
	CRandom() {
		r_.seed(random_device()());
	}
	DWORD dwValue() { return dw_(r_); }
};

static CRandom g_rand;

//static inline BOOL isLetter(TCHAR ch) {
//	return (ch >= _T('a') && ch <= _T('z')) || (ch >= _T('A') && ch <= _T('Z'));
//}

//static inline BOOL isDash(TCHAR ch) {
//	return ch == _T('-') || ch == _T('_');
//}

BOOL CheckCharName(const CString& name) {
	int len = name.GetLength();
	return len > 1 && len < 16;
	// From D2R 2.4, character name can be UTF-8. So the old check no longer applies.
	//
	//if (len >= 2 && len < 16 && isLetter(name[0]) && isLetter(name[len - 1])) {
	//	for (int i = 2, j = 0; i < len; ++i)
	//		if (isDash(name[i])) {
	//			if (++j > 1)
	//				return FALSE;
	//		} else if (!isLetter(name[i]))
	//			return FALSE;
	//		return TRUE;
	//}
	//return FALSE;
}

CString DecodeCharName(const BYTE* name) {
	ASSERT(name);
	int len = MultiByteToWideChar(CP_UTF8, 0, (LPCCH)name, -1, NULL, 0);
	if (len <= 0) return L"";
	CString ret;
	wchar_t* buf = ret.GetBuffer(len);
	if (buf) MultiByteToWideChar(CP_UTF8, 0, (LPCCH)name, -1, buf, len);
	ret.ReleaseBuffer();
	return ret;
}

CStringA EncodeCharName(const CString& name) {
	int len = WideCharToMultiByte(CP_UTF8, 0, name, -1, NULL, 0, 0, 0);
	if (len <= 0) return "";
	CStringA utf8;
	char* buf = utf8.GetBuffer(len);
	if (buf) WideCharToMultiByte(CP_UTF8, 0, name, -1, buf, len, 0, 0);
	utf8.ReleaseBuffer();
	return utf8;
}

// Succinct encoding of a Huffman tree
const BYTE HUFFMAN[] = {
   1,	1,	1,	1,	1,	'w',0,	0,	'u',0,
   0,	1,	1,	'8',0,	0,	1,	'y',0,	0,
   1,	'5',0,	0,	1,	'j',0,	0,	1,	0,
   0,	'h',0,	0,	1,	's',0,	0,	1,	1,
   '2',0,	0,	'n',0,	0,	'x',0,	0,	1,
   1,	1,	'c',0,	0,	1,	'k',0,	0,	'f',
   0,	0,	'b',0,	0,	1,	1,	't',0,	0,
   'm',0,	0,	1,	'9',0,	0,	'7',0,	0,
   1,	' ',0,	0,	1,	1,	1,	1,	'e',0,
   0,	'd',0,	0,	'p',0,	0,	1,	'g',0,
   0,	1,	1,	1,	'z',0,	0,	'q',0,	0,
   '3',0,	0,	1,	'v',0,	0,	'6',0,	0,
   1,	1,	'r',0,	0,	'l',0,	0,	1,	'a',
   0,	0,	1,	1,	'1',0,	0,	1,	'4',0,
   0,	'0',0,	0,	1,	'i',0,	0,	'o',0,
};

class HuffmanTree {
	struct HuffmanNode {
		const HuffmanNode* left = NULL;
		const HuffmanNode* right = NULL;
		BYTE data = 0;
		~HuffmanNode() {
			delete left;
			delete right;
		}
	};
public:
	HuffmanTree() {
		int i = 0;
		root_ = constructHuffmanTree(i);
	}
	~HuffmanTree() {
		delete root_;
	}
	BYTE readData(CInBitsStream& bs) const {
		auto node = root_;
		for (BOOL b; bs.Good(); node = b ? node->right : node->left) {
			ASSERT(node);
			if (node->data)return node->data;
			bs >> b;
		}
		ASSERT(false);
		return 0;
	}
	void writeData(COutBitsStream& bs, BYTE ch) const {
		auto wh = codes_.find(ch);
		ASSERT(wh != codes_.end());
		bs << bits(wh->second.first, wh->second.second);
	}
private:
	HuffmanTree(const HuffmanTree&);
	HuffmanTree& operator =(const HuffmanTree&);
	const HuffmanNode* constructHuffmanTree(int& index, int bits = 0, DWORD code = 0) {
		if (index >= sizeof HUFFMAN)return NULL;
		const BYTE c = HUFFMAN[index++];
		if (!c)
			return NULL;
		HuffmanNode* node = new HuffmanNode;
		if (c > 1) {
			node->data = c;
			codes_[c] = make_pair(code, bits);
		}
		node->left = constructHuffmanTree(index, bits + 1, code);
		node->right = constructHuffmanTree(index, bits + 1, code + (1 << bits));
		return node;
	}
	const HuffmanNode* root_;
	map<BYTE, pair<DWORD, int>> codes_;	//char => (code, bits)
};

static const HuffmanTree g_huffmanTree;

// struct CEar

void CEar::ReadData(CInBitsStream& bs, BOOL isPtr24) {
	bs >> bits(iEarClass, 3) >> bits(iEarLevel, 7);
	int b = isPtr24 ? 8 : 7;
	BYTE buf[0x40] = { 0 };	//人物名字最多15个UTF8字符 + \0 (16 * 4)
	for (auto& c : buf) {
		bs >> bits(c, b);
		if (!bs.Good() || c == 0)
			break;
	}
	sEarName = DecodeCharName(buf);
}

void CEar::WriteData(COutBitsStream& bs, BOOL isPtr24) const {
	bs << bits(iEarClass, 3) << bits(iEarLevel, 7);
	const CStringA buf = EncodeCharName(sEarName);
	int b = isPtr24 ? 8 : 7;
	int len = isPtr24 ? 60 : 15;	//60 = 15 * 4
	for (int i = 0; i < buf.GetLength() && i < len; ++i) {
		BYTE c = buf[i];
		bs << bits(c, b);
		if (!bs.Good() || c == 0)
			break;
	}
	bs << bits(BYTE(0), b);
}

// struct CLongName

void CLongName::ReadData(CInBitsStream& bs) {
	bs >> bits(iName1, 8)
		>> bits(iName2, 8)
		>> bPref1;
	if (bPref1)
		bs >> bits(wPref1, 11);
	bs >> bSuff1;
	if (bSuff1)
		bs >> bits(wSuff1, 11);
	bs >> bPref2;
	if (bPref2)
		bs >> bits(wPref2, 11);
	bs >> bSuff2;
	if (bSuff2)
		bs >> bits(wSuff2, 11);
	bs >> bPref3;
	if (bPref3)
		bs >> bits(wPref3, 11);
	bs >> bSuff3;
	if (bSuff3)
		bs >> bits(wSuff3, 11);
}

void CLongName::WriteData(COutBitsStream& bs) const {
	bs << bits(iName1, 8)
		<< bits(iName2, 8)
		<< bPref1;
	if (bPref1)
		bs << bits(wPref1, 11);
	bs << bSuff1;
	if (bSuff1)
		bs << bits(wSuff1, 11);
	bs << bPref2;
	if (bPref2)
		bs << bits(wPref2, 11);
	bs << bSuff2;
	if (bSuff2)
		bs << bits(wSuff2, 11);
	bs << bPref3;
	if (bPref3)
		bs << bits(wPref3, 11);
	bs << bSuff3;
	if (bSuff3)
		bs << bits(wSuff3, 11);
}

// struct CGoldQuantity

void CGoldQuantity::ReadData(CInBitsStream& bs) {
	bs >> bNotGold >> bits(wQuantity, 12);
}

void CGoldQuantity::WriteData(COutBitsStream& bs) const {
	bs << bNotGold << bits(wQuantity, 12);
}

//struct CPropertyList

void CPropertyList::ReadData(CInBitsStream& bs) {
	for (bs >> bits(iEndFlag, 9); bs.Good() && iEndFlag < 0x1FF; bs >> bits(iEndFlag, 9)) {
		const int b = ::theApp.PropertyMetaData(iEndFlag).Bits();
		if (b > 0)
			bs >> bits(mProperty.emplace_back(iEndFlag, 0).second, b);
	}
}
void CPropertyList::WriteData(COutBitsStream& bs) const {
	for (auto& p : mProperty) {
		const int b = ::theApp.PropertyMetaData(p.first).Bits();
		if (b > 0)
			bs << bits(p.first, 9) << bits(p.second, b);
	}
	bs << bits<WORD>(0x1FF, 9);
}

int CPropertyList::ExtSockets() const {
	int r = 0;
	for (auto& p : mProperty)
		if (p.first == 194)	//194是额外孔属性ID
			r += p.second;	//可能有多个194属性
	return r;
}

BOOL CPropertyList::IsIndestructible() const {
	for (auto& p : mProperty)
		if (p.first == 152 && p.second != 0)	//152是不可破坏属性ID
			return TRUE;	//可能有多个152属性
	return FALSE;
}

//pack

template<class V, class T>
static pair<V&, const T&> pack(V& v, const T& t) {
	return pair<V&, const T&>(v, t);
}

// struct CExtItemInfo

CExtItemInfo::CExtItemInfo(const CItemMetaData* meta) {
	if (meta) {
		dwGUID = g_rand.dwValue();
		if (meta->HasMonsterID)
			wMonsterID.ensure();
		if (meta->IsCharm)
			wCharm.ensure();
		if (meta->SpellId > 0)
			iSpellID.ensure(meta->SpellId - 1);
	}
}

void CExtItemInfo::ReadData(CInBitsStream& bs, BOOL bIsCharm, BOOL bRuneWord, BOOL bPersonalized, BOOL bHasMonsterID, BOOL bHasSpellId, BOOL isPtr24) {
	bs >> bits(nGems, 3)
		>> bits(dwGUID, 32)
		>> bits(iDropLevel, 7)
		>> bits(iQuality, 4)
		>> bVarGfx;
	if (bVarGfx)
		bs >> bits(iVarGfx, 3);
	bs >> bClass;
	if (bClass)
		bs >> bits(wClass, 11);
	switch (iQuality) {
	case 1:			//low quality
		bs >> bits(loQual, 3);
		break;
	case 2:			//normal
		if (bIsCharm)
			bs >> bits(wCharm, 12);
		break;
	case 3:			//high quality
		bs >> bits(hiQual, 3);
		break;
	case 4:			//magically enhanced
		bs >> bits(wPrefix, 11) >> bits(wSuffix, 11);
		break;
	case 5:			//part of a set
		bs >> bits(wSetID, 12);
		break;
	case 6:			//rare
		pRareName.ensure().ReadData(bs);
		break;
	case 7:			//unique
		bs >> bits(wUniID, 12);
		break;
	case 8:			//crafted
		pCraftName.ensure().ReadData(bs);
		break;
	default:
		throw CSFormat(::theApp.MsgBoxInfo(7), UINT(iQuality));
	}
	if (bRuneWord)
		bs >> bits(wRune, 16);
	if (bPersonalized) {
		int b = isPtr24 ? 8 : 7;
		for (auto& c : sPersonName.ensure()) {
			bs >> bits(c, b);
			if (!bs.Good() || !c)
				break;
		}
	}
	if (bHasMonsterID)
		bs >> bits(wMonsterID, 10);
	else if (bHasSpellId)
		bs >> bits(iSpellID, 5);
}

void CExtItemInfo::WriteData(COutBitsStream& bs, BOOL bIsCharm, BOOL bRuneWord, BOOL bPersonalized, BOOL bHasMonsterID, BOOL bHasSpellId, BOOL isPtr24) const {
	bs << bits(nGems, 3)
		<< bits(dwGUID, 32)
		<< bits(iDropLevel, 7)
		<< bits(iQuality, 4)
		<< bVarGfx;
	if (bVarGfx)
		bs << bits(iVarGfx, 3);
	bs << bClass;
	if (bClass)
		bs << bits(wClass, 11);
	switch (iQuality) {
	case 1:			//low quality
		bs << bits(loQual, 3);
		break;
	case 2:			//normal
		if (bIsCharm)
			bs << bits(wCharm, 12);
		break;
	case 3:			//high quality
		bs << bits(hiQual, 3);
		break;
	case 4:			//magically enhanced
		bs << bits(wPrefix, 11) << bits(wSuffix, 11);
		break;
	case 5:			//part of a set
		bs << bits(wSetID, 12);
		break;
	case 6:			//rare
		pRareName->WriteData(bs);
		break;
	case 7:			//unique
		bs << bits(wUniID, 12);
		break;
	case 8:			//crafted
		pCraftName->WriteData(bs);
		break;
	default:
		ASSERT(FALSE && _T("Invalid item quality"));
	}
	if (bRuneWord)	//
		bs << bits(wRune, 16);
	if (bPersonalized) {
		int b = isPtr24 ? 8 : 7;
		for (auto c : sPersonName) {
			bs << bits(c, b);
			if (!bs.Good() || !c)
				break;
		}
	}
	if (bHasMonsterID)
		bs << bits(wMonsterID, 10);
	else if (bHasSpellId)
		bs << bits(iSpellID, 5);
}

//struct CTypeSpecificInfo

CTypeSpecificInfo::CTypeSpecificInfo(const CItemMetaData* meta) {
	if (meta) {
		if (meta->HasDef)
			iDefence.ensure();
		if (meta->HasDur) {
			iMaxDurability.ensure(10);
			iCurDur.ensure(10);
		}
		if (meta->IsStacked)
			iQuantity.ensure();
	}
}

void CTypeSpecificInfo::ReadData(CInBitsStream& bs, BOOL bHasDef, BOOL bHasDur, BOOL bSocketed, BOOL bIsStacked, BOOL bIsSet, BOOL bRuneWord) {
	if (bHasDef)
		bs >> bits(iDefence, 11);
	if (bHasDur) {
		bs >> bits(iMaxDurability, 8);
		if (iMaxDurability)
			bs >> bits(iCurDur, 9);
	}
	if (bSocketed) {
		bs >> bits(iSocket, 4);
		if (iSocket < 1) iSocket = 1;
	}
	if (bIsStacked)	//
		bs >> bits(iQuantity, 9);
	if (bIsSet) 	//这是一个套装
		for (auto& b : aHasSetPropList.ensure())
			if (bs.Good())
				bs >> b;
	stPropertyList.ReadData(bs);
	if (bIsSet) 	//这是一个套装
		for (size_t i = 0; bs.Good() && i < aHasSetPropList.size(); ++i)
			if (aHasSetPropList[i])
				apSetProperty[i].ensure().ReadData(bs);
	if (bRuneWord)	//有符文之语属性
		stRuneWordPropertyList.ensure().ReadData(bs);
}

void CTypeSpecificInfo::WriteData(COutBitsStream& bs, BOOL bHasDef, BOOL bHasDur, BOOL bSocketed, BOOL bIsStacked, BOOL bIsSet, BOOL bRuneWord) const {
	if (bHasDef)
		bs << bits(iDefence, 11);
	if (bHasDur) {
		bs << bits(iMaxDurability, 8);
		if (iMaxDurability)
			bs << bits(iCurDur, 9);
	}
	if (bSocketed)
		bs << bits(iSocket, 4);
	if (bIsStacked)
		bs << bits(iQuantity, 9);
	if (bIsSet) 	//这是一个套装
		for (auto b : aHasSetPropList)
			if (bs.Good())
				bs << b;
	stPropertyList.WriteData(bs);
	if (bIsSet) 	//这是一个套装
		for (size_t i = 0; bs.Good() && i < aHasSetPropList.size(); ++i)
			if (aHasSetPropList[i])
				apSetProperty[i]->WriteData(bs);
	if (bRuneWord)	//有符文之语属性
		stRuneWordPropertyList->WriteData(bs);
}

pair<int, int> CTypeSpecificInfo::Sockets() const {
	const int b = min(MAX_SOCKETS, (iSocket.exist() ? iSocket : 0));
	int e = stPropertyList.ExtSockets();
	for (auto& p : apSetProperty)
		if (p.exist())
			e += p->ExtSockets();
	if (stRuneWordPropertyList.exist())
		e += stRuneWordPropertyList->ExtSockets();
	e = min(MAX_SOCKETS - b, e);
	return make_pair(b, e);
}

BOOL CTypeSpecificInfo::IsIndestructible() const {
	if ((iMaxDurability.exist() && 0 == iMaxDurability) || stPropertyList.IsIndestructible())
		return TRUE;
	for (auto& p : apSetProperty)
		if (p.exist() && p->IsIndestructible())
			return TRUE;
	if (stRuneWordPropertyList.exist() && stRuneWordPropertyList->IsIndestructible())
		return TRUE;
	return FALSE;
}

// struct CItemInfo

CItemInfo::CItemInfo(const CItemMetaData* meta) {
	if (meta) {
		dwTypeID = meta->dwTypeID;
		if (IsGold())
			pGold.ensure();
		if (!meta->Simple) {
			pExtItemInfo.ensure(meta);
			pTpSpInfo.ensure(meta);
		}
	}
}

const CItemMetaData* CItemInfo::ReadData(CInBitsStream& bs, BOOL bSimple, BOOL bRuneWord, BOOL bPersonalized, BOOL bSocketed, BOOL isD2R, BOOL isPtr24) {
	for (auto& b : sTypeName)
		if (isD2R)
			b = g_huffmanTree.readData(bs);
		else
			bs >> bits(b, 8);
	auto pItemData = ::theApp.ItemMetaData(dwTypeID);
	if (!pItemData) {	//本程序不能识别此物品
		if (IsNameValid()) {
			throw CSFormat(::theApp.MsgBoxInfo(6), sTypeName[0], sTypeName[1], sTypeName[2], sTypeName[3]);
		}
		else
			throw ::theApp.MsgBoxInfo(18);
	}
	if (!bSimple)	//物品有额外属性
		pExtItemInfo.ensure().ReadData(bs,
			pItemData->IsCharm,
			bRuneWord,
			bPersonalized,
			pItemData->HasMonsterID,
			pItemData->SpellId,
			isPtr24);
	//特殊物品类型的额外数据
	if (IsGold())	//gld 的数量域
		pGold.ensure().ReadData(bs);
	bs >> bHasRand;
	if (!bSimple) {
		if (bHasRand)
			for (auto& i : pTmStFlag.ensure())
				if (bs.Good())
					bs >> bits(i, 32);
		//Type Specific info
		pTpSpInfo.ensure().ReadData(bs,
			pItemData->HasDef,
			pItemData->HasDur,
			bSocketed,
			pItemData->IsStacked,
			pExtItemInfo->IsSet(),
			bRuneWord);
	}
	else  if (isD2R && pItemData->Pad > 0) {
		// Simple items need an extra bit of padding. Tested in D2R only.
		// https://github.com/daidodo/diablo_edit/issues/13
		BYTE pad;
		bs >> bits(pad, pItemData->Pad);
	}
	return pItemData;
}

void CItemInfo::WriteData(COutBitsStream& bs, const CItemMetaData& itemData, BOOL bSimple, BOOL bRuneWord, BOOL bPersonalized, BOOL bSocketed, BOOL isD2R, BOOL isPtr24) const {
	for (auto b : sTypeName)
		if (isD2R)
			g_huffmanTree.writeData(bs, b);
		else
			bs << bits(b, 8);
	if (!bSimple)	//物品有额外属性
		pExtItemInfo->WriteData(bs,
			itemData.IsCharm,
			bRuneWord,
			bPersonalized,
			itemData.HasMonsterID,
			itemData.SpellId,
			isPtr24);
	//特殊物品类型的额外数据
	if (IsGold())	//gld 的数量域
		pGold->WriteData(bs);
	bs << bHasRand;
	if (bHasRand)
		for (auto i : pTmStFlag)
			if (bs.Good())
				bs << bits(i, 32);
	if (!bSimple) {	//Type Specific info
		pTpSpInfo->WriteData(bs,
			itemData.HasDef,
			itemData.HasDur,
			bSocketed,
			itemData.IsStacked,
			pExtItemInfo->IsSet(),
			bRuneWord);
	}
	else if (isD2R && itemData.Pad > 0)
		bs << bits<BYTE>(0, itemData.Pad);	// padding
}

BOOL CItemInfo::IsNameValid() const {
	for (char c : sTypeName)
		if (c != ' ' && !isalnum(c))
			return FALSE;
	return TRUE;
}

//CD2Item

typedef deque<CString> __Tokens;

static CString text(const __Tokens& tokens) {
	CString ret;
	int i = 0;
	for (auto& t : tokens) {
		if (t.IsEmpty())
			continue;
		if (i++)
			ret += _T(" ");
		ret += t;
	}
	return ret;
}

CD2Item::CD2Item(DWORD type) {
	if (type > 0) {
		pItemData = ::theApp.ItemMetaData(type);
		ASSERT(pItemData);
		bSimple = pItemData->Simple;
		if (0x20726165 == type) {	//"ear "
			bEar = TRUE;
			pEar.ensure(L"unknown");
		}
		else
			pItemInfo.ensure(pItemData);
	}
}

CString CD2Item::ItemName() const {
	if (bEar)	//耳朵
		return CSFormat(::theApp.ItemSuspendUI(10), CString(pEar->sEarName));
	__Tokens name{ ::theApp.ItemName(MetaData().NameIndex) };
	if (!bSimple) {
		ASSERT(pItemInfo->pExtItemInfo.exist());
		auto& extInfo = *pItemInfo->pExtItemInfo;
		switch (extInfo.iQuality) {
		case 1:		//low
			name.push_front(::theApp.ItemSuspendUI(0));
			break;
		case 3:		 //high
			name.push_front(::theApp.ItemSuspendUI(1));
			break;
		case 4:		//magic
			name.push_front(::theApp.MagicPrefix(extInfo.wPrefix));
			name.push_back(::theApp.MagicSuffix(extInfo.wSuffix));
			break;
		case 5:		//set
			name.push_front(::theApp.SetItemName(extInfo.wSetID));
			break;
		case 6:
		{	//rare
			const auto& rare = *extInfo.pRareName;
			name.insert(name.begin(), { ::theApp.RareCraftedName(rare.iName1), ::theApp.RareCraftedName(rare.iName2) });
			break;
		}
		case 7:		//unique
			name.push_front(::theApp.UniqueName(extInfo.wUniID));
			break;
		case 8:
		{	//crafted
			const auto& craft = *extInfo.pCraftName;
			name.insert(name.begin(), { ::theApp.RareCraftedName(craft.iName1), ::theApp.RareCraftedName(craft.iName2) });
			break;;
		}
		default:
			if (extInfo.wMonsterID.exist())
				name.push_front(::theApp.MonsterName(extInfo.wMonsterID));
		}
		if (extInfo.iQuality <= 3 && IsRuneWord())
			name.push_front(::theApp.RuneWordName(RuneWordId()));
		if (bEthereal)
			name.push_back(_T("(ETH)"));
	}
	return text(name);
}

int CD2Item::GemIndexMax() const {
	int r = -1;
	for (auto& i : aGemItems)
		r = max(r, i.iColumn);
	return r;
}

void CD2Item::ReadData(CInBitsStream& bs, BOOL isD2R, BOOL isPtr24) {
	if (!isD2R) {
		bs >> wMajic;
		if (wMajic != 0x4D4A && wMajic != 0x2010)
			throw ::theApp.MsgBoxInfo(18);
	}
	bs >> bQuest
		>> bits(iUNKNOWN_01, 3)
		>> bIdentified
		>> bits(iUNKNOWN_02, 3)
		>> bDisabled
		>> bits(iUNKNOWN_10, 2)
		>> bSocketed
		>> bits(iUNKNOWN_03, 2)
		>> bBadEquipped
		>> bUNKNOWN_04
		>> bEar
		>> bNewbie
		>> bits(iUNKNOWN_05, 3)
		>> bSimple
		>> bEthereal
		>> bUNKNOWN_06
		>> bPersonalized
		>> bUNKNOWN_07
		>> bRuneWord;
	if (isD2R)
		bs >> bits(iUNKNOWN_09, 8);
	else
		bs >> bits(iUNKNOWN_08, 5) >> bits(wVersion, 10);
	bs >> bits(iLocation, 3)
		>> bits(iPosition, 4)
		>> bits(iColumn, 4)
		>> bits(iRow, 4)
		>> bits(iStoredIn, 3);
	if (bEar) {	//这是一个耳朵
		pEar.ensure().ReadData(bs, isPtr24);
		pItemData = ::theApp.ItemMetaData(0x20726165);	//"ear "
	}
	else 		//这是一个物品,但是也可能为"ear "
		pItemData = pItemInfo.ensure().ReadData(bs, bSimple, bRuneWord, bPersonalized, bSocketed, isD2R, isPtr24);
	ASSERT(pItemData && bSimple == pItemData->Simple);
	bs.AlignByte();
	aGemItems.resize(Gems());
	for (auto& item : aGemItems)
		item.ReadData(bs, isD2R, isPtr24);
}

void CD2Item::WriteData(COutBitsStream& bs, BOOL isD2R, BOOL isPtr24) const {
	if (!vUnknownItem.empty()) {	//未识别物品数据
		bs << vUnknownItem;
	}
	else {
		if (!isD2R)
			bs << wMajic;
		bs << bQuest
			<< bits(iUNKNOWN_01, 3)
			<< bIdentified
			<< bits(iUNKNOWN_02, 3)
			<< BOOL(FALSE)		// Always reset bDisabled
			<< bits(iUNKNOWN_10, 2)
			<< bSocketed
			<< bits(iUNKNOWN_03, 2)
			<< bBadEquipped
			<< bUNKNOWN_04
			<< bEar
			<< bNewbie
			<< bits(iUNKNOWN_05, 3)
			<< bSimple
			<< bEthereal
			<< bUNKNOWN_06
			<< bPersonalized
			<< bUNKNOWN_07
			<< bRuneWord;
		if (isD2R)
			bs << bits(iUNKNOWN_09, 8);
		else
			bs << bits(iUNKNOWN_08, 5) << bits(wVersion, 10);
		bs << bits(iLocation, 3)
			<< bits(iPosition, 4)
			<< bits(iColumn, 4)
			<< bits(iRow, 4)
			<< bits(iStoredIn, 3);
		if (bEar) {	//这是一个耳朵
			pEar->WriteData(bs, isPtr24);
		}
		else		//这是一个物品
			pItemInfo->WriteData(bs, *pItemData, bSimple, bRuneWord, bPersonalized, bSocketed, isD2R, isPtr24);
	}
	bs.AlignByte();
	for (auto item : aGemItems)
		if (bs.Good())
			item.WriteData(bs, isD2R, isPtr24);
}

BOOL CD2Item::ReadFile(CFile& file) {
	CInBitsStream bs;
	bs.ReadFile(file);
	try {
		ReadData(bs, FALSE, FALSE);
	}
	catch (...) {
		return FALSE;
	}
	return TRUE;
}

void CD2Item::WriteFile(CFile& file) const {
	COutBitsStream bs;
	WriteData(bs, FALSE, FALSE);
	bs.WriteFile(file);
}

// struct CItemList

void CItemList::ReadData(CInBitsStream& bs, BOOL isD2R, BOOL isPtr24) {
	WORD nItems;
	bs >> wMajic >> nItems;
	if (wMajic != 0x4D4A)
		throw ::theApp.MsgBoxInfo(17);
	vItems.resize(nItems);
	for (auto& item : vItems) {
		if (!bs.Good())
			break;
		item.ReadData(bs, isD2R, isPtr24);
	}
}

void CItemList::WriteData(COutBitsStream& bs, BOOL isD2R, BOOL isPtr24) const {
	bs << WORD(0x4D4A) << WORD(vItems.size());
	const auto off = bs.BytePos();
	for (auto& item : vItems) {
		if (!bs.Good())
			break;
		item.WriteData(bs, isD2R, isPtr24);
	}
}
