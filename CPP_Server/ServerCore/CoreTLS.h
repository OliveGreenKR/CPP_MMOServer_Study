#pragma once

//threadid ������ ���� "TLS" extern����
extern thread_local uint32 LThreadId; 

extern thread_local std::stack<int32> LLocalStack;
