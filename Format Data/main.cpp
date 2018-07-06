#include "pch.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

using namespace std;

//合并列
int main01()
{
	ifstream inf1("1.txt"),inf2("2.txt");
	if(!inf1.is_open() || !inf2.is_open()){
		cerr<<"error open file\n";
		return 0;
	}
	ofstream out("r.txt");
	char buf1[128],buf2[128];
	inf1.getline(buf1,128);
	while(strlen(buf1)){
		inf2.getline(buf2,128);
		out<<buf1<<"\t";
		if(strlen(buf2))
			out<<buf2<<endl;
		else
			out<<0<<endl;
		inf1.getline(buf1,128);
	}
	return 0;
}

//统一物品格式
int main()
{
	const char IN_FILE_NAME[] = "Weap.eng.txt";
	const char OUT_FILE_NAME[] = "Weap.txt";
	char buf[127];
	ifstream inf(IN_FILE_NAME);
	if(!inf.is_open()){
		cerr<<"error open file\n";
		return 0;
	}
	ofstream ouf(OUT_FILE_NAME);
	do{
		inf.getline(buf,127);
		ouf<<buf<<"\t\t1\n";
	}while(!inf.eof());


}

