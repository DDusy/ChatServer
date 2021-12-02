TCP

===============================================================================

UDP

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

끊어진 연결을 감지하기 위해 주기적으로 연결 상태 확인 (TCP)

SO_LINGER

송신 버퍼에 있는 데이터를 보낼지 말지 

onoff - 0 이면 closesocket()  바로 리턴 아닐땐 linger 초 만큼 대기





===============================================================================

