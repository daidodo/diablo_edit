#pragma once

#include <vector>
#include <string>
#include <cassert>

typedef unsigned long	DWORD;
typedef unsigned short	WORD;
typedef unsigned char	BYTE;
 
struct Message
{
	WORD				wMsgLen;
	std::vector<BYTE>	vContent;
//	explicit Message(string msg):wMsgLen(WORD(msg.length())),vContent(msg.begin(),msg.end()){}
};
struct Lang
{
	BYTE					bTitleLen;
	std::vector<BYTE>		vTitle;
	std::vector<Message>	vMsg;
};
class LangData
{
	std::vector<BYTE>	vData;
	WORD & Majic()		{return *reinterpret_cast<WORD *>(&vData[0]);}
	DWORD & Flags()		{return *reinterpret_cast<DWORD *>(&vData[2]);}
	DWORD & Count()		{return *reinterpret_cast<DWORD *>(&vData[6]);}
	DWORD & FileLen()	{return *reinterpret_cast<DWORD *>(&vData[10]);}
	DWORD & CRC()		{return *reinterpret_cast<DWORD *>(&vData[14]);}
public:
	LangData(DWORD lan_num,DWORD msg_num):vData(18){
		Majic() = 0x11DE;
		Flags() = 0x474E414C;
		Count() = (lan_num << 24) + (msg_num & 0xFFFFFF);
	}
	~LangData(){}
	void AddLangTitle(const std::string & title){
		BYTE len = BYTE(title.length());
		vData.push_back(len);
		vData.insert(vData.end(),title.begin(),title.end());
	}
	void AddMessage(const std::string & msg){
		WORD len = WORD(msg.length());
		BYTE * p = reinterpret_cast<BYTE *>(&len);
		vData.insert(vData.end(),p,p + 2);
		if(len)
			vData.insert(vData.end(),msg.begin(),msg.end());
	}
	void WriteFile(FILE * fp){
		assert(fp && "LangData::WriteFile(FILE * fp) const");
		FileLen() = DWORD(vData.size());
		//compute CRC
		const int STEP = 3;
		const int MASK = (1 << STEP) - 1;
		const int SHIFT = (sizeof(DWORD) << 3) - STEP;
		DWORD crc = CRC() = 0;
		for(int i = int(vData.size()) - 1;i >= 0;--i){
			DWORD add = (crc & MASK) << SHIFT;
			crc = (crc << STEP) + vData[i];
		}
		CRC() = crc;
		//encode the data with CRC
		for(BYTE *cur = &vData[0] + 18, *end = &vData[0] + vData.size() - 4;cur < end;cur += STEP)
			*reinterpret_cast<DWORD *>(cur) ^= crc;
		//write to file
		fwrite(&vData[0],1,vData.size(),fp);
	}
};
