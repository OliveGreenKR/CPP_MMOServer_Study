#pragma once

//threadid 관리를 위한 "TLS" extern변수
extern thread_local uint32 LThreadId; 

extern thread_local std::stack<int32> LLocalStack;
