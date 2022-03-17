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

HANDLE hMutex = 0; // 쓰레드 동기화 처리를 위한 뮤텍스
int client_num = 0; // 클라이언트 개수
int seat = 0; // 클라이언트 번호
SOCKET client_sock[65535]; // 클라이언트 welcom socket

// 서버 초기화
int server_init() {
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

	// 3. Socket 생성 후 Bind
	if (bind(s, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
		printf("Bind failed with error code : %d\n", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	else {
		Sleep(2000);
		printf("Bind done!\n");
	}

	// 4. Socket에서 Listen을 통해 수신 대기
	if (listen(s, SOMAXCONN) == SOCKET_ERROR) {
		printf("Listen failed with error: %ld\n", WSAGetLastError());
		closesocket(s);
		WSACleanup();
		return 1;
	}
	else {
		Sleep(2000);
		printf("Waiting for incoming connections...\n");
	}
	return s;
	
}

// 스레드 함수
void recv_client(void* ns) {
	// 정상적으로 연결요청 accpet 되었을 때 스레드 실행
	// 클라이언트의 숫자를 늘림

	// 임계 영역, mutex lock을 활용해 공동 접근 불가능하게 한다
	WaitForSingleObject(hMutex, INFINITE);
	client_num++; // 클라이언트 개수 증가
	seat++; // 클라이언트 번호 증가
	ReleaseMutex(hMutex);

	char welcome[100] = { 0 };
	char buff[1024] = { 0 };
	int ret, i;

	_itoa(seat, welcome, 10);
	strcat(welcome, "번 클라이언트, 환영합니다\n");
	ret = send(*(SOCKET*)ns, welcome, sizeof(welcome), 0); // 메시지 전송

	while (ret != SOCKET_ERROR || ret != INVALID_SOCKET) {
		ret = recv(*(SOCKET*)ns, buff, 1024, 0); //클라이언트로부터 메시지 받음

		// 모든 client에게 broadcast
		for (i = 0; i < 65535; i++) {
			// 받은 클라이언트 소켓의 메모리 주소와
			// 보내는 클라이언트의 소켓 메모리 주소가 다르면 전송
			WaitForSingleObject(hMutex, INFINITE);
			if ((unsigned*)&client_sock[i] != (SOCKET*)ns) {
				send(client_sock[i], buff, strlen(buff), 0);
			}
			ReleaseMutex(hMutex);
		}
		if (strlen(buff) != 0) {
			printf("%d에 메시지 보냄 : %s", strlen(buff), buff);
		}
		memset(buff, 0, 1024);
	}

	// 접속된 소켓이 연결을 해제 시켰을때 
	WaitForSingleObject(hMutex,INFINITE);
	client_num--;
	ReleaseMutex(hMutex);
	ReleaseMutex(hMutex);
	return;
}

int main() {
	int addrlen,addrsize;
	SOCKET serverSocket;
	struct sockaddr_in client;
	hMutex = CreateMutex(NULL, FALSE, NULL);
	if (!hMutex) {
		printf("Mutex ERROR\n");
		CloseHandle(hMutex);
		return 1;
	}

	serverSocket = server_init();
	if (serverSocket < 0) {
		printf("서버 초기화 에러\n");
		exit(0);
	}
	// wait clients' connecting signal

	addrlen = sizeof(struct sockaddr_in);

	while (1) {
		//Blocking 방식으로 Client 대기
		client_sock[seat] = accept(serverSocket, (struct sockaddr*)&client, (int*) & addrlen);
		if (client_num < MAX_CLIENTS) {
			if (!(client_sock[seat] == INVALID_SOCKET || client_sock[seat] == SOCKET_ERROR)){
				_beginthread(recv_client, 0, &client_sock[seat]);
				Sleep(10);
				printf("New connection, %dth client, ip is : %s, port : %d\n", seat, inet_ntoa(client.sin_addr), ntohs(client.sin_port));
			}
		}
		// 가득 찬 경우
		else {
			addrsize = sizeof(client);
			// client_sock 배열이 가득 차서 더 이상 받을 수 없음
			if (client_sock[seat] == INVALID_SOCKET) {
				printf("accept error");
				closesocket(client_sock[seat]);
				closesocket(serverSocket);
				WSACleanup();
				return 1;
			}
			char* message = "Client Full!\n";
			if (send(client_sock[seat], message, strlen(message), 0) != strlen(message)) {
				perror("send failed");
			}
			printf("Welcome message sent successfully to %s\n", inet_ntoa(client.sin_addr));
		}
	}
}