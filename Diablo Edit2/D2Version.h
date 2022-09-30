#pragma once

#include "stdafx.h"

inline BOOL IsD2R(DWORD dwVersion) { return dwVersion >= 0x61; }

inline BOOL IsPtr24AndAbove(DWORD dwVersion) { return dwVersion >= 0x62; }

inline BOOL IsPtr25AndAbove(DWORD dwVersion) { return dwVersion >= 0x63; }