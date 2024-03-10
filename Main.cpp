#include <iostream>

#include "AntiDebugHWBP.hpp"

using namespace std;

void ProtectedFunction() {
	while (1) {
		cout << "I'm running." << endl;
		AntiDebugHWBP::Check();
		Sleep(1000);
	}
}

int main()
{
	try {
		AntiDebugHWBP antiDebug(ProtectedFunction);
	}
	catch (exception e) {
		cout << "[ Exception ]" << endl;
		cout << e.what() << endl;
	}

	while (1) {
		Sleep(5000);
	}

	return 0;
}