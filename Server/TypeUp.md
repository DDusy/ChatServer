클라이언트 

```
1. 소켓준비
2. 서버 주소로 connect
```

서버

	1. Listener 소켓준비
	2. Bind(서버 주소/ Port를 소켓에 연동)
	3. Listen
	4. Accept



===============================================================================

소켓 입출력 버퍼를 이용해 데이터를 주고 받음

Client Send Buffer -> Server RecvBuff

Server Send Buffer -> Client RecvBuff

```c++
//Send	
char sendBuffer[100] = "Hello World!";

		for (int32 i = 0; i < 10; i++)
		{
        int32 resultCode = ::send(clientSocket, sendBuffer, sizeof(sendBuffer), 0);
			if (resultCode == SOCKET_ERROR)
			{
				int32 errCode = ::WSAGetLastError();
				cout << "Send ErrorCode : " << errCode << endl;
				return 0;
			}
		}
		
// Receiv
	char recvBuffer[1000];

		int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
		if (recvLen <= 0)
		{
			int32 errCode = ::WSAGetLastError();
			cout << "Recv ErrorCode : " << errCode << endl;
			return 0;
		}
```

RecvBuff가 가득차있거나 SendBuff가 비어있으면 블로킹 상태로 된다

===============================================================================

UDP(User Datagram Protocol)

사용자가 정의한 데이터그램을 상대방에게 보낼 수 있게하는 프로토콜

장점

1. Boundary 개념 O  보낼때 부터 경계 생성

단점

1. 전송 순서가 보장되지 않는다

2. 데이터 분실시 복구 X  

   

   ＊연결 개념이 없고 단순하게 데이터를 주고받기만 하므로 속도가 빠름

TCP(Transmission Control Protocol)

장점

1. 전송 순서가 보장
2. 데이터를 주고 받을 상황이 아니면 일부만 보낸다(흐름/ 혼잡 제어)  
3. 데이터 분실시 다시 전송 

단점

1. Boundary 개념 X

   

   *연결을 위해 할당된 논리적 경로 필요 상대적으로 고려할게 많아 UDP에 비해 느림

별도

* UDP +TCP -> Relievable UDP (UDP의 단점 보완)



===============================================================================

Socket Option

```
int setsockopt(

  [in] SOCKET     s,

  [in] int        level, // 옵션을 해석하고 처리

  [in] int        optname, // 

  [in] const char *optval,

  [in] int        optlen );
```

소켓 옵션을 해석하고 level의 어떤 옵션인지 선택

[SO_KEEPALIVE](https://docs.microsoft.com/en-us/windows/desktop/WinSock/so-keepalive)  

끊어진 연결을 감지하기 위해 주기적으;로 연결 상태 확인 (TCP)

SO_LINGER

송신 버퍼에 있는 데이터를 보낼지 말지 

onoff - 0 이면 closesocket()  바로 리턴 아닐땐 linger 초 만큼 대기

===============================================================================

블로킹 모델 

- I/O 작업이 진행되는 동안 유저 프로세스의 작업을 중단한 채 대기하는 방식

  

  - 데이터를 입력받을때까지 무한정 대기(블로킹상태)시 자원이 낭비되기 때문에 무한정 기다릴 수 없고  컨텍스트 스위칭의 부하때문에 클라이언트 수만큼 스레드를 늘릴 수 없다.

논 블로킹 모델

	-  I/O 작업이 진행되는 동안 유저 프로세스의 작업을 중단시키지않는 방식

 Select 모델 
	소켓 함수 호출이 성공할 시점을 미리 알 수 있다!
	// 문제 상황)
	// 수신버퍼에 데이터가 없는데, read 한다거나!
	// 송신버퍼가 꽉 찼는데, write 한다거나!
	// - 블로킹 소켓 : 조건이 만족되지 않아서 블로킹되는 상황 예방
	// - 논블로킹 소켓 : 조건이 만족되지 않아서 불필요하게 반복 체크하는 상황을 예방
