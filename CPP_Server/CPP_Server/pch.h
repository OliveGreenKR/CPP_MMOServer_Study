#pragma once

#define WIN32_LEAN_AND_MEAN    // 거의 사용되지 안흔ㄴ 내용을 windows 헤더에서 제외한다.

#ifdef _DEBUG
#pragma comment(lib,"Debug\\ServerCore.lib")
#else
#pragma comment(lib,"Release\\ServerCore.lib")
#endif

#include "CorePch.h"

