#include "StdAfx.h"
#include "Diablo Edit2.h"
#include "ItemList.h"

CItemList::CItemList(){}

CItemList::~CItemList(){ClearItems();}

void CItemList::ReadData(CBinDataStream & bs)
{
    bs>>wMajic>>nItems;
    if(wMajic != 0x4D4A)
        if(MessageBox(0,::theApp.String(377),::theApp.String(5),MB_YESNO | MB_ICONWARNING) == IDNO)
            throw 0;
    ClearItems();
    vpItems.resize(nItems);
    for(WORD i = 0;i < nItems;++i){
        vpItems[i] = new CD2Item;
        vpItems[i]->ReadData(bs);
    }
    bs>>wEndMajic;
    if(wEndMajic != 0x4D4A)
        if(MessageBox(0,::theApp.String(377),::theApp.String(5),MB_YESNO | MB_ICONWARNING) == IDNO)
            throw 0;
}

void CItemList::ClearItems()
{
    if(!vpItems.empty()){
        for(std::vector<CD2Item *>::iterator it = vpItems.begin();it != vpItems.end();++it){
            if(*it){
                delete *it;
                *it = 0;
            }
        }
    }
}

