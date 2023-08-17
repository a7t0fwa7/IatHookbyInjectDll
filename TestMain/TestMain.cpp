// test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
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
	printf("第一次调用createfile：\n");
	hFile = CreateFileA(
		"第一次.txt",
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
		"第二次.txt",
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
