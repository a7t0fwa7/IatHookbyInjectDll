// test.cpp : ���ļ����� "main" ����������ִ�н��ڴ˴���ʼ��������
//

#include <iostream>
#include<windows.h>
using namespace std;

#include "stdio.h"
#include "windows.h"

int main()
{
	HANDLE hFile;
	HANDLE hFile1;
	printf("��һ�ε���createfile��\n");
	hFile = CreateFileA(
		"��һ��.txt",
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile == NULL) {
		printf("hFile==NULL\n");
	}
	else if (hFile != INVALID_HANDLE_VALUE) {
		CloseHandle(hFile);
	}
	getchar();
	hFile1 = CreateFileA(
		"�ڶ���.txt",
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile1 == NULL) {
		printf("hFile==NULL\n");
	}
	else if (hFile1 != INVALID_HANDLE_VALUE) {
		CloseHandle(hFile);
	}

	return 0;
}
