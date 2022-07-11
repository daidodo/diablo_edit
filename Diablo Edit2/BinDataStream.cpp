#include "stdafx.h"
#include "BinDataStream.h"

using namespace std;

void CopyBits(const BYTE * from, BYTE * to, DWORD fromOff, DWORD toOff, DWORD len) {
	ASSERT(from && to);
	for (; len > 0;) {
		from += fromOff / 8;
		fromOff %= 8;
		to += toOff / 8;
		toOff %= 8;
		auto bits = min(min(len, 8 - fromOff), 8 - toOff);
		*to &= (BYTE(1) << toOff) - 1;
		*to |= ((*from >> fromOff) & ((BYTE(1) << bits) - 1)) << toOff;
		fromOff += bits;
		toOff += bits;
		len -= bits;
	}
}

// class CInBitsStream

string CInBitsStream::ToString(DWORD len) const {
	DWORD byte = bytes_;
	DWORD bit = bits_;
	string ret;
	while (len-- != 0 && byte < data_.size()) {
		ret.push_back(((data_[byte] >> bit) & 1) + '0');
		if (++bit == 8) {
			bit = 0, ++byte;
			ret.push_back(' ');
		}
	}
	if (!ret.empty()) {
		if (ret.back() == ' ')
			ret.pop_back();
		reverse(ret.begin(), ret.end());
	}
	return ret;
}

// class COutBitsStream
