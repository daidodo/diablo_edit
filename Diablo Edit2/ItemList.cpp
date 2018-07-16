#include "StdAfx.h"
#include "Diablo Edit2.h"
#include "ItemList.h"

CItemList::CItemList(){}

CItemList::~CItemList(){ClearItems();}

void CItemList::ReadData(CInBitsStream & bs)
{
    bs>>wMajic>>nItems;
	if (wMajic != 0x4D4A) {
		MessageBox(0, ::theApp.MsgBoxInfo(17), ::theApp.MsgError(), MB_ICONERROR);
		throw 0;
	}
    ClearItems();
    vpItems.resize(nItems);
    for(WORD i = 0;bs.Good() && i < nItems;++i){
        vpItems[i] = new CD2Item;
        vpItems[i]->ReadData(bs);
    }
    bs>>wEndMajic;
	if (wEndMajic != 0x4D4A) {
		MessageBox(0, ::theApp.MsgBoxInfo(17), ::theApp.MsgError(), MB_ICONERROR);
		throw 0;
	}
}

void CItemList::WriteData(COutBitsStream & bs) const {
	bs << WORD(0x4D4A);
	const auto off = bs.BytePos();
	WORD cnt = 0;
	bs << cnt;	//item number
	for (auto pItem : vpItems)
		if (pItem) {
			pItem->WriteData(bs);
			++cnt;
		}
	bs << WORD(0x4D4A) << offset_value(off, cnt);
}

void CItemList::ClearItems()
{
	for (auto p : vpItems)
		delete p;
	vpItems.clear();
}

