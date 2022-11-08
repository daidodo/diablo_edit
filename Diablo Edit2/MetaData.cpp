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

CPropertyMetaDataItem::CPropertyMetaDataItem(DWORD verMin, const vector<CPropertyField> & fields, DWORD def)
	: fields_(fields)
	, def_(def)
	, verMin_(verMin)
{
	bitsSum_ = accumulate(fields_.begin(), fields_.end(), 0, [](auto s, auto f) { return s + f.bits; });
}

vector<int> CPropertyMetaDataItem::Parse(DWORD value) const {
	vector<int> ret;
	for (auto & f : fields_) {
		const auto MASK = (DWORD(1) << f.bits) - 1;
		ret.push_back((value & MASK) + f.base);
		value >>= f.bits;
	}
	return ret;
}

void CPropertyMetaDataItem::Normalise(std::vector<int>& params) const {
	params.resize(fields_.size());
	for (size_t i = 0; i < fields_.size(); ++i)
		params[i] -= fields_[i].base;
}


vector<tuple<int, int, int>> CPropertyMetaDataItem::GetParams(DWORD value) const {
	vector<tuple<int, int, int>> ret;
	for (auto & f : fields_) {
		if (f.bits < 1)
			break;
		ret.emplace_back(value & ((DWORD(1) << f.bits) - 1), f.min, f.max);
		value >>= f.bits;
	}
	return ret;
}

pair<BOOL, DWORD> CPropertyMetaDataItem::GetValue(const std::vector<int> & params) const {
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

// class CPropertyMetaData

void CPropertyMetaData::addData(const CPropertyMetaDataItem& item) {
	//保证元素按照verMin_递减排列
	auto wh = data_.begin();
	for (; wh != data_.end() && item.verMin_ < wh->verMin_; ++wh);
	data_.insert(wh, item);
}

const CPropertyMetaDataItem& CPropertyMetaData::findData(DWORD version) const {
	for (auto& item : data_)
		if (item.matchVersion(version))
			return item;
	ASSERT(!data_.empty());
	return data_.back();
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

