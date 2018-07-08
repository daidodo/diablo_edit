#pragma once

#include "D2Item.h"

struct CItemList
{
//members
	CItemList();
	~CItemList();
	void ReadData(CInBitsStream & bs);
private:
	void ClearItems();
//fields
public:
	//人物物品信息
	WORD		wMajic;		//0x4D4A,"JM"
	WORD		nItems;		//物品数目
	std::vector<CD2Item *>	vpItems;
	WORD		wEndMajic;	//0x4D4A,"JM"
};
