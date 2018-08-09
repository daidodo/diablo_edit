#include "stdafx.h"
#include "MetaData.h"
#include <numeric>
#include <cstdarg>

using namespace std;

//struct CPropertyField

const CPropertyField & CPropertyField::Normalze() {
	ASSERT(0 <= bits && 0 <= min);
	if (0 == max)
		max = (1 << bits) - 1;
	ASSERT(min <= max);
	return *this;
}

//class CPropertyMetaData

CPropertyMetaData::CPropertyMetaData(const vector<CPropertyField> & fields, int def)
	: fields_(fields)
	, def_(def)
{
	bitsSum_ = accumulate(fields_.begin(), fields_.end(), 0, [](auto s, auto f) { return s + f.bits; });
}

vector<int> CPropertyMetaData::Parse(DWORD value) const {
	vector<int> ret;
	for (auto & f : fields_) {
		ret.push_back((value & ((DWORD(1) << f.bits) - 1)) + f.base);
		value >>= f.bits;
	}
	return move(ret);
}

vector<tuple<int, int, int>> CPropertyMetaData::GetParams(DWORD value) const {
	vector<tuple<int, int, int>> ret;
	for (auto & f : fields_) {
		if (f.bits < 1)
			break;
		ret.emplace_back(value & ((DWORD(1) << f.bits) - 1), f.min, f.max);
		value >>= f.bits;
	}
	return move(ret);
}

int CPropertyMetaData::GetValue(const std::vector<int> & params) const {
	int r = 0, i = -1, s = 0;
	for (auto & f : fields_) {
		if (f.bits < 1)
			break;
		if(++i >= int(params.size()))
			return -1;
		if (params[i] < f.min || f.max < params[i])
			return -1;
		r += params[i] << s;
		s += f.bits;
	}
	return r;
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

