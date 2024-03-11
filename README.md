#사용방법
1. 아래 코드와 같이 스레드 생성.
```cpp
AntiDebugHWBP antiDebug(ptrFunction);
```
2. 보호할 함수 내에서 AntiDebugHWBP::Check(); 함수 실행.
[ 예시 ]
```cpp
void ProtectedFunction() {
	while (1) {
		cout << "I'm running." << endl;
		AntiDebugHWBP::Check(); // 디버거 감지
		Sleep(1000);
	}
}
```
