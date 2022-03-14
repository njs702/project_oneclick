#pragma comment(lib,"ws2_32.lib")  // winsock2를 사용하기 위한 lib를 추가합니다.
#include <stdio.h>
#include <WinSock2.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define DEFAULT_PORT "27015"
#define MY_LOCAL_IP "211.215.249.35"

// Client의 동작 순서
// 1. 소켓 생성 -> 2. 서버에 Connect -> 3. Send data -> 4. Recieve Reply

int main() {
	
	WSADATA wsaData;
	SOCKET clientSocket;
	struct sockaddr_in server;

	// initializing Winsock
	Sleep(2000);
	printf("Initializing Winsock...\n");
	// 1. Winsock 초기화, WINSOCK VERSION 2.2
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf("WSAStartup failed: %d\n", WSAStartup(MAKEWORD(2, 2), &wsaData));
		return 1;
	}
	else {
		Sleep(2000);
		printf("WSAStartup completed!!\n");
	}

	// 2. Creating socket
	// 2. 초기화 후 서버에서 사용할 SOCKET 인스턴스 생성
	// socket() function is used to create a socket
	if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		printf("Could not create socket : %d\n", WSAGetLastError());
	}
	else {
		Sleep(2000);
		printf("Socket has been created!\n");
		// Address Family : AF_INET ( = IPv4)
		// Type : SOCK_STREAM( = TCP Protocol connection)
		// Protocol : 0
	}

	server.sin_addr.s_addr = inet_addr(MY_LOCAL_IP); // 서버 IP
	server.sin_family = AF_INET; // IPv4
	server.sin_port = htons(9090); // 사용할 포트 번호 지정

	// 3. Connect to server
	if (connect(clientSocket, (struct sockaddr*)&server, sizeof(server)) < 0) {
		printf("Connect error!\n");
		return 1;
	}
	else {
		printf("Connection has been completed!\n");
	}

}