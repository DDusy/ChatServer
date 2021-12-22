#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <mutex>
#include <winsock2.h> // window만의 소켓
#include <MSWSock.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int main()
{

	// 관련 정보가 wsaData에 채워짐
	WSAData wsaData;

	/*
	* 윈소켓 초기화 lib 초기화 필요
	 https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsastartup
	 2번째 인자 win소켓 구현의 세부정보를 수신할 WSADATA 데이터 구조에 대한 포인터

	 성공하면 0리턴
	*/
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	/*
	* 
	* 1. ad(Address Family) : 어떤 주소체계를 가질지  ->(AF_INET = IPv4, AF_INET6 = IPv6)
	* 2. type : 소켓을 어떻게 사용지  ->  TCP(SOCK_STREAM) or UDP(SOCK_DGRAM)?
	* 3. protocol: 어떤 프로토콜
	* 4. return : descriptor 정수를 받아 사용 실제 리소스를 가르키는 모양
	* 
	* cmd 로  ipconfig로 네트워크 주소 검색
	*/
	SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);  //IPv4
	if (clientSocket == INVALID_SOCKET)	// 소켓 처리 실패시
	{
		int32 errCode = ::WSAGetLastError();	// 에러코드 반환 
		cout << "Socket ErrorCode : " << errCode << endl;
		return 0;
	}

	u_long on = 1;
	if (::ioctlsocket(clientSocket, FIONBIO, &on) == INVALID_SOCKET)
		return 0;

	// 연결할 목적지는? IP주소 + Port
	SOCKADDR_IN serverAddr; // IPv4
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr); 
	serverAddr.sin_port = ::htons(7777);  


	while (true)
	{
		if (::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		{
			int32 errCode = ::WSAGetLastError();


			if (errCode == WSAEWOULDBLOCK)
				continue;

			if (errCode == WSAEISCONN)
				break;

			break;;
		}

	}
	
	cout << "Connected To Server!" << endl;

	char sendBuffer[100] = "Client SendData";

	WSAEVENT wsaEvent = ::WSACreateEvent();
	WSAOVERLAPPED overlapped = {};
	overlapped.hEvent = wsaEvent;

	while (true)
	{
	
		WSABUF wsaBuf;
		wsaBuf.buf = sendBuffer;
		wsaBuf.len = 100;

		DWORD sendLen = 0;
		DWORD flags = 0;
		if (::WSASend(clientSocket, &wsaBuf, 1, &sendLen, flags, &overlapped, nullptr) == SOCKET_ERROR)
		{
			if (::WSAGetLastError() == WSA_IO_PENDING)
			{
				// Pending
				::WSAWaitForMultipleEvents(1, &wsaEvent, TRUE, WSA_INFINITE, FALSE);
				::WSAGetOverlappedResult(clientSocket, &overlapped, &sendLen, FALSE, &flags);
			}
			else
			{
				// 진짜 문제 있는 상황
				break;
			}
		}

		cout << "Client RecvData = " << recvBuffer<< " Length : " << sizeof(recvBuffer) << endl;

			break;
		
		this_thread::sleep_for(1s);
	}

	// 소켓 다 사용시 정리
	::closesocket(clientSocket);

	//Window 소켓 종료
	WSACleanup();
	
}