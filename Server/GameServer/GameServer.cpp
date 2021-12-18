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
	/*
	* 네트워크가 어려개일때 유동적으로 동작
	*/

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

	
	vector<Session> sessions;
	sessions.reserve(100);

	fd_set reads;
	fd_set writes;

	while (true)
	{
		FD_ZERO(&reads);
		FD_ZERO(&writes);

		FD_SET(listenSocket, &reads);

		//***
		for (Session& session : sessions)
		{
			if (session.recvBytes <= session.sendBytes)
				FD_SET(session.socket, &reads);
			else
				FD_SET(session.socket, &writes);
		}

		int32 retVal = ::select(0, &reads, &writes, nullptr, nullptr);
		if (retVal == SOCKET_ERROR)
			break;

		// Listener 소켓 체크 -> 삭제되지않으면 준비 완료
		if (FD_ISSET(listenSocket, &reads))
		{
			SOCKADDR_IN clientAddr;
			int32 addrLen = sizeof(clientAddr);
			SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
			if (clientSocket != INVALID_SOCKET)
			{
				cout << "Client Connected" << endl;
				sessions.push_back(Session{ clientSocket });
			}
		}

		// 나머지 소켓 체크
		for (Session& s : sessions)
		{
			// Read
			if (FD_ISSET(s.socket, &reads))
			{
				int32 recvLen = ::recv(s.socket, s.recvBuffer, BUFSIZE, 0);
				if (recvLen <= 0) // 연결이 끊김
				{
					//sessions 제거


					break;
				}

				s.recvBytes = recvLen;
			}

			// Write
			if (FD_ISSET(s.socket, &writes))
			{
				// 블로킹 모드 -> 모든 데이터 다 보냄
				// 논블로킹 모드 -> 일부만 보낼 수가 있음 (상대방 수신 버퍼 상황에 따라)
				int32 sendLen = ::send(s.socket, &s.recvBuffer[s.sendBytes], s.recvBytes - s.sendBytes, 0);
				if (sendLen == SOCKET_ERROR)
				{
					// TODO : sessions 제거
					continue;
				}

				s.sendBytes += sendLen;
				if (s.recvBytes == s.sendBytes)
				{
					s.recvBytes = 0;
					s.sendBytes = 0;
				}
			}
		}
	}

	// -----------------------------


	// 윈속 종료
	::WSACleanup();
}