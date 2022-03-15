#pragma comment(lib,"ws2_32.lib")  // winsock2를 사용하기 위한 lib를 추가합니다.
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <string.h>
#include <io.h>
#define DEFAULT_PORT "27015"
#define CLIENT_SOCKET_NUMBER 30
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

// 윈도우 소켓의 가장 기본적인 흐름
// Winsock 초기화 -> socket 생성 -> network 통신 -> socket 닫기 -> Winsock 종료

int main() {

	//WSADATA 구조체에는 Windows 소켓 구현에 대한 정보가 포함되어 있음
	WSADATA wsaData;
	SOCKET serverSocket;
	SOCKET newSocket;
	SOCKET s;
	SOCKET master, client_socket[CLIENT_SOCKET_NUMBER]; // master 사용하지 않음 위에 serverSocket 사용
	struct sockaddr_in server;
	struct sockaddr_in client;
	struct sockaddr_in address;
	char *message = NULL;
	int c;
	int max_clients = CLIENT_SOCKET_NUMBER, activity, addrlen, i, valread;
	int MAXRECV = 1024; // Recieve 버퍼 최대 크기
	fd_set readfds; // Set of socket descriptors
	char* buffer = malloc(sizeof(char) * (MAXRECV + 1)); // NULL character를 위한 1개의 추가 공간

	// Client socket 배열 초기화
	for (int i = 0; i < 30; i++) {
		client_socket[i] = 0;
	}

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
	server.sin_addr.s_addr = inet_addr("211.215.249.35");
	server.sin_family = AF_INET; // IPv4
	server.sin_port = htons(8888); // 사용할 포트 번호 지정

	// 3. Socket 생성 후 Bind
	if (bind(serverSocket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
		printf("Bind failed with error code : %d\n", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	else {
		Sleep(2000);
		printf("Bind done!\n");
	}

	// 4. Socket에서 Listen을 통해 수신 대기
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

	addrlen = sizeof(struct sockaddr_in);

	// 5. Client의 연결 요청을 accept하기
	while (TRUE) {
		// clear the socket fd set
		FD_ZERO(&readfds);

		// add server socket to fd set
		FD_SET(serverSocket, &readfds);

		// add child sockets to fd set
		for (int i = 0; i < max_clients; i++) {
			 s = client_socket[i];
			 if (s > 0) {
				 FD_SET(s, &readfds);
			 }
		}

		// wait for an activity on any of the sockets, timeout = NULL -> 유한대로 기다림
		activity = select(0, &readfds, NULL, NULL, NULL);


		if (activity == SOCKET_ERROR) {
			perror("accept");
			exit(EXIT_FAILURE);
		}

		// 만약 server socket에서 무슨 일이 일어난다 -> 연결 요청이 들어오는 것
		if (FD_ISSET(serverSocket, &readfds)) {
			if ((newSocket = accept(serverSocket, (struct sockaddr*)&address, (int*)&addrlen)) < 0) {
				perror("accept");
				exit(EXIT_FAILURE);
			}
			printf("New connection, socket fd is %d, ip is : %s, port : %d\n", newSocket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));
		}
	}

	/*c = sizeof(struct sockaddr_in);
	newSocket = INVALID_SOCKET;
	
	while ((newSocket = accept(serverSocket, NULL, NULL))!=INVALID_SOCKET) {
		printf("Connection accepted!\n");
		char* recv_buf = malloc(sizeof(char)* (MAXRECV+1));
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
	}*/


	closesocket(newSocket);
	closesocket(serverSocket);
	WSACleanup();

}