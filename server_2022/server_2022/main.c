#pragma comment(lib,"ws2_32.lib")  // winsock2를 사용하기 위한 lib를 추가합니다.
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <string.h>
#include <io.h>
#define DEFAULT_PORT "27015"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

// 윈도우 소켓의 가장 기본적인 흐름
// Winsock 초기화 -> socket 생성 -> network 통신 -> socket 닫기 -> Winsock 종료

int main() {

	//WSADATA 구조체에는 Windows 소켓 구현에 대한 정보가 포함되어 있음
	WSADATA wsaData;
	SOCKET serverSocket;
	SOCKET newSocket;
	struct sockaddr_in server;
	struct sockaddr_in client;
	char *message = NULL;
	int c;

	// 1. Winsock 초기화, WINSOCK VERSION 2.2
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf("WSAStartup failed: %d\n", WSAStartup(MAKEWORD(2, 2), &wsaData));
		return 1;
	}
	else {
		Sleep(2000);
		printf("WSAStartup completed!!\n");
	}

	// 2. 초기화 후 서버에서 사용할 SOCKET 인스턴스 생성
	// socket() function is used to create a socket
	if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		printf("Could not create socket : %d\n", WSAGetLastError());
	}
	else {
		Sleep(2000);
		printf("Socket has been created!\n");
		// Address Family : AF_INET ( = IPv4)
		// Type : SOCK_STREAM( = TCP Protocol connection)
		// Protocol : 0
	}

	//server.sin_addr.s_addr = INADDR_ANY; // 자동으로 이 컴퓨터에 존재하는 랜카드 중 사용가능한 랜카드의 IP주소 사용
	server.sin_addr.s_addr = inet_addr("211.215.249.35");
	server.sin_family = AF_INET; // IPv4
	server.sin_port = htons(8888); // 사용할 포트 번호 지정

	// 3. Socket 생성 후 Bind
	if (bind(serverSocket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
		printf("Bind failed with error code : %d\n", WSAGetLastError());
	}
	else {
		Sleep(2000);
		printf("Bind done!\n");
	}

	// 4. Socket에서 Listen을 통해 수신 대기
	/*Sleep(2000);
	listen(serverSocket, SOMAXCONN);
	printf("Waiting for incoming connections...\n");*/

	if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
		printf("Listen failed with error: %ld\n", WSAGetLastError());
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}
	else {
		Sleep(2000);
		printf("Waiting for incoming connections...\n");
	}

	// 5. Client의 연결 요청을 accept하기
	c = sizeof(struct sockaddr_in);
	newSocket = INVALID_SOCKET;
	
	while ((newSocket = accept(serverSocket, NULL, NULL))!=INVALID_SOCKET) {
		printf("Connection accepted!\n");
		char* recv_buf = malloc(sizeof(char)*100);
		//message = malloc(sizeof(char) * 1000);
		int recv_size;
		// 5.1 Client로부터 온 Data Read
		recv_size = recv(newSocket, recv_buf, strlen(recv_buf), 0);
		recv_buf[recv_size] = '\0';
		printf("Message from Client : %s\n", recv_buf);

		// 6. Reply to Client
		//strcpy(message, recv_buf);
		message = "Hello client! ";
		send(newSocket, message, strlen(message), 0);
		free(recv_buf);
		//free(message);
	}
	if (newSocket == INVALID_SOCKET) {
		printf("accept failed: %d\n", WSAGetLastError());
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}


	closesocket(newSocket);
	closesocket(serverSocket);
	WSACleanup();

}