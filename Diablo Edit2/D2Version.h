#pragma once

#include "stdafx.h"

inline BOOL IsD2R(DWORD dwVersion) { return dwVersion >= 0x61; }

inline BOOL IsPtr24AndAbove(DWORD dwVersion) { return dwVersion >= 0x62; }

inline BOOL IsPtr25AndAbove(DWORD dwVersion) { return dwVersion >= 0x63; }

inline BOOL IsValidVersion(DWORD dwVersion) {
	switch (dwVersion)
	{
		case 0x63:
		case 0x62:
		case 0x61:
		case 0x60:
		case 0x5C:
		case 0x59:
		case 0x57:
		case 0x47:return TRUE;
		default:
			return FALSE;
	}
}