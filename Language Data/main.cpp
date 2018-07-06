#include "pch.h"
#include <iostream>
#include <fstream>
#include "LangData.h"

using namespace std;

bool GetLine(ifstream & inf,string & buf){
	buf.clear();
	bool ret = false;
	while(!inf.eof()){
		int ch = inf.get();
		if(ch == -1)
			break;
		if(ch == '\n'){
			if(!ret)
				continue;
			return true;
		}
		buf.push_back(ch);
		ret = true;
	}
	return ret;
}

template<typename T>
void WriteData(T data,FILE * fp){
	fwrite(&data,sizeof(T),1,fp);
}

template<typename T>
void WriteData(const vector<T> & data,FILE * fp){
	fwrite(&data[0],sizeof(T) * data.size(),1,fp);
}

int main(int argc,char ** argv)
{
	const wchar_t * input = L"lang.txt";
	const char * output = "language.dat";
	ifstream inf(input);
	if(!inf.is_open()){
		cerr<<"cannot open input file\n";
		return 1;
	}
	//读取文本文件
	DWORD lan_num,msg_num;
	try{
		if(!(inf>>lan_num>>msg_num)){	//头信息
			cerr<<"input file format error\n";
			throw 0;
		}
		LangData lang(lan_num,msg_num);
		string buf;
		for(DWORD i = 0;i < lan_num;++i){
			if(!GetLine(inf,buf)){	//语言名称
				cerr<<"input file format error for TITLE "<<(i + 1)<<endl;
				throw 0;
			}
			lang.AddLangTitle(buf);
			for(DWORD j = 1;j < msg_num;){
				if(!GetLine(inf,buf)){	//信息内容
					cerr<<"input file format error for MSG "<<(j + 1)<<" in TITLE "<<(i + 1)<<endl;
				throw 0;
				}
                if(buf[0] == '#')  //注释
                    continue;
				if(buf[0] == '*')	//一种新的信息类型
					buf.clear();
				lang.AddMessage(buf);
                ++j;
			}
		}
        cout<<buf<<endl;
		//system("pause");
		//写入2进制文件
		FILE * outf = 0;
		errno_t err = fopen_s(&outf, output, "wb");
		if(err != 0){
			cerr<<"cannot open output file\n";
			return 1;
		}
		lang.WriteFile(outf);
		fclose(outf);
	}catch(...){
		system("pause");
	}
	return 0;
}