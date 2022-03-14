#pragma comment(lib,"ws2_32.lib")  // winsock2를 사용하기 위한 lib를 추가합니다.
#include <stdio.h>
#include <WinSock2.h>

// 윈도우 소켓의 가장 기본적인 흐름
// Winsock 초기화 -> socket 생성 -> network 통신 -> socket 닫기 -> Winsock 종료

int main() {

	//WSADATA 구조체에는 Windows 소켓 구현에 대한 정보가 포함되어 있음
	WSADATA wsaData;

	// 1. Winsock 초기화, WINSOCK VERSION 2.2
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf("WSAStartup failed: %d\n", WSAStartup(MAKEWORD(2, 2), &wsaData));
		return 1;
	}
	else {
		printf("WSAStartup completed!!\n");
	}

	// 2. 초기화 후 서버에서 사용할 SOCKET 인스턴스 생성

}