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

	// 연결할 목적지는? IP주소 + Port
	SOCKADDR_IN serverAddr; // IPv4
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr); // dns를 이용해 서버 찾기
	serverAddr.sin_port = ::htons(7777);  
	// host to network short -> endian issue 존재
	// Little-Endian vs Big-Endian
	// ex) 0x12345678 4바이트 정수
	// low [0x78][0x56][0x34][0x12] high < little 인텔은 little 
	// low [0x12][0x34][0x56][0x78] high < big

	/*
	connect 원격 호스트(원격 컴퓨터)와 연결하는 함수입니다. 
	연결된 정보는 remote_host에 저장됩니다.
	성공시 0, 오류시 -1을 반환합니다.
	*/
	//				소켓			,	소켓 어드레스		  , 주소사이즈
	if (::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		int32 errCode = ::WSAGetLastError();
		cout << "Connect ErrorCode : " << errCode << endl;
		return 0;
	}

	cout << "Connected To Server!" << endl;

	while (true)
	{
		// TODO
		char sendBuffer[100] = "Hello World!";

		for (int32 i = 0; i < 10; i++)
		{
			//클라이언트는 send 서버에선 receive
			//buffer를 소켓 파일 디스크립터인 소켓으로 전송합니다.
			//보낸 바이트수를 반환하며 실패시 - 1을 반환합니다.
			int32 resultCode = ::send(clientSocket, sendBuffer, sizeof(sendBuffer), 0);
			if (resultCode == SOCKET_ERROR)
			{
				int32 errCode = ::WSAGetLastError();
				cout << "Send ErrorCode : " << errCode << endl;
				return 0;
			}
		}


		cout << "Send Data! Len = " << sizeof(sendBuffer) << endl;
		/*
		* 소켓 입출력 버퍼
		* 클라 - 서버 (버퍼를 통해 통신)
		* 커널 레벨에서 소켓과 관련한 버퍼 생성
		* 
		*/
		char recvBuffer[1000];

		int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
		if (recvLen <= 0)
		{
			int32 errCode = ::WSAGetLastError();
			cout << "Recv ErrorCode : " << errCode << endl;
			return 0;
		}

		cout << "Recv Data! Data = " << recvBuffer << endl;
		cout << "Recv Data! Len = " << recvLen << endl;

		this_thread::sleep_for(1s);
	}

	// 소켓 다 사용시 정리
	::closesocket(clientSocket);

	//Window 소켓 종료
	WSACleanup();
	
}