#include "pch.h"
#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>

using namespace std;

typedef unsigned char	BYTE;
typedef unsigned short	WORD;
typedef unsigned long	DWORD;

struct ItemStruct
{
	union{
		DWORD	dwTypeID;
		BYTE	sTypeName[4];
	};
	WORD		wPic;
	WORD		wName;
	BYTE		bEqRange;
	BYTE		bAdd;
    BYTE        bDmg1Min,bDmg1Max;
    BYTE        bDmg2Min,bDmg2Max;
	static const int SIZE = 14;
};

class ItemData
{
	vector<BYTE>	vData;
	WORD			wCount;
public:
	ItemData():vData(16),wCount(0){}
	WORD Size() const{return wCount;}
	void AddData(const ItemStruct & item){
		vData.insert(vData.end(),(BYTE *)&item,(BYTE *)&item + ItemStruct::SIZE);
		++wCount;
	}
	void WriteFile(FILE * outf){
		assert(outf);
		*reinterpret_cast<WORD *>(&vData[0]) = 0x11DE;					//Magic
		*reinterpret_cast<DWORD *>(&vData[2]) = 0x4D455449;				//"ITEM"
		*reinterpret_cast<DWORD *>(&vData[6]) = DWORD(vData.size());	//File Length
		*reinterpret_cast<DWORD *>(&vData[10]) = 0;						//CRC
		*reinterpret_cast<WORD *>(&vData[14]) = wCount;					//Item Count
		//compute CRC
		const int STEP = 5;
		const int MASK = (1 << STEP) - 1;
		const int SHIFT = (sizeof(DWORD) << 3) - STEP;
		DWORD crc = 0;
		for(int i = int(vData.size()) - 1;i >= 0;--i){
			DWORD add = (crc & MASK) << SHIFT;
			crc = (crc << STEP) + vData[i];
		}
		*reinterpret_cast<DWORD *>(&vData[10]) = crc;
		//encode the data with CRC
		for(BYTE *cur = &vData[0] + 14, *end = &vData[0] + vData.size() - 4;cur < end;cur += STEP)
			*reinterpret_cast<DWORD *>(cur) ^= crc;
		//write to file
		fwrite(&vData[0],1,vData.size(),outf);
	}
};

int main(){
	cout<<sizeof(ItemStruct)<<endl;
	const wchar_t * input = L"E:\\My Documents\\Visual Studio 2005\\Projects\\暗黑II\\Diablo Edit2\\itemdata.txt";
	const char * output = "E:\\My Documents\\Visual Studio 2005\\Projects\\暗黑II\\Diablo Edit2\\itemdata.dat";
	ifstream inf(input);
	if(!inf.is_open()){
		cerr<<"cannot open input file\n";
		return 1;
	}
	const int BUF_SIZE = 255;
	char buf[BUF_SIZE];
    int pic_off = 0;
	try{
		//读取文本文件
		inf.getline(buf,BUF_SIZE);	//列表头
		ItemData vItem;
		ItemStruct item;
		while(!inf.eof()){
			memset(buf,0,BUF_SIZE);
			inf.getline(buf,BUF_SIZE);
            if(buf[0] == '*')   //注释
                continue;
			for(char * p = buf;*p;++p)
				if(*p == '\t')
					*p = 0;
			//ID
			memset(item.sTypeName,' ',4);
			char * cur = buf;
			for(int i = 0;i < 4 && *cur;++i,++cur)
				item.sTypeName[i] = *cur;
			//Pic
            bool minus = false;
            if(*++cur == '-'){
                minus = true;
                ++cur;
            }
			for(item.wPic = 0;*cur;++cur)
				item.wPic = item.wPic * 10 + *cur - '0';
            if(minus){
                pic_off += item.wPic;
                item.wPic = 0;
            }
			if(!item.wPic)
				item.wPic = vItem.Size() - pic_off;
			//Name
			for(item.wName = 0,++cur;*cur;++cur)
				item.wName = item.wName * 10 + *cur - '0';
            if(!item.wName)
				item.wName = vItem.Size();
			//Range
			item.bEqRange = 0;
			if(*++cur){
				item.bEqRange = ((*cur - '1') << 2);	//col
				if(*++cur)
					item.bEqRange += *cur++ - '1';	//row
			}
			//Equip(0-8)
			if(*++cur)
				item.bEqRange += *cur++ << 4;
			//bDef
			item.bAdd = 0;
			if(*++cur)
				item.bAdd += *cur++ != '0' ? 0x80 : 0;
			//bDur
			if(*++cur)
				item.bAdd += *cur++ != '0' ? 0x40 : 0;
			//bStacked
			if(*++cur)
				item.bAdd += *cur++ != '0' ? 0x20 : 0;
			//bTome
			if(*++cur)
				item.bAdd += *cur++ != '0' ? 0x10 : 0;
			//bMonster
			if(*++cur)
				item.bAdd += *cur++ != '0' ? 8 : 0;
			//bCharm
			if(*++cur)
				item.bAdd += *cur++ != '0' ? 4 : 0;
			//bSpell
			if(*++cur)
				item.bAdd += *cur++ != '0' ? 2 : 0;
			//bUnique
			if(*++cur)
				item.bAdd += *cur++ != '0' ? 1 : 0;
            //Dmg1Min
			for(item.bDmg1Min = 0,++cur;*cur;++cur)
				item.bDmg1Min = item.bDmg1Min * 10 + *cur - '0';
            //Dmg1Max
			for(item.bDmg1Max = 0,++cur;*cur;++cur)
				item.bDmg1Max = item.bDmg1Max * 10 + *cur - '0';
            //Dmg2Min
			for(item.bDmg2Min = 0,++cur;*cur;++cur)
				item.bDmg2Min = item.bDmg2Min * 10 + *cur - '0';
            //Dmg2Max
			for(item.bDmg2Max = 0,++cur;*cur;++cur)
				item.bDmg2Max = item.bDmg2Max * 10 + *cur - '0';
			//完成
			vItem.AddData(item);
		}
		//写数据文件
		FILE * outf = fopen(output,"wb");
		if(!outf){
			cerr<<"cannot open output file\n";
			return 0;
		}
		vItem.WriteFile(outf);
	}catch(...){
		system("pause");
	}
	return 0;
}
