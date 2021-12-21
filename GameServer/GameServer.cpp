#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>

#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include "ConCurrentQueue.h"
#include "ConCurrentStack.h"

const int32 BUFSIZE = 1000;

struct Session
{
	SOCKET socket = INVALID_SOCKET;
	char recvBuffer[BUFSIZE] = {};
	char sendBuffer[BUFSIZE] = "WSASelectModel Test!";

	int32 recvBytes = 0;
	int32 sendBytes = 0;
};

int main()
{
	//	Bind -> lisen ->accept(socket 반환)->
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;


	/*소켓을 만드는데 바로 이 함수를 사용합니다.
	소켓 역시 파일로 다루어지기 때문에 반환값은 파일디스크립터입니다.
	만약 소켓을 여는데 실패했다면 - 1을 리턴합니다.*/
	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
	{
		int32 errCode = ::WSAGetLastError();
		cout << "Socket ErrorCode : " << errCode << endl;
		return 0;
	}

	u_long on = 1;
	if (::ioctlsocket(listenSocket, FIONBIO, &on) == INVALID_SOCKET)
	{
		cout << "ioc ErrorCode : " << endl;
		return 0;
	}
		


	// 나의 주소
	SOCKADDR_IN serverAddr; // IPv4
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY); // 자동으로 선택
	
	serverAddr.sin_port = ::htons(7777); // 80 : HTTP

		if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		int32 errCode = ::WSAGetLastError();
		cout << "Bind ErrorCode : " << errCode << endl;
		return 0;
	}

	if (::listen(listenSocket, 10) == SOCKET_ERROR)
	{
		int32 errCode = ::WSAGetLastError();
		cout << "Listen ErrorCode : " << errCode << endl;
		return 0;
	}

	cout << "Accept" << endl;

	
	
	vector<WSAEVENT> wsaEvents; // WsaEvent로 교체

	vector<Session> sessions;
	sessions.reserve(100);

	WSAEVENT listenEvent = WSACreateEvent();
	wsaEvents.push_back(listenEvent);
	sessions.push_back(Session{ listenSocket }); // 클라이벤트랑 소켓의 개수가 맞추기위해

	// lisenEvent와 lisenSocket의 개수는 같음
	if (::WSAEventSelect(listenSocket, listenEvent, FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR)
	{
		cout << "Socket Error" << endl;
		return 0;
	}

	
	while (true)
	{
		int32 idx = ::WSAWaitForMultipleEvents(DWORD(wsaEvents.size()),&wsaEvents[0], FALSE, WSA_INFINITE, FALSE);

		if (idx == WSA_WAIT_FAILED)
			continue;

		WSANETWORKEVENTS networkEvents;
		//Reset 포함
		if (::WSAEnumNetworkEvents(sessions[idx].socket, wsaEvents[idx], &networkEvents) == SOCKET_ERROR)
			continue;

		if (networkEvents.lNetworkEvents & FD_ACCEPT)
		{
			if (networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
				continue;

			SOCKADDR_IN clientAddr;
			int32 AddrLen = sizeof(clientAddr);

			SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &AddrLen);
			if (clientSocket != INVALID_SOCKET)
			{
				cout << "Client Connected" << endl;

				WSAEVENT clientEvent = ::WSACreateEvent();
				wsaEvents.push_back(clientEvent);
				sessions.push_back(Session{ clientSocket });

				if (::WSAEventSelect(clientSocket, clientEvent, FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
					return 0;
			}

		}

		// Client Session 소켓 체크
		if (networkEvents.lNetworkEvents & FD_READ || networkEvents.lNetworkEvents & FD_WRITE)
		{
			// Error-Check
			if ((networkEvents.lNetworkEvents & FD_READ) && (networkEvents.iErrorCode[FD_READ_BIT] != 0))
				continue;
			// Error-Check
			if ((networkEvents.lNetworkEvents & FD_WRITE) && (networkEvents.iErrorCode[FD_WRITE_BIT] != 0))
				continue;

			Session& s = sessions[idx];

			// Read
			if (s.recvBytes == 0)
			{
				int32 recvLen = ::recv(s.socket, s.recvBuffer, BUFSIZE, 0);
				if (recvLen == SOCKET_ERROR && ::WSAGetLastError() != WSAEWOULDBLOCK)
				{
					cout << "Session error" << endl;
					continue;
				}

				s.recvBytes = recvLen;
				cout << "Server RecvData = " <<s.recvBuffer << " Length : " << recvLen << endl;
				
			}

			// Write
			if (s.recvBytes > s.sendBytes)
			{
				int32 sendLen = ::send(s.socket,s.sendBuffer, 100, 0);
				
				if (sendLen == SOCKET_ERROR && ::WSAGetLastError() != WSAEWOULDBLOCK)
				{
					cout << "Session error" << endl;
					continue;
				}

				s.sendBytes += sendLen;
				if (s.recvBytes == s.sendBytes)
				{
					s.recvBytes = 0;
					s.sendBytes = 0;
				}

				cout << "Server SendData = " <<s.sendBuffer<<" Length : " << sendLen << endl;
			}
		}

		// FD_CLOSE 처리
		if (networkEvents.lNetworkEvents & FD_CLOSE)
		{
			cout << "Socket error" << endl;
		}
	}


	// 윈속 종료
	::WSACleanup();
}