#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <winsock2.h> // window만의 소켓
#include <MSWSock.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int main()
{
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
	* 어떤 주소체계를 가질지
	* 소켓을 어떻게 사용지  -> tcp or udp?
	//  tcp는 패킷 유실시 재전송과 오염 체크가 가능하므로 자주사용 udp 는 패킷을 빨리처리
	* 어떤 프로토콜
	* return : descriptor // 정수를 받아 사용 실제 리소스를 가르키는 모양
	* 
	* cmd 로  ipconfig로 네트워크 주소 검색
	* IPv4 주소  AF_INET  . . . . . . . . . : 172.20.10.6
	* IPv6 주소  AF_INET6 
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
		//서버내용
	}

	// 소켓 다 사용시 정리
	::closesocket(clientSocket);

	//Window 소켓 종료
	WSACleanup();
	
}