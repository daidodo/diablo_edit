#include "StdAfx.h"
#include "Diablo Edit2.h"
#include "D2Item.h"
#include <fstream>

//CD2Item
//std::vector<BYTE>		CD2Item::SvBitFields;

CD2Item::CD2Item(){}

CD2Item::~CD2Item(){}

//void CD2Item::ReadPropertyFields()
//{
//	TCHAR	FILE_NAME[] = _T("E:\\My Documents\\Visual Studio 2005\\Projects\\暗黑II\\D2 Item Editor\\Properties.txt");
//	SvBitFields.resize(0x1FF);
//	std::ifstream inf(FILE_NAME);
//	for(int i,j;!inf.eof();){
//		if(inf>>i>>j)
//			SvBitFields[i] = j;
//	}
//}

void CD2Item::ReadData(CBinDataStream & bs)
{
	bs>>wMajic;
	if(wMajic != 0x4D4A)
		if(MessageBox(0,::theApp.String(378),::theApp.String(5),MB_YESNO | MB_ICONWARNING) == IDNO)
			throw 0;
	bs.ReadBit(bQuest);
	bs.ReadBits(iUNKNOWN_01,3);
	bs.ReadBit(bIdentified);
	bs.ReadBits(iUNKNOWN_02,3);
	bs.ReadBit(bIllegalInventory);
	bs.ReadBits(iUNKNOWN_10,2);
	bs.ReadBit(bSocketed);
	bs.ReadBits(iUNKNOWN_03,2);
	bs.ReadBit(bBadEquipped);
	bs.ReadBit(iUNKNOWN_04);
	bs.ReadBit(bEar);
	bs.ReadBit(bNewbie);
	bs.ReadBits(iUNKNOWN_05,3);
	bs.ReadBit(bSimple);
	bs.ReadBit(bEthereal);
	bs.ReadBit(iUNKNOWN_06);
	bs.ReadBit(bPersonalized);
	bs.ReadBit(iUNKNOWN_07);
	bs.ReadBit(bRuneWord);
	bs.ReadBits(iUNKNOWN_08,5);
	bs.ReadBits(wVersion,10);
	bs.ReadBits(iLocation,3);
	bs.ReadBits(iPosition,4);
	bs.ReadBits(iColumn,4);
	bs.ReadBits(iRow,4);
	bs.ReadBits(iStoredIn,3);
	if(bEar){	//这是一个耳朵
		bs.ReadBits(pEar->iEarClass,3);
		bs.ReadBits(pEar->iEarLevel,7);
		::ZeroMemory(pEar->sEarName,sizeof(pEar->sEarName));
		for(int i = 0;i < 16;++i){
			bs.ReadBits(pEar->sEarName[i],7);
			if(pEar->sEarName[i] == 0)
				break;
		}
        char buf[4] = {'e','a','r',' '};
        DWORD * type = reinterpret_cast<DWORD *>(buf);
        pItemData = ::theApp.ItemData(*type);
	}else{		//这是一个物品,但是也可能为"ear "
		for(int i = 0;i < 4;++i)
			bs.ReadBits(pItemInfo->sTypeName[i],8);
		if(!(pItemData = ::theApp.ItemData(pItemInfo->dwTypeID))){	//本程序不能识别此物品
			DWORD from = bs.BytePos() - 13;
			bs.AlignByte();
			BYTE J,M;
			bs>>J;
			for(bs>>M;J != 'J' || M != 'M';J = M,bs>>M);
			bs.SeekBack(2);
			bs.Restore(vItemData,from,bs.BytePos());
			{	//把未知的物品存储成物品文件
				CFile outf(::theApp.m_sAppPath + _T("unknown.d2i"),CFile::modeCreate | CFile::modeWrite);
				outf.Write(&vItemData[0],UINT(vItemData.size()));
			}
			CString msg;
			msg.Format(_T("(%c%c%c%c)"),pItemInfo->sTypeName[0],pItemInfo->sTypeName[1],pItemInfo->sTypeName[2],pItemInfo->sTypeName[3]);
			if(MessageBox(0,msg + ::theApp.String(380),::theApp.String(5),MB_YESNO | MB_ICONWARNING) == IDNO)
				throw 0;
		}
		if(!bSimple){	//物品有额外属性
			bs.ReadBits(pItemInfo->pExtItemInfo->nGems,3);
			bs.ReadBits(pItemInfo->pExtItemInfo->dwGUID,32);
			bs.ReadBits(pItemInfo->pExtItemInfo->iDropLevel,7);
			bs.ReadBits(pItemInfo->pExtItemInfo->iQuality,4);
			bs.ReadBit(pItemInfo->pExtItemInfo->bVarGfx);
			if(pItemInfo->pExtItemInfo->bVarGfx)
				bs.ReadBits(*pItemInfo->pExtItemInfo->iVarGfx,3);
			bs.ReadBit(pItemInfo->pExtItemInfo->bClass);
			if(pItemInfo->pExtItemInfo->bClass)
				bs.ReadBits(*pItemInfo->pExtItemInfo->wClass,11);
			switch(pItemInfo->pExtItemInfo->iQuality){
				case 1:			//low quality
					bs.ReadBits(*pItemInfo->pExtItemInfo->loQual,3);
					break;
				case 2:			//normal
                    if(pItemData->IsCharm())
                        bs.ReadBits(*pItemInfo->pExtItemInfo->wCharm,12);
                    break;
				case 3:			//high quality
					bs.ReadBits(*pItemInfo->pExtItemInfo->hiQual,3);
					break;
				case 4:			//magically enhanced
					bs.ReadBits(*pItemInfo->pExtItemInfo->wPrefix,11);
					bs.ReadBits(*pItemInfo->pExtItemInfo->wSuffix,11);
					break;
				case 5:			//part of a set
					bs.ReadBits(*pItemInfo->pExtItemInfo->wSetID,12);
					break;
				case 6:			//rare
					bs.ReadBits(pItemInfo->pExtItemInfo->pRareName->iName1,8);
					bs.ReadBits(pItemInfo->pExtItemInfo->pRareName->iName2,8);
					bs.ReadBit(pItemInfo->pExtItemInfo->pRareName->bPref1);
                    if(pItemInfo->pExtItemInfo->pRareName->bPref1)
					    bs.ReadBits(*pItemInfo->pExtItemInfo->pRareName->wPref1,11);
					bs.ReadBit(pItemInfo->pExtItemInfo->pRareName->bSuff1);
                    if(pItemInfo->pExtItemInfo->pRareName->bSuff1)
					    bs.ReadBits(*pItemInfo->pExtItemInfo->pRareName->wSuff1,11);
					bs.ReadBit(pItemInfo->pExtItemInfo->pRareName->bPref2);
                    if(pItemInfo->pExtItemInfo->pRareName->bPref2)
					    bs.ReadBits(*pItemInfo->pExtItemInfo->pRareName->wPref2,11);
					bs.ReadBit(pItemInfo->pExtItemInfo->pRareName->bSuff2);
                    if(pItemInfo->pExtItemInfo->pRareName->bSuff2)
					    bs.ReadBits(*pItemInfo->pExtItemInfo->pRareName->wSuff2,11);
					bs.ReadBit(pItemInfo->pExtItemInfo->pRareName->bPref3);
                    if(pItemInfo->pExtItemInfo->pRareName->bPref3)
					    bs.ReadBits(*pItemInfo->pExtItemInfo->pRareName->wPref3,11);
					bs.ReadBit(pItemInfo->pExtItemInfo->pRareName->bSuff3);
                    if(pItemInfo->pExtItemInfo->pRareName->bSuff3)
					    bs.ReadBits(*pItemInfo->pExtItemInfo->pRareName->wSuff3,11);
					break;
				case 7:			//unique
					bs.ReadBits(*pItemInfo->pExtItemInfo->wUniID,12);
					break;
				case 8:			//crafted
                    bs.ReadBits(pItemInfo->pExtItemInfo->pCraftName->iName1,8);
                    bs.ReadBits(pItemInfo->pExtItemInfo->pCraftName->iName2,8);
                    bs.ReadBit(pItemInfo->pExtItemInfo->pCraftName->bPref1);
                    if(pItemInfo->pExtItemInfo->pCraftName->bPref1)
                        bs.ReadBits(*pItemInfo->pExtItemInfo->pCraftName->wPref1,11);
                    bs.ReadBit(pItemInfo->pExtItemInfo->pCraftName->bSuff1);
                    if(pItemInfo->pExtItemInfo->pCraftName->bSuff1)
                        bs.ReadBits(*pItemInfo->pExtItemInfo->pCraftName->wSuff1,11);
                    bs.ReadBit(pItemInfo->pExtItemInfo->pCraftName->bPref2);
                    if(pItemInfo->pExtItemInfo->pCraftName->bPref2)
                        bs.ReadBits(*pItemInfo->pExtItemInfo->pCraftName->wPref2,11);
                    bs.ReadBit(pItemInfo->pExtItemInfo->pCraftName->bSuff2);
                    if(pItemInfo->pExtItemInfo->pCraftName->bSuff2)
                        bs.ReadBits(*pItemInfo->pExtItemInfo->pCraftName->wSuff2,11);
                    bs.ReadBit(pItemInfo->pExtItemInfo->pCraftName->bPref3);
                    if(pItemInfo->pExtItemInfo->pCraftName->bPref3)
                        bs.ReadBits(*pItemInfo->pExtItemInfo->pCraftName->wPref3,11);
                    bs.ReadBit(pItemInfo->pExtItemInfo->pCraftName->bSuff3);
                    if(pItemInfo->pExtItemInfo->pCraftName->bSuff3)
                        bs.ReadBits(*pItemInfo->pExtItemInfo->pCraftName->wSuff3,11);
                    break;
				default:
					CString msg;
					msg.Format(_T("iQuality = %d"),UINT(pItemInfo->pExtItemInfo->iQuality));
					::MessageBox(0,::theApp.String(381) + msg,0,MB_OK);
					throw 0;
			}
			if(bRuneWord)
				bs.ReadBits(*pItemInfo->pExtItemInfo->wRune,16);
			if(bPersonalized)
				for(int i = 0;i < 16;++i){
					bs.ReadBits(pItemInfo->pExtItemInfo->sPersonName[i],7);
					if(pItemInfo->pExtItemInfo->sPersonName[i] == 0)
						break;
				}
			if(pItemData->IsTome())
				bs.ReadBits(*pItemInfo->pExtItemInfo->iTome,5);
			else if(pItemData->HasMonsterID())
				bs.ReadBits(*pItemInfo->pExtItemInfo->wMonsterID,10);
			else if(pItemData->HasSpellID())
				bs.ReadBits(*pItemInfo->pExtItemInfo->bSpellID,5);
		}
		//特殊物品类型的额外数据
		if(IsSameType(pItemInfo->sTypeName,"gld ")){	//gld 的数量域
			bs.ReadBit(pItemInfo->pGold->bNotGold);
			bs.ReadBits(pItemInfo->pGold->wQuantity,12);
		}
		bs.ReadBit(pItemInfo->bHasRand);
		if(pItemInfo->bHasRand)
			for(int i = 0;i < 3;++i)
				bs.ReadBits(pItemInfo->pTmStFlag->dwUNKNOWN_[i],32);
		if(!bSimple){
			if(pItemData->HasDef())
				bs.ReadBits(*pItemInfo->pTpSpInfo->iDefence,11);
			if(pItemData->HasDur()){
				bs.ReadBits(*pItemInfo->pTpSpInfo->iMaxDurability,8);
				if(pItemInfo->pTpSpInfo->iMaxDurability.Value())
					bs.ReadBits(*pItemInfo->pTpSpInfo->iCurDur,9);
			}
			if(bSocketed)
				bs.ReadBits(*pItemInfo->pTpSpInfo->iSocket,4);
			if(pItemData->IsStacked())
				bs.ReadBits(*pItemInfo->pTpSpInfo->iQuantity,9);
			if(pItemInfo->pExtItemInfo->iQuality == 5){	//这是一个套装
				//暂时不支持.......................
			}
			if(bRuneWord){		//有符文之语属性
				//暂时不支持.......................
			}
			//接下来是额外属性列表
			for(bs.ReadBits(pItemInfo->pTpSpInfo->iEndFlag,9);
				pItemInfo->pTpSpInfo->iEndFlag < 0x1FF;
				bs.ReadBits(pItemInfo->pTpSpInfo->iEndFlag,9))
            {
				bs.ReadBits(pItemInfo->pTpSpInfo->mProperty[pItemInfo->pTpSpInfo->iEndFlag],
                    ::theApp.PropertyData(pItemInfo->pTpSpInfo->iEndFlag).bits);
            }
		}
	}
	bs.AlignByte();

}/*
		}
		if(!bSimple){
			pItemInfo->pTpSpInfo->bHasDef = TRUE;	//.......................
			if(pItemInfo->pTpSpInfo->bHasDef){
				pItemInfo->pTpSpInfo->iDefence = WORD(GETBITS(bit_pos,11));
			}
			pItemInfo->pTpSpInfo->bNoDurability = FALSE;	//.....................
			if(!pItemInfo->pTpSpInfo->bNoDurability){
				pItemInfo->pTpSpInfo->iMaxDurability = BYTE(GETBITS(bit_pos,8));
				if(pItemInfo->pTpSpInfo->iMaxDurability.Value())
					pItemInfo->pTpSpInfo->iCurDur = BYTE(GETBITS(bit_pos,9));	//............
			}
			if(bSocketed)
				pItemInfo->pTpSpInfo->iSocket = BYTE(GETBITS(bit_pos,4));
			pItemInfo->pTpSpInfo->bStacked = FALSE;	//.....................
			if(pItemInfo->pTpSpInfo->bStacked)
				pItemInfo->pTpSpInfo->iQuantity = BYTE(GETBITS(bit_pos,9));
			if(pItemInfo->pExtItemInfo->iQuality == 5){	//这是一个套装
				//暂时不支持.......................
			}
			if(bRuneWord){		//有符文之语属性
				//暂时不支持.......................
			}
			//接下来是额外属性列表
			if(SvBitFields.empty())
				ReadPropertyFields();
			for(WORD id = WORD(GETBITS(bit_pos,9));id < 0x1FF;id = WORD(GETBITS(bit_pos,9)))
		}
	}
CString left_bits = GetBitsString(bit_pos,32);
	return bit_pos >> 3;
}

CString CD2Item::GetBitsString(int from,int len)
{
	CString ret;
	int byte = from >> 3;
	from &= 7;
	while(len-- != 0){
		ret = ((bData[byte] >> from) & 1 ? _T("1") : _T("0")) + ret;
		if(++from == 8){
			from = 0,++byte;
			ret = _T(" ") + ret;
		}
	}
	return ret;
}


/*

//DWORD CD2Item::ExtractProperty(WORD id,int & pos)
//{
//	if(SvBitFields.empty())
//		ReadPropertyFields();
//	return 0;
//}


void CD2Item::MakeData()
{
	bData.resize(11);
	int bitpos = 0;
	SETBITS(bitpos,0x4D4A,sizeof(wMajic));
	SETBITS(bitpos,bQuest);
	SETBITS(bitpos = 20,bIdentified);
	SETBITS(bitpos = 27,bSocketed);
	SETBITS(bitpos = 30,bBadEquipped);
	SETBITS(bitpos = 32,bEar);
	SETBITS(bitpos,bNewbie);
	SETBITS(bitpos = 37,bSimple);
	SETBITS(bitpos,bEthereal);
	SETBITS(bitpos = 40,bPersonalized);
	SETBITS(bitpos = 42,bRuneWord);
	SETBITS(bitpos = 48,iVersion,8);
	SETBITS(bitpos = 58,iLocation,3);
	SETBITS(bitpos,iPosition,4);
	SETBITS(bitpos,iColumn,4);
	SETBITS(bitpos,iRow,4);
	SETBITS(bitpos,iStoredIn,3);
	if(bEar){
		pEar.Assert();
		SETBITS(bitpos,pEar->iEarClass,3);
		SETBITS(bitpos,pEar->iEarLevel,7);
		for(int i = 0;i < 15;++i){
			SETBITS(bitpos,pEar->sEarName[i],7);
			if(pEar->sEarName[i] == 0)
				break;
		}
	}else{
		pItemInfo.Assert();
		for(int i = 0;i < 4;++i)
			SETBITS(bitpos,pItemInfo->sTypeName[i],8);
		if(!bSimple){	//物品有额外属性
			pItemInfo->pExtItemInfo.Assert();
			SETBITS(bitpos,pItemInfo->pExtItemInfo->nGems,3);
			SETBITS(bitpos,pItemInfo->pExtItemInfo->dwGUID,32);
			SETBITS(bitpos,pItemInfo->pExtItemInfo->iDropLevel,7);
			SETBITS(bitpos,pItemInfo->pExtItemInfo->iQuality,4);
			SETBITS(bitpos,pItemInfo->pExtItemInfo->bVarGfx);
			if(pItemInfo->pExtItemInfo->bVarGfx)
				SETBITS(bitpos,pItemInfo->pExtItemInfo->iVarGfx.Value(),3);
			SETBITS(bitpos,pItemInfo->pExtItemInfo->bClass);
			if(pItemInfo->pExtItemInfo->bClass)
				SETBITS(bitpos,pItemInfo->pExtItemInfo->wClass.Value(),11);
			switch(pItemInfo->pExtItemInfo->iQuality){
				case 1:			//low quality
					SETBITS(bitpos,pItemInfo->pExtItemInfo->loQual.Value(),3);
					break;
				case 2:			//normal
					break;
				case 3:			//high quality
					SETBITS(bitpos,pItemInfo->pExtItemInfo->hiQual.Value(),3);
					break;
				case 4:			//magically enhanced
					SETBITS(bitpos,pItemInfo->pExtItemInfo->wPrefix.Value(),11);
					SETBITS(bitpos,pItemInfo->pExtItemInfo->wSuffix.Value(),11);
					break;
				case 5:			//part of a set
					SETBITS(bitpos,pItemInfo->pExtItemInfo->wSetID.Value(),12);
					break;
				case 6:			//rare
					pItemInfo->pExtItemInfo->pRareName.Assert();
					SETBITS(bitpos,pItemInfo->pExtItemInfo->pRareName->iName1,8);
					SETBITS(bitpos,pItemInfo->pExtItemInfo->pRareName->iName2,8);
					SETBITS(bitpos,pItemInfo->pExtItemInfo->pRareName->bPref1);
					SETBITS(bitpos,pItemInfo->pExtItemInfo->pRareName->wPref1.Value(),11);
					SETBITS(bitpos,pItemInfo->pExtItemInfo->pRareName->bSuff1);
					SETBITS(bitpos,pItemInfo->pExtItemInfo->pRareName->wSuff1.Value(),11);
					SETBITS(bitpos,pItemInfo->pExtItemInfo->pRareName->bPref2);
					SETBITS(bitpos,pItemInfo->pExtItemInfo->pRareName->wPref2.Value(),11);
					SETBITS(bitpos,pItemInfo->pExtItemInfo->pRareName->bSuff2);
					SETBITS(bitpos,pItemInfo->pExtItemInfo->pRareName->wSuff2.Value(),11);
					SETBITS(bitpos,pItemInfo->pExtItemInfo->pRareName->bPref3);
					SETBITS(bitpos,pItemInfo->pExtItemInfo->pRareName->wPref3.Value(),11);
					SETBITS(bitpos,pItemInfo->pExtItemInfo->pRareName->bSuff3);
					SETBITS(bitpos,pItemInfo->pExtItemInfo->pRareName->wSuff3.Value(),11);
					break;
				case 7:			//unique
					SETBITS(bitpos,pItemInfo->pExtItemInfo->wUniID.Value(),12);
					break;
				case 8:			//crafted
					pItemInfo->pExtItemInfo->pCraftName.Assert();
					SETBITS(bitpos,pItemInfo->pExtItemInfo->pCraftName->iName1,8);
					SETBITS(bitpos,pItemInfo->pExtItemInfo->pCraftName->iName2,8);
					SETBITS(bitpos,pItemInfo->pExtItemInfo->pCraftName->bPref1);
					SETBITS(bitpos,pItemInfo->pExtItemInfo->pCraftName->wPref1.Value(),11);
					SETBITS(bitpos,pItemInfo->pExtItemInfo->pCraftName->bSuff1);
					SETBITS(bitpos,pItemInfo->pExtItemInfo->pCraftName->wSuff1.Value(),11);
					SETBITS(bitpos,pItemInfo->pExtItemInfo->pCraftName->bPref2);
					SETBITS(bitpos,pItemInfo->pExtItemInfo->pCraftName->wPref2.Value(),11);
					SETBITS(bitpos,pItemInfo->pExtItemInfo->pCraftName->bSuff2);
					SETBITS(bitpos,pItemInfo->pExtItemInfo->pCraftName->wSuff2.Value(),11);
					SETBITS(bitpos,pItemInfo->pExtItemInfo->pCraftName->bPref3);
					SETBITS(bitpos,pItemInfo->pExtItemInfo->pCraftName->wPref3.Value(),11);
					SETBITS(bitpos,pItemInfo->pExtItemInfo->pCraftName->bSuff3);
					SETBITS(bitpos,pItemInfo->pExtItemInfo->pCraftName->wSuff3.Value(),11);
					break;
				default:
					CString msg;
					msg.Format(_T("未知的物品等级：iQuality = %d"),pItemInfo->pExtItemInfo->iQuality);
					::MessageBox(0,msg,0,MB_OK);
					ASSERT(FALSE);
					return;
			}
			if(bRuneWord)
				SETBITS(bitpos,pItemInfo->pExtItemInfo->wRune.Value(),16);
			if(bPersonalized)
				for(int i = 0;i < 15;++i){
					SETBITS(bitpos,pItemInfo->pExtItemInfo->sPersonName[i],7);
					if(pItemInfo->pExtItemInfo->sPersonName[i] == 0)
						break;
				}
			if(IsSameType(pItemInfo->sTypeName,"tbk ") ||
				IsSameType(pItemInfo->sTypeName,"ibk "))
				SETBITS(bitpos,pItemInfo->pExtItemInfo->iTome.Value(),5);
		}
		SETBITS(bitpos,pItemInfo->bHasRand);
		if(pItemInfo->bHasRand){
			pItemInfo->pTmStFlag.Assert();
			SETBITS(bitpos,pItemInfo->pTmStFlag->dwUNKNOWN_[0],32);
			SETBITS(bitpos,pItemInfo->pTmStFlag->dwUNKNOWN_[1],32);
			SETBITS(bitpos,pItemInfo->pTmStFlag->dwUNKNOWN_[2],32);
		}
		if(!bSimple){
			pItemInfo->pTpSpInfo.Assert();
			pItemInfo->pTpSpInfo->bHasDef = TRUE;	//.......................
			if(pItemInfo->pTpSpInfo->bHasDef)
				SETBITS(bitpos,pItemInfo->pTpSpInfo->iDefence.Value(),11);
			pItemInfo->pTpSpInfo->bNoDurability = FALSE;	//.....................
			if(!pItemInfo->pTpSpInfo->bNoDurability){
				SETBITS(bitpos,pItemInfo->pTpSpInfo->iMaxDurability.Value(),8);
				if(pItemInfo->pTpSpInfo->iMaxDurability.Value())
					SETBITS(bitpos,pItemInfo->pTpSpInfo->iCurDur.Value(),9);	//...........
			}
			if(bSocketed)
				SETBITS(bitpos,pItemInfo->pTpSpInfo->iSocket.Value(),4);
			pItemInfo->pTpSpInfo->bStacked = FALSE;	//.....................
			if(pItemInfo->pTpSpInfo->bStacked)
				SETBITS(bitpos,pItemInfo->pTpSpInfo->iQuantity.Value(),9);
			if(pItemInfo->pExtItemInfo->iQuality == 5){	//这是一个套装
				//暂时不支持.......................
			}
			if(bRuneWord){		//有符文之语属性
				//暂时不支持.......................
			}
			//接下来是额外属性列表
	//		if(SvBitFields.empty())
	//			ReadPropertyFields();
	//		for(WORD id = WORD(GETBITS(bit_pos,9));id < 0x1FF;id = WORD(GETBITS(bit_pos,9)))
	//			pItemInfo->pTpSpInfo->mProperty[id] = GETBITS(bit_pos,SvBitFields[id]);
		}
	}
}//*/

