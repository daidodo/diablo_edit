#pragma once

#include <vector>
#include <map>
#include "ItemDataStruct.h"
#include "MayExist.h"
#include "BinDataStream.h"

template<int N>
static bool IsSameType(const BYTE(&v1)[N], const char * v2) {
	return memcmp(v1, v2, N) == 0;
}

//Ear
struct CEar
{
	BYTE	iEarClass;		//bit 76-78
	BYTE	iEarLevel;		//bit 79-85
	BYTE	sEarName[16];	//bit 86-92,93-99,100-106,...,以0x00结束
	void ReadData(CInBitsStream & bs);
	void WriteData(COutBitsStream & bs) const;
};

//Item Long Name
struct CLongName
{
	BYTE				iName1;		//8 bits,First Name
	BYTE				iName2;		//8 bits,Second Name
	BOOL				bPref1;		//1 bit,Prefix 1 flag
	CMayExist<WORD>		wPref1;		//11 bits,Prefix 1,if bPref1 == TRUE
	BOOL				bSuff1;		//1 bit,Suffix 1 flag
	CMayExist<WORD>		wSuff1;		//11 bits,Suffix 1,if bSuff1 == TRUE
	BOOL				bPref2;		//1 bit,Prefix 2 flag
	CMayExist<WORD>		wPref2;		//11 bits,Prefix 2,if bPref2 == TRUE
	BOOL				bSuff2;		//1 bit,Suffix 2 flag
	CMayExist<WORD>		wSuff2;		//11 bits,Suffix 2,if bSuff2 == TRUE
	BOOL				bPref3;		//1 bit,Prefix 3 flag
	CMayExist<WORD>		wPref3;		//11 bits,Prefix 3,if bPref3 == TRUE
	BOOL				bSuff3;		//1 bit,Suffix 3 flag
	CMayExist<WORD>		wSuff3;		//11 bits,Suffix 3,if bSuff3 == TRUE
	void ReadData(CInBitsStream & bs);
	void WriteData(COutBitsStream & bs) const;
};

//Extended Item Info
struct CExtItemInfo
{
	BYTE					nGems;			//bit 108-110, 如果有孔，镶嵌的宝石数
	DWORD					dwGUID;			//bit 111-142
	BYTE					iDropLevel;		//bit 143-149,drop level
	BYTE					iQuality;		/*4 bits
											1	low quality
											2	normal
											3	high quality(白色)
											4	magically enhanced(魔法,蓝色)
											5	part of a set(套装,绿色)
											6	rare(黄金)
											7	unique(暗金)
											8	crafted(合成，橙色)
											//*/
	BOOL					bVarGfx;		//1 bit
	CMayExist<BYTE>			iVarGfx;		//3 bits,如果bVarGfx == TRUE,此结构存在,似乎表示物品的图片索引
	BOOL					bClass;			//1 bit,Class Info Flag
	CMayExist<WORD>			wClass;			//11 bits,Class Info,如果bClass == TRUE,此结构存在
//Low Quality
	CMayExist<BYTE>			loQual;			//3 bits,Low Quality sub-type,如果iQuality == 1,此结构存在
//Hi Quality
	CMayExist<BYTE>			hiQual;			//3 bits,Hi Quality sub-type,如果iQuality == 3,此结构存在
//Magic
	CMayExist<WORD>			wPrefix;		//11 bits,Magic Prefix,if iQuality == 4
	CMayExist<WORD>			wSuffix;		//11 bits,Magic Suffix,if iQuality == 4
//Set
	CMayExist<WORD>			wSetID;			//12 bits,Set ID,if iQuality == 5
//Rare
	CMayExist<CLongName>	pRareName;		//if iQuality == 6
//Unique
	CMayExist<WORD>			wUniID;			//12 bits,Unique ID,if iQuality == 7
//Craft
	CMayExist<CLongName>	pCraftName;		//if iQuality == 8
//Rune Word
	CMayExist<WORD>			wRune;			//16 bits,Rune word,if bRuneWord == TRUE;
//Personalized
	CMayExistArray<BYTE,16>	sPersonName;	//和sEarName相同，每字符占7bits，以0x00结束,if bPersonalized == TRUE
//Tome
	CMayExist<BYTE>			iTome;			//5 bits,if sTypeName = "tbk " or "ibk "
//Monster ID
	CMayExist<WORD>			wMonsterID;		//10 bits,if sTypeName是身体器官
//Charm
	CMayExist<WORD>			wCharm;			//12 bits,if sTypeName == "cm1" || "cm2" || "cm3"
//Spell ID
	CMayExist<BYTE>			bSpellID;		//5 bits,if sTypeName == "0sc"
	BOOL IsSet() const { return iQuality == 5; }
	int Gems() const { return nGems; }
	void ReadData(CInBitsStream & bs, BOOL bIsCharm, BOOL bRuneWord, BOOL bPersonalized, BOOL bIsTome, BOOL bHasMonsterID, BOOL bHasSpellID);
	void WriteData(COutBitsStream & bs, BOOL bIsCharm, BOOL bRuneWord, BOOL bPersonalized, BOOL bIsTome, BOOL bHasMonsterID, BOOL bHasSpellID) const;
};

//Gold Quantity
struct CGoldQuantity
{
	BOOL	bNotGold;		//1 bit
	WORD	wQuantity;		//12 bits,黄金数量
	void ReadData(CInBitsStream & bs);
	void WriteData(COutBitsStream & bs) const;
};

struct CPropertyList
{
	std::map<WORD, DWORD>	mProperty;		//属性列表，每项（9 bits ID + VALUE)
	WORD					iEndFlag;		//9 bits, 0x1FF, 结束标志
	void ReadData(CInBitsStream & bs);
	void WriteData(COutBitsStream & bs) const;
};

//Type Specific info
struct CTypeSpecificInfo
{
	CMayExist<WORD>			iDefence;		//11 bits,实际防御值+10,if bHasDef = TRUE
	CMayExist<WORD>			iMaxDurability;	//8 bits,if bNoDurability == FALSE
	CMayExist<WORD>			iCurDur;		//9 bits,if bNoDurability == FALSE && iMaxDur > 0
	CMayExist<BYTE>			iSocket;		//4 bits,Base Number of Sockets,基础数目,后面还有个附加数目,if bSocketed = TRUE
	CMayExist<WORD>			iQuantity;		//9 bits,if bStacked == TRUE
	CMayExistArray<BOOL, 5>	aHasSetPropList;//5 bits,if iQuality == 5 
	//Rune Word Property
	CPropertyList			stPropertyList;		//属性列表
	CMayExist<CPropertyList> apSetProperty[5];	//套装属性列表，每个列表是否存在由(aHasSetPropList[i] == TRUE)决定

	void ReadData(CInBitsStream & bs, BOOL bHasDef, BOOL bHasDur, BOOL bSocketed, BOOL bIsStacked, BOOL bIsSet, BOOL bRuneWord);
	void WriteData(COutBitsStream & bs, BOOL bHasDef, BOOL bHasDur, BOOL bSocketed, BOOL bIsStacked, BOOL bIsSet, BOOL bRuneWord) const;
};

//ItemInfo
struct CItemInfo
{
	union{
		BYTE						sTypeName[4];	//bit 76-83,84-91,92-99,100-107
		DWORD						dwTypeID;
	};
	CMayExist<CExtItemInfo>			pExtItemInfo;	//如果bSimple == FALSE，则此结构存在
	CMayExist<CGoldQuantity>		pGold;			//如果sTypeName == "gld "，则此结构存在
	BOOL							bHasRand;		//1 bit
	CMayExistArray<DWORD, 3>		pTmStFlag;		//如果bHasRand == TRUE，则此结构存在
	CMayExist<CTypeSpecificInfo>	pTpSpInfo;		//如果bSimple == FALSE，则此结构存在
	const CItemDataStruct * ReadData(CInBitsStream & bs, BOOL bSimple, BOOL bRuneWord, BOOL bPersonalized, BOOL bSocketed);
	void WriteData(COutBitsStream & bs, const CItemDataStruct & itemData, BOOL bSimple, BOOL bRuneWord, BOOL bPersonalized, BOOL bSocketed) const;
	BOOL IsNameValid() const;
	BOOL IsSet() const { return pExtItemInfo.IsValid() && pExtItemInfo.Value().IsSet(); }
	int Gems() const { return pExtItemInfo.IsValid() && pExtItemInfo.Value().Gems(); }
};

class CD2Item
{
//members
public:
	~CD2Item();
    //Quality()只作显示时,控制名字的颜色用
	BYTE Quality() const{return !bEar && !bSimple ? pItemInfo->pExtItemInfo->iQuality : (pItemData->IsUnique ? 7 : 2);}
	void ReadData(CInBitsStream & bs);
	void WriteData(COutBitsStream & bs) const;
private:
	void findUnknownItem(CInBitsStream & bs);
public:
	std::vector<BYTE>		vItemData;	//如果不能识别物品,那么物品的数据将存在这里
	const CItemDataStruct *	pItemData;	//物品的额外属性,大小,bHasDef,bNoDurability,bStacked,等.如果不能识别物品,pItemData = 0;
	//物品信息
	WORD	wMajic;				//0x4D4A,"JM"
	BOOL	bQuest;				//bit 16,是否为系统装备
	BYTE	iUNKNOWN_01;		//bit 17-19
	BOOL	bIdentified;		//bit 20,是否已经辨识
	BYTE	iUNKNOWN_02;		//bit 21-23
	BOOL	bIllegalInventory;	//bit 24
	BYTE	iUNKNOWN_10;		//bit 25,26
	BOOL	bSocketed;			//bit 27,是否有孔
	BYTE	iUNKNOWN_03;		//bit 28,29
	BOOL	bBadEquipped;		//bit 30
	BOOL	iUNKNOWN_04;		//bit 31
	BOOL	bEar;				//bit 32
	BOOL	bNewbie;			//bit 33
	BYTE	iUNKNOWN_05;		//bit 34-36
	BOOL	bSimple;			//bit 37
	BOOL	bEthereal;			//bit 38
	BOOL	iUNKNOWN_06;		//bit 39,总是1
	BOOL	bPersonalized;		//bit 40
	BOOL	iUNKNOWN_07;		//bit 41
	BOOL	bRuneWord;			//bit 42
	BYTE	iUNKNOWN_08;		//bit 43-47
	WORD	wVersion;			//bit 48-57
	BYTE	iLocation;			//bit 58-60,0 = grid 1 = equipped 2 = on belt 3 = ? 4 = in hand(has been picked up by the mouse) 5 = ? 6 = socket(glued into a socket) 7 = ?
	BYTE	iPosition;			//bit 61-64,Body position, 01 = head(帽子), 02 = neck（项链）, 03 = tors（衣服）, 04 = rarm（右手，武器，盾）, 05 = larm（左手，武器，盾）, 06 = lrin（左手指，戒指）, 07 = rrin（右手指，戒指）, 08 = belt（腰带）, 09 = feet（鞋子）, 0a = glov（手套）, 0b = ralt（右II手，Expansion Set only）, 0c = lalt（左II手，Expansion Set only）
	BYTE	iColumn;			//bit 65-68
	BYTE	iRow;				//bit 69-72
	BYTE	iStoredIn;			//bit 73-75,0 = equip/belt 1 = inventory 2 = ? 3 = ? 4 = cube 5 = stash
	CMayExist<CEar>			pEar;			//如果bEar == TRUE，则此结构存在
	CMayExist<CItemInfo>	pItemInfo;		//如果bEar == FALSE，则此结构存在
	std::vector<CD2Item *>	aGemItems;		//如果有孔，镶嵌在孔里的装备
	//Functions
	BOOL IsSet() const { return pItemInfo.IsValid() && pItemInfo.Value().IsSet(); }
	BOOL Gems() const { return pItemInfo.IsValid() && pItemInfo.Value().Gems(); }
};
