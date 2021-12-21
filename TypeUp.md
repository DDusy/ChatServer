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

 Select 모델 - 동기방식
	소켓 함수 호출이 성공할 시점을 미리 알 수 있다 (send / recv 등)

​	문제상황들

​	 수신버퍼에 데이터가 없는데, read 한다거나, 송신버퍼가 꽉 찼는데, write 할때
​	 블로킹 소켓 => 조건이 만족되지 않아서 블로킹되는 상황 예방
​	논블로킹 소켓 => 조건이 만족되지 않아서 불필요하게 반복 체크하는 상황을 예방

Socket Set 과정

1) 읽기[ ] 쓰기[ ] 예외(OOB)[ ] 관찰 대상 등록
    OutOfBand는 send() 마지막 인자 MSG_OOB로 보내는 특별한 데이터
    받는 쪽에서도 recv OOB 세팅을 해야 읽을 수 있음 	*OOB(Out of Band) 우선순위 통신시 사용

2. select(readSet, writeSet, exceptSet); -> 관찰 시작

3. 적어도 하나의 소켓이 준비되면 리턴 -> 준비안된 소켓은 제거

4. 남은 소켓 체크해서 진행

     

     *관련함수 - FD(File Descriptor) 사용할 파일이나 장치에 대한 고유 식별 값

     FD_ZERO : 초기화/ ex) FD_ZERO(set);
     FD_SET : 소켓 s를 넣음 / ex) FD_SET(s, &set);
     FD_CLR : 소켓 s를 제거 / ex) FD_CLR(s, &set);
     FD_ISSET : 소켓 s가 set에 들어있으면 0이 아닌 값을 리턴

단점

- 한번에 등록할 수있는 Set의 개수가 적음

- 매번 소켓Set을 등록하고 사용 / 많은 양은 불가X

WSAEventSelect - 비동기 방식

​	소켓과 관련된 네트워크 이벤트를 이벤트 객체를 통해 감지 

​	소켓개수 만큼 이벤트 객체 연동

**이벤트 객체 관련 함수들**

- 생성 : WSACreateEvent (기본 select에서 수동 리셋 Manual-Reset + Non-Signaled 상태 시작)
-  삭제 : WSACloseEvent
- 신호 상태 감지 : WSAWaitForMultipleEvents
- 구체적인 네트워크 이벤트 알아내기 : WSAEnumNetworkEvents (Read,Write,Connect 등을 알고싶을때)

소켓 연동 : **WSAEventSelect(socket, event, networkEvents) **

> 관심있는 네트워크 이벤트 종류 / 어떤 이벤트상태를 감지할지

- FD_ACCEPT : 접속한 클라가 있음 accept
- FD_READ : 데이터 수신 가능 recv, recvfrom
- FD_WRITE : 데이터 송신 가능 send, sendto
- FD_CLOSE : 상대가 접속 종료
- FD_CONNECT : 통신을 위한 연결 절차 완료
- FD_OOB

**주의 사항**
 **WSAEventSelect** 함수를 호출하면, 해당 소켓은 자동으로 논블로킹 모드 전환
accept() 함수가 리턴하는 소켓은 listenSocket과 동일한 속성을 갖는다

- 따라서 clientSocket은 FD_READ, FD_WRITE 등을 다시 등록 필요
- 드물게 WSAEWOULDBLOCK 오류가 뜰 수 있으니 예외 처리 필요
-  이벤트 발생 시, 적절한 소켓 함수 호출해야 함아니면 다음 번에는 동일 네트워크 이벤트가 발생 X
  ex) FD_READ 이벤트 떴으면 recv() 호출해야 하고, 안하면 FD_READ 두 번 다시 X

**WSAWaitForMultipleEvents 인자**

1. count, event

2. waitAll :  모두 기다릴지 아니면 하나만 완료 되어도 빠져나올지 결정

3. timeout : 타임아웃

4. 지금은 false

   **return 은 완료된 첫번째 인덱스**

**WSAEnumNetworkEvents 인자** 
1.socket	
2.eventobject 	:	socket 과 연동된 이벤트 객체 핸들을 넘겨주면,
								 이벤트 객체를 non-signaled
3.networkEvent : 네트워크 이벤트 / 오류 정보가 저장

