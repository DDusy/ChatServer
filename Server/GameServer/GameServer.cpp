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

	// 나의 주소
	SOCKADDR_IN serverAddr; // IPv4
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY); // 자동으로 선택
	/*
	* 네트워크가 어려개일때 유동적으로 동작
	*/

	serverAddr.sin_port = ::htons(7777); // 80 : HTTP

	/*소켓을 바인딩합니다. 지금 넘겨지는 소켓과 이 프로세스와 묶는다(bind)라고 생각하시면 됩니다. 
	그래서 해당 프로세스는 소켓을 통해 다른 컴퓨터로부터 연결을 받아들일 수 있습니다.*/
	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		int32 errCode = ::WSAGetLastError();
		cout << "Bind ErrorCode : " << errCode << endl;
		return 0;
	}

	// 처리시작
	/* 소켓을 통해 들어오는 연결을 듣습니다.
	backlog_queue_size만큼 연결 요청을 큐에 넣습니다. 
	성공시 0, 오류시 -1을 반환합니다.*/
	if (::listen(listenSocket, 10) == SOCKET_ERROR)
	{
		int32 errCode = ::WSAGetLastError();
		cout << "Listen ErrorCode : " << errCode << endl;
		return 0;
	}

	// -----------------------------

	while (true)
	{
		SOCKADDR_IN clientAddr; // IPv4
		::memset(&clientAddr, 0, sizeof(clientAddr));
		int32 addrLen = sizeof(clientAddr);

		/*
		어떤 컴퓨터에서 이 컴퓨터로 연결할때 연결을 받아들입니다.
		연결된 원격 컴퓨터의 정보는 remote_host에 저장됩니다. 오류시에 -1을 반환합니다.
		*/
		SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			int32 errCode = ::WSAGetLastError();
			cout << "Accept ErrorCode : " << errCode << endl;
			return 0;
		}

		// client 연결
		char ipAddress[16];
		// 알아보기쉽게 문자열로 변환
		::inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddress, sizeof(ipAddress));
		cout << "Client Connected! IP = " << ipAddress << endl;


		while (true)
		{
			SOCKADDR_IN clientAddr; // IPv4
			::memset(&clientAddr, 0, sizeof(clientAddr));
			int32 addrLen = sizeof(clientAddr);

			SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
			if (clientSocket == INVALID_SOCKET)
			{
				int32 errCode = ::WSAGetLastError();
				cout << "Accept ErrorCode : " << errCode << endl;
				return 0;
			}

			// 손님 입장!
			char ipAddress[16];
			::inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddress, sizeof(ipAddress));
			cout << "Client Connected! IP = " << ipAddress << endl;

			// TODO
			while (true)
			{
				char recvBuffer[1000];

				this_thread::sleep_for(1s);

				int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
				if (recvLen <= 0)
				{
					int32 errCode = ::WSAGetLastError();
					cout << "Recv ErrorCode : " << errCode << endl;
					return 0;
				}

				cout << "Recv Data! Data = " << recvBuffer << endl;
				cout << "Recv Data! Len = " << recvLen << endl;

				/*int32 resultCode = ::send(clientSocket, recvBuffer, recvLen, 0);
				if (resultCode == SOCKET_ERROR)
				{
					int32 errCode = ::WSAGetLastError();
					cout << "Send ErrorCode : " << errCode << endl;
					return 0;
				}*/
			}
		}
	}

	// -----------------------------


	// 윈속 종료
	::WSACleanup();
}