#include "pch.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <vector>

using namespace std;

typedef unsigned char	BYTE;
typedef unsigned short	WORD;
typedef unsigned long	DWORD;

class CProperty
{
    vector<BYTE> vData_;
    WORD maxID_;
public:
    CProperty():vData_(16),maxID_(0){}
    void AddProp(WORD id,int bits,int bias){
        if(bits){
            if(id > maxID_)
                maxID_ = id;
            vData_.insert(vData_.end(),(BYTE *)&id,(BYTE *)&id + sizeof id);
            vData_.push_back(bits);
            vData_.push_back(bias);
        }
    }
    void WriteFile(FILE * outf){
		assert(outf);
		*(WORD *)(&vData_[0]) = 0x11DE;					//Magic
		*(DWORD *)(&vData_[2]) = 0x504F5250;			//"PROP"
		*(DWORD *)(&vData_[6]) = DWORD(vData_.size());	//File Length
		*(DWORD *)(&vData_[10]) = 0;					//CRC
		*(WORD *)(&vData_[14]) = maxID_;				//Max ID
		//compute CRC
		const int STEP = 5;
		const int MASK = (1 << STEP) - 1;
		const int SHIFT = (sizeof(DWORD) << 3) - STEP;
		DWORD crc = 0;
		for(int i = int(vData_.size()) - 1;i >= 0;--i){
			DWORD add = (crc & MASK) << SHIFT;
			crc = (crc << STEP) + vData_[i];
		}
		*(DWORD *)(&vData_[10]) = crc;
		//encode the data with CRC
		for(BYTE *cur = &vData_[0] + 14, *end = &vData_[0] + vData_.size() - 4;cur < end;cur += STEP)
			*(DWORD *)(cur) ^= crc;
		//write to file
		fwrite(&vData_[0],1,vData_.size(),outf);
    }
};

int main()
{
    const wchar_t * input = L"property.txt";
    const char * output = "property.dat";
	ifstream inf(input);
	if(!inf.is_open()){
		cerr<<"cannot open input file\n";
		system("pause");
        return 1;
	}
	const int BUF_SIZE = 100;
	char buf[BUF_SIZE];
	try{
		//读取文本文件
		inf.getline(buf,BUF_SIZE);	//列表头
        CProperty prop;
        while(!inf.eof()){
		    inf.getline(buf,BUF_SIZE);	//列表头
            istringstream iss(buf);
            int id,bits = 0,bias = 0;
            iss>>id>>bits>>bias;
            prop.AddProp(id,bits,bias);
        }
        //写数据文件
		FILE * outf = 0;
		errno_t err = fopen_s(&outf, output, "wb");
		if(err != 0){
			cerr<<"cannot open output file\n";
		    system("pause");
			return 1;
		}
        prop.WriteFile(outf);
    }catch(...){
		system("pause");
    }
    return 0;
}
