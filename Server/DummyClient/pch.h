#pragma once

#define WIN32_LEAN_AND_MEAN // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.

#ifdef _DEBUG
#pragma comment(lib, "Debug\\ServerCore.lib")
#else
#pragma comment(lib, "Release\\ServerCore.lib")
#endif

#include "CorePch.h"

//�̸� ������ �� ��� ��� ��ü 
// �������� ���� ����?

//���̺귯�� �ֱ�
//�߰� ���� ���� / ��Ŀ ���Ͽ� �߰�
// VC++ �� ����
// ��Ŀ �߰� ���Ӽ��� ����
// pch�� pragma comment �̿� ��ó�� �� �ܰ迡��

//pbd �� ũ���� ���� �����ؼ� �ٷ��

/*
* ��Ƽ ������
* 
* 
* 
*/