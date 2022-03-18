# project_oneclick

## 2022 mini project - socket programming with C [미니 프로젝트]

* [Markdown 문법 사용하기](http://whatismarkdown.com/)


* [git add Permision denied 관련 해결](https://openingsound.tistory.com/95)


* [telent 사용하기](https://opentutorials.org/module/2160/12506) - cmd 실행 후 telnet localhost port번호 입력 (ex) telnet localhost 9090)



# 1. 프로젝트 개요
## 1.1 TCP/IP 프로토콜 기반 소켓 통신 구현하기 with C
[TCP/IP 통신이란?](https://www.ibm.com/docs/ko/aix/7.1?topic=management-transmission-control-protocolinternet-protocol) 컴퓨터가 서로 통신하는 경우, 특정 규칙이나 프로토콜을 사용하여 순서대로 데이터를 전송 및 수신할 수 있다. 전 세계를 통해 가장 일상적으로 사용되는 프로토콜 세트 중 하나가 TCP/IP(Transmission Control Protocol/Internet Protocol)이다. TCP/IP는 네트워크에 연결된 여러 Host 사이의 통신을 허용한다. 하드웨어에 구애받지 않고, 여러 컴퓨터들을 제어할 수 있다는 점에서 TCP/IP 및 소켓 통신 기반의 간단한 메신저를 만들어보고자 한다. 


## 1.2. Socket 통신 사용하기
[소켓 통신이란?](https://kotlinworld.com/75) 소켓(Socket)은 프로세스가 드넓은 네트워크 세계로 데이터를 내보내거나 혹은 그 세계로부터 데이터를 받기 위한 실제적인 청구 역할을 한다. 그러므로 프로세스가 데이터를 보내거나 받기 위해서는 반드시 소켓을 열어서 소켓에 데이터를 써보내거나 소켓으로부터 데이터를 읽어들여야 한다. 소켓은 <u>프로토콜, IP, 포트</u>로 정의된다.

### 1.2.1 용어정리
* <b>프로토콜</b> - 어떤 시스템이 다른 시스템과 통신을 원활하게 수행하도록 해주는 통신 규약

* <b>IP</b> - 전 세계 컴퓨터에 부여된 고유의 식별 주소

* <b>포트</b> - 네트워크 상에서 통신하기 위해서 호스트 내부적으로 프로세스가 할당받아야 하는 고유한 숫자. 한 호스트 내에서 네트워크 통신을 하고 있는 프로세스를 식별하기 위해 사용되는 값으로, 같은 호스트 내에서 서로 다른 프로세스가 같은 포트 넘버를 가질 수 없다. 같은 컴퓨터 내에서 프로그램을 식별하는 번호

### 1.2.2 소켓 통신 흐름
![socket_structure](./img/socket_structure.png)
> * <span style="color:00AFFF"><b>SERVER</b></span> : 클라이언트에서 접속할 때 까지 Listen 상태로 대기한다. 클라이언트의 연결 요청이 오면 accept를 통해 접속을 허락한다. Server socket을 생성할 때, 사용할 프로토콜, IP, 포트 번호를 지정해 준다.  
> * <span style="color:00AFFF"><b>CLIENT</b></span> : 서버와 같은 프로토콜, 서버의 IP 및 포트 번호를 사용해 Socket을 생성하고 서버와 통신할 수 있다.

### 1.2.3 정리
소켓은 기본적으로 Connection 기반의 양방향 통신이다. 서버와 클라이언트 모두 Socket이 필요하며, 해당 Socket이 연결되어 있는 상태에서 통신이 가능하다. 따라서 서버가 클라이언트로 메시지를 보내고 받을 수 있지만, 클라이언트 역시 서버로 메시지를 보내고 받을 수 있다. 이 Connection이 계속해서 유지되기 때문에 스트리밍이나 실시간 채팅 등 실시간 데이터 교환에서 이점이 있다. 하지만 계속해서 Connection을 유지해야 하는 부분에서 많은 리소스가 소모된다는 단점이 있다.  

# 2. 프로젝트 구현
## 2.1 개발 환경
* Visual Studio 2022
* Operating System - Windows 10
## 2.2 SERVER-CLIENT : Basic Socket Communication
</br>
<p align ="center"><img src="./img/socket-basic.gif"></p>

* <em>socket()</em> - 서버에서 사용 할 소켓을 생성하는 함수
* <em>bind()</em> - 생성한 소켓에 사용할 프로토콜, IP, 포트 넘버를 부여하고 BIND하는 함수
* <em>listen()</em> - 서버에서 클라이언트의 요청이 오는지 확인하는 함수
* <em>accpet()</em> - 클라이언트에서 connect 연결 요청이 왔을 때, 해당 연결을 허용하는 함수
* <em>recv()/send()</em> - 서버 및 클라이언트 소켓에서 데이터를 송/수신하는 함수
* <em>closesocket()</em> - 생성된 소켓 소멸하는 함수

### 2.2.1 Implementation - SERVER  

#### 2.2.1.1 Headers & 전처리
```C
#pragma comment(lib,"ws2_32.lib")  // winsock2를 사용하기 위한 lib를 추가합니다.
#include <Winsock2.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9090
```
1. 윈도우 환경에서 소켓을 사용하기 위한 외부 라이브러리 추가
2. Visual studio에서 소켓을 사용하기 위한 Winsock2.h 헤더파일 추가
3. 사용할 IP 및 포트 설정(프로젝트에서는 로컬 IP 주소를 사용했음)  

#### 2.2.1.2 Winsock 초기화
```C
WSADATA wsaData;

// 1. Winsock 초기화, WINSOCK VERSION 2.2
if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
	printf("WSAStartup failed: %d\n", WSAStartup(MAKEWORD(2, 2), &wsaData));
	return 1;
}
else {
	Sleep(2000);
	printf("WSAStartup completed!!\n");
}

```
1. Winsock을 초기화하는 부분이다.
2. WSAStartup은 Winsock의 속성 정보를 자동으로 설정해준다.
3. 실제로 소켓 프로그래밍을 할 때 사용할 일은 없지만, 하위 버전과의 호환성을 맞추어준다는 부분에서 꼭 설정해야 한다.

#### 2.2.1.3 Socket 생성
```C
SOCKET serverSocket; // 소켓 인스턴스
struct sockaddr_in server; // 프로토콜, IP, 포트 설정을 위한 구조체

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
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET; // IPv4
	server.sin_port = htons(9090); // 사용할 포트 번호 지정
```
1. socket() 함수를 통해 소켓 인스턴스를 만들어 준다.
2. sockaddr_in 구조체 server에 서버가 사용할 통신방법, IP, 포트 번호를 할당해 놓는다.
3. INADDR_ANY를 통해 자동으로 컴퓨터에 존재하는 랜카드 ip주소를 할당해 줄 수있다. 이 방법을 사용하는 것이 호환성 부분에서 좀 더 유연하기 때문에 사용하는 것이 좋다.

#### 2.2.1.4 컴퓨터 네트워크에 Bind
```C
// 3. Socket 생성 후 Bind
	if (bind(serverSocket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
		printf("Bind failed with error code : %d\n", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	else {
		Sleep(2000);
		printf("Bind done!\n");
	}
```
1. bind() 함수를 통해 소켓이 사용할 네트워크 정보를 바인딩 해준다.
2. 이전 단계에서 생성했던 sockaddr_in 구조체 server에 할당했던 통신방법, ip정보, 포트 넘버가 소켓 및 네트워크에 할당되는 단계이다.

#### 2.2.1.5 서버에서 listen을 통해 클라이언트의 연결 요청을 대기
```C
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
```
1. listen()함수를 통해 클라이언트의 연결 요청을 감지한다

#### 2.2.1.6 서버에서 클라이언트의 요청을 accept
```C
SOCKET clientSocket;

        // 5. Client의 연결 요청을 수락하기
	clientSocket = INVALID_SOCKET;
	clientSocket = accept(serverSocket, NULL, NULL);
	if (clientSocket == INVALID_SOCKET) {
		printf("accept failed: %d\n", WSAGetLastError());
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}
	else {
		printf("Connection accepted!\n");
	}
```
1. 서버의 소켓을 통해 accept 된 클라이언트의 정보를 clientSocket에 넣는다
2. 해당 클라이언트에서 전송한 정보들이 clientSocket에 저장되는 형식

#### 2.2.1.7 서버 종료 및 Winsock 초기화
```C
        closesocket(clientSocket);
	closesocket(serverSocket);
	WSACleanup();
```
1. 사용했던 서버 및 받아온 클라이언트 소켓 종료 및 초기화
2. 사용한 Winsock 초기화

### 2.2.2 Implementation - CLIENT
#### 2.2.2.1 Headers & 전처리
```C
#pragma comment(lib,"ws2_32.lib")  // winsock2를 사용하기 위한 lib를 추가합니다.
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <process.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define DEFAULT_PORT "27015"
#define MY_LOCAL_IP "127.0.0.1"
```
1. 윈도우 환경에서 소켓을 사용하기 위한 외부 라이브러리 추가
2. Visual studio에서 소켓을 사용하기 위한 Winsock2.h 헤더파일 추가
3. 사용할 IP및 포트 설정(프로젝트에서는 로컬 IP주소를 사용했음)

#### 2.2.2.2 Winsock 초기화
```C
        // initializing Winsock
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
```
1. Winsock을 초기화하는 부분이다.
2. WSAStartup은 Winsock의 속성 정보를 자동으로 설정해준다.
3. 실제로 소켓 프로그래밍을 할 때 사용할 일은 없지만, 하위 버전과의 호환성을 맞추어준다는 부분에서 꼭 설정해야 한다.

#### 2.2.2.3 Socket 생성
```C
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
```
1. socket() 함수를 통해 소켓 인스턴스를 만들어 준다.
2. sockaddr_in 구조체 server에 클라이언트가 사용할 통신방법, IP, 포트 번호를 할당해 놓는다.
3. INADDR_ANY를 통해 자동으로 컴퓨터에 존재하는 랜카드 ip주소를 할당해 줄 수있다. 이 방법을 사용하는 것이 호환성 부분에서 좀 더 유연하기 때문에 사용하는 것이 좋다.

#### 2.2.2.4 서버에 연결(connect) 요청
```C
    // 3. Connect to server
    if (connect(clientSocket, (struct sockaddr*)&server, sizeof(server)) < 0) {
        printf("Connect error!\n");
        return 1;
    }
    else {
        printf("Connection has been completed!\n");
    }
```
1. sockaddr_in 구조체 server에 저장해 놓았던 연결을 요청할 서버의 통신방법, IP주소, 포트 번호를 통해 서버에 연결을 요청할 수 있다.
2. connect()함수를 사용해 서버에 연결을 요청한다.

#### 2.2.2.5 클라이언트 종료 및 초기화
```C
    closesocket(clientSocket);
    WSACleanup();
```
1. 사용했던 클라이언트 소켓 종료 및 초기화
2. 사용한 Winsock 초기화