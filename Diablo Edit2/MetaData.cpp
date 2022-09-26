#include "stdafx.h"
#include "MetaData.h"
#include <numeric>
#include <cstdarg>

using namespace std;

//struct CPropertyField

const CPropertyField & CPropertyField::Normalize() {
	ASSERT(0 <= bits && 0 <= min);
	if (0 == max)
		max = (1 << bits) - 1;
	ASSERT(min <= max);
	return *this;
}

//class CPropertyMetaData

CPropertyMetaData::CPropertyMetaData(const vector<CPropertyField> & fields, DWORD def)
	: fields_(fields)
	, def_(def)
{
	bitsSum_ = accumulate(fields_.begin(), fields_.end(), 0, [](auto s, auto f) { return s + f.bits; });
}

vector<int> CPropertyMetaData::Parse(DWORD value) const {
	vector<int> ret;
	for (auto & f : fields_) {
		ret.push_back((value & f.max) + f.base);
		value >>= f.bits;
	}
	return ret;
}

vector<tuple<int, int, int>> CPropertyMetaData::GetParams(DWORD value) const {
	vector<tuple<int, int, int>> ret;
	for (auto & f : fields_) {
		if (f.bits < 1)
			break;
		ret.emplace_back(value & ((DWORD(1) << f.bits) - 1), f.min, f.max);
		value >>= f.bits;
	}
	return ret;
}

pair<BOOL, DWORD> CPropertyMetaData::GetValue(const std::vector<int> & params) const {
	int i = 0, s = 0;
	DWORD r = 0;
	for (auto & f : fields_) {
		DWORD v = i < int(params.size()) ? params[i] : 0;
		if (int(v) < f.min || f.max < int(v))
			return make_pair(FALSE, 0);
		r += v << s;
		s += f.bits;
		++i;
	}
	return make_pair(TRUE, r);
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

