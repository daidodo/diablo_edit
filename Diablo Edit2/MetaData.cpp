#include "stdafx.h"
#include "MetaData.h"
#include <numeric>
#include <cstdarg>

using namespace std;

//class CPropertyMetaData

CPropertyMetaData::CPropertyMetaData(vector<pair<int, int>> fields)
	: fields_(fields) {
	bitsSum_ = accumulate(fields_.begin(), fields_.end(), 0, [](auto s, auto f) { return s + f.first; });
}

vector<int> CPropertyMetaData::Parse(DWORD value) const {
	vector<int> ret;
	for (auto & f : fields_) {
		ret.push_back((value & ((DWORD(1) << f.first) - 1)) + f.second);
		value >>= f.first;
	}
	return ret;
}

//CSFormat

CString CSFormat(LPCTSTR lpszFormat, ...) {
	CString ret;
	va_list argList;
	va_start(argList, lpszFormat);
	ret.FormatV(lpszFormat, argList);
	va_end(argList);
	return ret;
}

