#pragma comment(lib,"ws2_32.lib")  // winsock2를 사용하기 위한 lib를 추가합니다.
#include <stdio.h>
#include <WinSock2.h>
#include <process.h>
#include <string.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9090
#define MAX_CLIENTS 20

HANDLE hMutex; // 뮤텍스 선언

int ret = 0;
SOCKET clientSocket = 0;
// 클라이언트 초기화 및 서버 접속
int client_init() {
	WSADATA wsaData;
	SOCKET s;
	struct sockaddr_in server;

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
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		printf("Could not create socket : %d\n", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	else {
		Sleep(2000);
		printf("Socket has been created!\n");
		// Address Family : AF_INET ( = IPv4)
		// Type : SOCK_STREAM( = TCP Protocol connection)
		// Protocol : 0
	}

	//server.sin_addr.s_addr = INADDR_ANY; // 자동으로 이 컴퓨터에 존재하는 랜카드 중 사용가능한 랜카드의 IP주소 사용
	server.sin_addr.s_addr = inet_addr(SERVER_IP);
	server.sin_family = AF_INET; // IPv4
	server.sin_port = htons(SERVER_PORT); // 사용할 포트 번호 지정

	// 3. Connect to server
	if (connect(s, (struct sockaddr*)&server, sizeof(server)) < 0) {
		printf("Connect error!\n");
		return 1;
	}
	else {
		printf("Connection has been completed!\n");
	}

	return s;
}

// 메시지 받는 스레드 함수
void recv_thread(void* pData) {
	int ret_thread = 65535;
	char buff_thread[1024] = { 0 };

	// 스레드용 리턴 값이 원하는 값이 아니면 받는 중에 서버와 통신이 끊긴 것
	while (ret_thread != INVALID_SOCKET || ret_thread != SOCKET_ERROR) {
		Sleep(10);

		// 서버에서 주는 메시지를 실시간으로 기다렸다가 받는다.
		ret_thread = recv(clientSocket, buff_thread, sizeof(buff_thread), 0);

		// 서버에서 받는 작업을 한 결과 비정상이면 탈출
		if (ret_thread == INVALID_SOCKET || ret_thread == SOCKET_ERROR) {
			break;
		}

		// 정상적으로 받는다면 받은 버퍼 출력
		printf("\nmessage recieve : %s", buff_thread);
		memset(buff_thread, 0, 1024); // 버퍼 초기화
	}

	// 작업 끝난 소켓 초기화
	WaitForSingleObject(hMutex, 100L);
	ret = INVALID_SOCKET;
	ReleaseMutex(hMutex);
	return;
}

int main() {
	// 뮤텍스 초기화
	struct sockaddr_in client;
	hMutex = CreateMutex(NULL, FALSE, NULL);
	char buff[1024];
	int recv_size;

	if (!hMutex) {
		printf("Mutex create error\n");
		return 1;
	}

	clientSocket = client_init();
	if (clientSocket == SOCKET_ERROR) {
		printf("socket init error\n");
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

	// 3.5 Receive welcome message from server
	if ((ret = recv(clientSocket, buff, 1024, 0)) == SOCKET_ERROR) {
		printf("Receive failed!\n");
	}
	else {
		printf("Welcome message from server : \n");
		buff[ret] = '\0';
		puts(buff);
	}

	if (!strcmp("Client Full!\n", buff)) {
		closesocket(clientSocket);
		WSACleanup();
		return 0;
	}

	// 정상 접속이 되면 스레드 작동 - 받는 메시지 스레드 실시간 수행
	_beginthread(recv_thread, 0, NULL);

	// 보내는 메시지는 스레드로 넣을 필요가 없음
	// 전송 결과 잘못된 값을 얻으면 탈출
	while (ret != INVALID_SOCKET || ret != SOCKET_ERROR) {
		Sleep(10);
		printf("보낼 메시지 입력 : ");
		fgets(buff, 1024, stdin);
		if (strcmp(buff, "#exit") == 0) {
			break;
		}

		// 서버로 보내는 글은 fgets를 받고 나서 순차적으로 보냄
		ret = send(clientSocket, buff, strlen(buff), 0);
		
		memset(buff, 0, 1024);
	}
	printf("서버와 연결이 끊어졌습니다(PRESS ANY KEY)\n");
	closesocket(clientSocket);
	WSACleanup();

	getchar();
}