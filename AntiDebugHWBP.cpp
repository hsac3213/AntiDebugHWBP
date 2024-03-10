#include "AntiDebugHWBP.hpp"

bool AntiDebugHWBP::bInitialized = false;
CRITICAL_SECTION AntiDebugHWBP::csVecList{};
vector<AntiDebugHWBP*> AntiDebugHWBP::vecList{};

long AntiDebugHWBP::ExceptionHandler(PEXCEPTION_POINTERS e) {
	const PCONTEXT ctx = e->ContextRecord;
	const PEXCEPTION_RECORD er = e->ExceptionRecord;

	if (er->ExceptionCode == EXCEPTION_SINGLE_STEP) {
		EnterCriticalSection(&csVecList);
		if (!vecList.empty()) {
			for (AntiDebugHWBP* v : vecList) {
				if (v->tid == GetCurrentThreadId()) {
					if (v->values[0] == CHECK_VALUE_FIRST) {
						v->check_values[0] = CHECK_VALUE_SECOND;

						LeaveCriticalSection(&csVecList);
						return EXCEPTION_CONTINUE_EXECUTION;
					}
				}
			}
		}
		LeaveCriticalSection(&csVecList);
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

void AntiDebugHWBP::SetConsoleColor(int color) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, color);
}

#pragma optimize("", off)
void AntiDebugHWBP::Check(void) {
	EnterCriticalSection(&csVecList);
	if (!vecList.empty()) {
		for (AntiDebugHWBP* v : vecList) {
			if (v->tid == GetCurrentThreadId()) {
				v->values[0] = CHECK_VALUE_FIRST;

				if (v->check_values[0] != CHECK_VALUE_SECOND) {
					SetConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);	
					cout << "Debugger Found!" << endl;
					SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				}
				v->check_values[0] = 0;

				break;
			}
		}
	}
	LeaveCriticalSection(&csVecList);
}
#pragma optimize("", on)

AntiDebugHWBP::AntiDebugHWBP() {
	throw exception("Thread function must be identifed!");
}

AntiDebugHWBP::AntiDebugHWBP(void* pFunc) {
	if (!bInitialized) {
		InitializeCriticalSection(&csVecList);
		AddVectoredExceptionHandler(1, ExceptionHandler);
		HANDLE hThread = CreateThread(NULL, 0, NULL, NULL, CREATE_SUSPENDED, NULL);
		if (hThread) {
			CONTEXT ctx{};

			ctx.ContextFlags = CONTEXT_ALL;
			GetThreadContext(hThread, &ctx);
			ctx.Rcx = (UINT64)Check;
			SetThreadContext(hThread, &ctx);
			ResumeThread(hThread);
		}
		bInitialized = true;
	}

	HANDLE hThread = CreateThread(NULL, 0, NULL, NULL, CREATE_SUSPENDED, &this->tid);
	if (hThread) {
		CONTEXT ctx{};

		EnterCriticalSection(&csVecList);
		vecList.push_back(this);
		LeaveCriticalSection(&csVecList);

		ctx.ContextFlags = CONTEXT_ALL;
		GetThreadContext(hThread, &ctx);
		ctx.Rcx = (UINT64)pFunc;
		ctx.Dr0 = (UINT64)&this->values[0];
		ctx.Dr7 |= 1 << 0;
		ctx.Dr7 |= 1 << 1;
		ctx.Dr7 |= 1 << 16;
		ctx.Dr7 |= 2 << 18;

		cout << hex << ctx.Dr7 << endl;

		SetThreadContext(hThread, &ctx);
		ResumeThread(hThread);
	}
}