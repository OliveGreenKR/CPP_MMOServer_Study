#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "DeadLockProfiler.h"
#include "Memory.h"

ThreadManager*		GThreadManager = nullptr;
DeadLockProfiler*	GDeadLockProfiler = nullptr;
MemoryManager*		GMemory = nullptr;

class CoreGlobal
{
public:
	CoreGlobal()
	{
		GThreadManager = new ThreadManager();
		GMemory = new MemoryManager();
		GDeadLockProfiler = new DeadLockProfiler();
	}
	~CoreGlobal()
	{
		delete GThreadManager;
		delete GMemory;
		delete GDeadLockProfiler;
	}
} GCoreGlobal;
