#include "stdafx.h"
#include "BinDataStream.h"

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

CString CInBitsStream::ToString(DWORD len) const {
	CString ret;
	DWORD byte = bytes_;
	DWORD bit = bits_;
	while (len-- != 0 && byte < data_.size()) {
		ret = ((data_[byte] >> bit) & 1 ? _T("1") : _T("0")) + ret;
		if (++bit == 8) {
			bit = 0, ++byte;
			ret = _T(" ") + ret;
		}
	}
	return ret;
}

// class COutBitsStream
