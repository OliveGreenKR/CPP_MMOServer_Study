#pragma once

#define WIN32_LEAN_AND_MEAN    // ���� ������ ���礤 ������ windows ������� �����Ѵ�.

#ifdef _DEBUG
#pragma comment(lib,"Debug\\ServerCore.lib")
#else
#pragma comment(lib,"Release\\ServerCore.lib")
#endif

#include "CorePch.h"

