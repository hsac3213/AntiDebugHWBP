#include <Windows.h>
#include <iostream>
#include <vector>

const UINT64 CHECK_VALUE_FIRST = 123;
const UINT64 CHECK_VALUE_SECOND = 456;

using namespace std;

class AntiDebugHWBP {
private:
	static bool bInitialized;
	static CRITICAL_SECTION csVecList;
	static vector<AntiDebugHWBP*> vecList;

	DWORD tid = NULL;
	UINT64 values[4]{};
	UINT64 check_values[4]{};

	static long ExceptionHandler(PEXCEPTION_POINTERS e);
	static void SetConsoleColor(int color);
public:
	static void Check();

	AntiDebugHWBP();
	AntiDebugHWBP(void* pFunc);
};