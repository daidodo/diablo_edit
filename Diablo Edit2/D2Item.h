#pragma once

#include "MetaData.h"
#include "MayExist.h"
#include "BinDataStream.h"

#include <vector>

const int MAX_SOCKETS = 7;	//物品最大孔数，多于6个可能会导致游戏崩溃

//检查并设置人物姓名
BOOL SetCharName(BYTE (&dest)[16], const CString & src);

//Ear
struct CEar
{
	BYTE	iEarClass = 0;	//3 bits
	BYTE	iEarLevel = 1;	//7 bits
	BYTE	sEarName[16];	//7 bit * Count, 以0x00结束
	//Functions:
	explicit CEar(const char * name = 0);
	void ReadData(CInBitsStream & bs, BOOL isPtr24);
	void WriteData(COutBitsStream & bs, BOOL isPtr24) const;

};

//Item Long Name
struct CLongName
{
	BYTE				iName1;		//8 bits,First Name
	BYTE				iName2;		//8 bits,Second Name
	BOOL				bPref1;		//1 bit,Prefix 1 flag
	MayExist<WORD>		wPref1;		//11 bits,Prefix 1,if bPref1 == TRUE
	BOOL				bSuff1;		//1 bit,Suffix 1 flag
	MayExist<WORD>		wSuff1;		//11 bits,Suffix 1,if bSuff1 == TRUE
	BOOL				bPref2;		//1 bit,Prefix 2 flag
	MayExist<WORD>		wPref2;		//11 bits,Prefix 2,if bPref2 == TRUE
	BOOL				bSuff2;		//1 bit,Suffix 2 flag
	MayExist<WORD>		wSuff2;		//11 bits,Suffix 2,if bSuff2 == TRUE
	BOOL				bPref3;		//1 bit,Prefix 3 flag
	MayExist<WORD>		wPref3;		//11 bits,Prefix 3,if bPref3 == TRUE
	BOOL				bSuff3;		//1 bit,Suffix 3 flag
	MayExist<WORD>		wSuff3;		//11 bits,Suffix 3,if bSuff3 == TRUE
};

//Gold Quantity
struct CGoldQuantity
{
	BOOL	bNotGold = FALSE;	//1 bit
	WORD	wQuantity = 0;		//12 bits,黄金数量
};

struct CPropertyList
{
	std::vector<std::pair<WORD, DWORD>> mProperty;	//属性列表，每项（9 bits ID + VALUE)
	WORD					iEndFlag;				//9 bits, 0x1FF, 结束标志
	//Functons:
	int ExtSockets() const;			//属性列表里的额外孔数
	BOOL IsIndestructible() const;	//属性列表里的不可破坏属性
};

//Extended Item Info
struct CExtItemInfo
{
	BYTE					nGems = 0;		//bit 108-110, 如果有孔，镶嵌的宝石数
	DWORD					dwGUID;			//bit 111-142
	BYTE					iDropLevel = 99;//bit 143-149,drop level
	BYTE					iQuality = 2;	/*4 bits
												1	low quality
												2	normal
												3	high quality(白色)
												4	magically enhanced(魔法,蓝色)
												5	part of a set(套装,绿色)
												6	rare(黄金)
												7	unique(暗金)
												8	crafted(合成，橙色) */
	BOOL					bVarGfx = FALSE;//1 bit
	MayExist<BYTE>			iVarGfx;		//3 bits,如果bVarGfx == TRUE,此结构存在,似乎表示物品的图片索引
	BOOL					bClass = FALSE;	//1 bit,Class Info Flag
	MayExist<WORD>			wClass;			//11 bits,Class Info,如果bClass == TRUE,此结构存在
	//Low Quality
	MayExist<BYTE>			loQual;			//3 bits,Low Quality sub-type,如果iQuality == 1,此结构存在
	//Hi Quality
	MayExist<BYTE>			hiQual;			//3 bits,Hi Quality sub-type,如果iQuality == 3,此结构存在
	//Magic
	MayExist<WORD>			wPrefix;		//11 bits,Magic Prefix,if iQuality == 4
	MayExist<WORD>			wSuffix;		//11 bits,Magic Suffix,if iQuality == 4
	//Set
	MayExist<WORD>			wSetID;			//12 bits,Set ID,if iQuality == 5
	//Rare
	MayExist<CLongName>		pRareName;		//if iQuality == 6
	//Unique
	MayExist<WORD>			wUniID;			//12 bits,Unique ID,if iQuality == 7
	//Craft
	MayExist<CLongName>		pCraftName;		//if iQuality == 8
	//Rune Word
	MayExist<WORD>			wRune;			//16 bits,Rune word,if bRuneWord == TRUE;
	//Personalized
	MayExist<BYTE, 16>		sPersonName;	//和sEarName相同，每字符占7bits，以0x00结束,if bPersonalized == TRUE
	//Monster ID
	MayExist<WORD>			wMonsterID;		//10 bits,if sTypeName是身体器官
	//Charm
	MayExist<WORD>			wCharm;			//12 bits,if iQuality == 2 && IsCharm == TRUE
	//Spell ID
	MayExist<BYTE>			iSpellID;		//5 bits,if SpellId > 0
	//Functions:
	explicit CExtItemInfo(const CItemMetaData * meta = 0);
	BOOL IsSet() const { return iQuality == 5; }
	int RuneWordId() const { ASSERT(wRune.exist()); return (wRune & 0xFFF); }
	int Gems() const { return nGems; }
};

//Type Specific info
struct CTypeSpecificInfo
{
	MayExist<WORD>			iDefence;		//11 bits,实际防御值+10,if bHasDef = TRUE
	MayExist<WORD>			iMaxDurability;	//8 bits,if bNoDurability == FALSE
	MayExist<WORD>			iCurDur;		//9 bits,if iMaxDur > 0
	MayExist<BYTE>			iSocket;		//4 bits,基础孔数(0-6),属性列表里可以有附加孔数(总数不超过6),if bSocketed = TRUE
	MayExist<WORD>			iQuantity;		//9 bits,if bStacked == TRUE
	MayExist<BOOL, 5>		aHasSetPropList;//5 bits,if iQuality == 5 
	//Rune Word Property
	CPropertyList			stPropertyList;			//属性列表
	MayExist<CPropertyList> apSetProperty[5];		//套装属性列表，每个列表是否存在由(aHasSetPropList[i] == TRUE)决定
	MayExist<CPropertyList>	stRuneWordPropertyList;	//符文之语属性列表，if bRuneWord == TRUE
	//Functions:
	explicit CTypeSpecificInfo(const CItemMetaData * meta = 0);
	std::pair<int, int> Sockets() const;	//return: {base sockets, ext sockets}
	int TotalSockets() const { auto s = Sockets(); return s.first + s.second; }
	int GetDefence() const { ASSERT(iDefence.exist()); return iDefence - 10; }
	void SetDefence(int def) { iDefence.ensure() = def + 10; }
	BOOL IsIndestructible() const;
};

//ItemInfo
struct CItemInfo
{
	union{
		BYTE						sTypeName[4];	//bit 76-83,84-91,92-99,100-107
		DWORD						dwTypeID;
	};
	MayExist<CExtItemInfo>			pExtItemInfo;	//如果bSimple == FALSE，则此结构存在
	MayExist<CGoldQuantity>			pGold;			//如果sTypeName == "gld "，则此结构存在
	BOOL							bHasRand = FALSE;//1 bit
	MayExist<DWORD, 3>				pTmStFlag;		//如果bHasRand == TRUE，则此结构存在
	MayExist<CTypeSpecificInfo>		pTpSpInfo;		//如果bSimple == FALSE，则此结构存在
	//Functions:
	explicit CItemInfo(const CItemMetaData * meta = 0);
	const CItemMetaData * ReadData(CInBitsStream & bs, BOOL bSimple, BOOL bRuneWord, BOOL bPersonalized, BOOL bSocketed, BOOL isD2R, BOOL isPtr24);
	void WriteData(COutBitsStream & bs, const CItemMetaData & itemData, BOOL bSimple, BOOL bRuneWord, BOOL bPersonalized, BOOL bSocketed, BOOL isD2R, BOOL isPtr24) const;
	BOOL IsNameValid() const;
	BOOL IsSet() const { return pExtItemInfo.exist() && pExtItemInfo->IsSet(); }
	BOOL IsGold() const { return ::memcmp(sTypeName, "gld ", sizeof sTypeName) == 0; }
	BOOL IsBox() const { return ::memcmp(sTypeName, "box ", sizeof sTypeName) == 0; }			//是否赫拉迪卡方块
	int RuneWordId() const { ASSERT(pExtItemInfo.exist()); return pExtItemInfo->RuneWordId(); }
	int Gems() const { return (pExtItemInfo.exist() ? pExtItemInfo->Gems() : 0); }
	int Sockets() const { return (pTpSpInfo.exist() ? pTpSpInfo->TotalSockets() : 0); }
};

struct CD2Item
{
	WORD	wMajic = 0x4D4A;		//0x4D4A,"JM"
	BOOL	bQuest = FALSE;			//bit 16,是否为系统装备(从商店买的？)
	BYTE	iUNKNOWN_01 = 0;		//bit 17-19
	BOOL	bIdentified = TRUE;		//bit 20,是否已经辨识
	BYTE	iUNKNOWN_02 = 0;		//bit 21-23
	BOOL	bDisabled = FALSE;		//bit 24，物品不可用。比如耐久度变成0了，仅仅增加耐久度不够，还要重置这个bit
	BYTE	iUNKNOWN_10 = 0;		//bit 25,26
	BOOL	bSocketed = FALSE;		//bit 27,是否有孔
	BYTE	iUNKNOWN_03 = 0;		//bit 28,29
	BOOL	bBadEquipped = FALSE;	//bit 30
	BOOL	bUNKNOWN_04 = FALSE;	//bit 31
	BOOL	bEar = FALSE;			//bit 32
	BOOL	bNewbie = FALSE;		//bit 33
	BYTE	iUNKNOWN_05 = 0;		//bit 34-36
	BOOL	bSimple = TRUE;			//bit 37
	BOOL	bEthereal = FALSE;		//bit 38
	BOOL	bUNKNOWN_06 = TRUE;		//bit 39,总是1
	BOOL	bPersonalized = FALSE;	//bit 40
	BOOL	bUNKNOWN_07 = FALSE;	//bit 41
	BOOL	bRuneWord = FALSE;		//bit 42
	BYTE	iUNKNOWN_08 = 0;		//bit 43-47
	WORD	wVersion = 101;			//bit 48-57
	BYTE	iUNKNOWN_09 = 0xA0;		//bit 43-50 (D2R)
	BYTE	iLocation = 0;			//bit 58-60,0 = grid, 1 = equipped, 2 = on belt, 3 = ?, 4 = in hand(has been picked up by the mouse), 5 = ?, 6 = socket(glued into a socket), 7 = ?
	BYTE	iPosition = 0;			/*bit 61-64,Body position,
										01 = head(帽子)
										02 = neck（项链）
										03 = tors（衣服）
										04 = rarm（右手，武器，盾）
										05 = larm（左手，武器，盾）
										06 = lrin（左手指，戒指）
										07 = rrin（右手指，戒指）
										08 = belt（腰带）
										09 = feet（鞋子）
										0a = glov（手套）
										0b = ralt（右II手，Expansion Set only）
										0c = lalt（左II手，Expansion Set only）*/
	BYTE	iColumn = 0;			//bit 65-68
	BYTE	iRow = 0;				//bit 69-72
	BYTE	iStoredIn = 1;			//bit 73-75,0 = equip/belt, 1 = inventory, 2 = ?, 3 = ?, 4 = cube, 5 = stash
	MayExist<CEar>			pEar;		//如果bEar == TRUE，则此结构存在
	MayExist<CItemInfo>		pItemInfo;	//如果bEar == FALSE，则此结构存在
	std::vector<CD2Item>	aGemItems;	//如果有孔，镶嵌在孔里的装备
	//Functions
	explicit CD2Item(DWORD type = 0);
	const CItemMetaData & MetaData() const { return *pItemData; }
	BYTE Quality() const{return !bEar && !bSimple ? pItemInfo->pExtItemInfo->iQuality : (pItemData->IsUnique ? 7 : 2);}
	BOOL IsSet() const { return pItemInfo.exist() && pItemInfo->IsSet(); }
	BOOL IsRuneWord() const { return bRuneWord; }
	BOOL IsEditable() const { return TRUE; }
	int RuneWordId() const { ASSERT(IsRuneWord() && pItemInfo.exist()); return pItemInfo->RuneWordId(); }
	int Gems() const { return (pItemInfo.exist() ? pItemInfo->Gems() : 0); }
	int Sockets() const { return (bSocketed && pItemInfo.exist() ? pItemInfo->Sockets() : 0); }	//物品的孔数总和（包括属性增加的孔）
	CString ItemName() const;
	int GemIndexMax() const;	//镶嵌宝石位置索引最大值，没有返回-1
	BOOL IsBox() const { return pItemInfo.exist() && pItemInfo->IsBox(); }	//是否赫拉迪卡方块
	BOOL HasPropertyList() const { return pItemInfo.exist() && pItemInfo->pTpSpInfo.exist(); }
	void ReadData(CInBitsStream & bs, BOOL isD2R, BOOL isPtr24);
	void WriteData(COutBitsStream & bs, BOOL isD2R, BOOL isPtr24) const;
	BOOL ReadFile(CFile & file);
	void WriteFile(CFile & file) const;
private:
	std::vector<BYTE>		vUnknownItem;	//如果不能识别物品,那么物品的数据将存在这里
	const CItemMetaData *	pItemData;		//物品的额外属性,大小,bHasDef,bNoDurability,bStacked,等.如果不能识别物品,pItemData = 0;
};

struct CItemList
{
	//人物物品信息
	WORD		wMajic;				//0x4D4A,"JM"
	//WORD		nItems;				//物品数目
	std::vector<CD2Item> vItems;	//所有物品，不包括镶嵌在孔里的
	WORD		wEndMajic;			//0x4D4A,"JM"
	//Functions:
	void SwapItems(CItemList & list) { vItems.swap(list.vItems); }
	void Reset() { vItems.clear(); }
	void ReadData(CInBitsStream & bs, BOOL isD2R, BOOL isPtr24);
	void WriteData(COutBitsStream & bs, BOOL isD2R, BOOL isPtr24) const;
};
