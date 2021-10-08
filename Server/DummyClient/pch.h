#pragma once

#define WIN32_LEAN_AND_MEAN // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

#ifdef _DEBUG
#pragma comment(lib, "Debug\\ServerCore.lib")
#else
#pragma comment(lib, "Release\\ServerCore.lib")
#endif

#include "CorePch.h"

//미리 컴파일 된 헤더 사용 교체 
// 만들기랑은 무슨 차이?

//라이브러리 넣기
//추가 포함 파일 / 링커 파일에 추가
// VC++ 에 설정
// 링커 추가 종속성도 가능
// pch에 pragma comment 이용 전처리 기 단계에서

//pbd 는 크래스 덤프 관련해서 다루기

/*
* 멀티 스레드
* 
* 
* 
*/