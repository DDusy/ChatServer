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

/*

6. send(int fd, void* buffer, size_t n, int flags)

7. recv(int fd, void* buffer, size_t n, int flags)
send함수와 사용법이 거의 비슷.
n바이트를 buffer로 읽습니다. 
성공시 받은 바이트수를 반환하며 실패시 -1을 반환합니다.
 */
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

	cout << "accept" << endl;

	SOCKADDR_IN clientAddr;
	int32 addrLen = sizeof(clientAddr);

	while (true)
	{
		SOCKADDR_IN clientAddr; // IPv4
		::memset(&clientAddr, 0, sizeof(clientAddr));
		int32 addrLen = sizeof(clientAddr);


		SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET)
		{

			int32 errCode = ::WSAGetLastError();

			//논블로킹이므로 블락이 문제되진 않는다.
			if (errCode == WSAEWOULDBLOCK)
				 continue;

			cout << "Accept ErrorCode : " << errCode << endl;

			break;
		}


		cout << "client Connected" << endl;

		// client 연결
		//char ipAddress[16];
		//// 알아보기쉽게 문자열로 변환
		//::inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddress, sizeof(ipAddress));
		//cout << "Client Connected! IP = " << ipAddress << endl;


			//// 손님 입장!
			//char ipAddress[16];
			//::inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddress, sizeof(ipAddress));
			//cout << "Client Connected! IP = " << ipAddress << endl;

			// Recv
			while (true)
			{
				char RecvBuffer[1000]=" ";
				int32 recvLen = ::recv(clientSocket, RecvBuffer, sizeof(RecvBuffer), 0);
				if (recvLen == SOCKET_ERROR)
				{

					int32 errCode = ::WSAGetLastError();
					if (errCode == WSAEWOULDBLOCK)
						continue;

					// Error
					cout << "Recv ErrorCode : " << errCode << endl;

					break;
				}
				else if (recvLen == 0)
				{
					// 연결 끊김
					break;
				}
				
				cout << "Recv Data! Data = " << RecvBuffer << endl;

				char SendBuffer[1000]="서버에서 보낸다";

				// Send
				while (true)
				{
					if (::send(clientSocket, SendBuffer, recvLen, 0) == SOCKET_ERROR)
					{
						
						if (::WSAGetLastError() == WSAEWOULDBLOCK)
							continue;
						// Error
						break;
					}

					break;
				}
			}
	}

	// -----------------------------


	// 윈속 종료
	::WSACleanup();
}