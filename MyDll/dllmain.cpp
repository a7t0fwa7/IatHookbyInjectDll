// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "windows.h"
#include "stdio.h"

//创建全局变量，来存要被hook的原始字节
char createfileback[5];


int iathook(PROC orfun, PROC myfun);
int iatunhook(PROC target);
/*
target 为对应原始函数的地址
   PROC orfun = (PROC)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "CreateFileA");

*/
int iatunhook(PROC target) {

    DWORD  oldProtect;
    VirtualProtect(target, 5, PAGE_EXECUTE_READWRITE, &oldProtect);

    //----------------恢复原来的5个字节------------------------------
    memcpy(target, createfileback, 5);
    //----------------恢复原来的5个字节------------------------------

    VirtualProtect(target, 5, oldProtect, &oldProtect);
    return  1;

}
/*
    myfunc 
    unhook——> orgfunc() ——> hook --- return 
*/
HANDLE WINAPI MY_CreateFileA(
    LPCSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile) {
    PROC myfun = (PROC)&MY_CreateFileA;
    HANDLE be = NULL;
    DWORD oldProtect;
    printf("MY_CreateFileA 传入的参数是:%s \n", lpFileName);
    //MessageBoxA(NULL, "aaaaa", "bbbbb", MB_OK);//mov eax,1
    //printf("调用了自定义的CreateFile\n");
    PROC orfun = (PROC)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "CreateFileA");
    int res = iatunhook(orfun);
    if (res == 0) {
        printf("unhook failed\n");
        return 0;
    }

    be = CreateFileA(
        "injected2.txt",
        dwDesiredAccess,
        dwShareMode,
        lpSecurityAttributes,
        dwCreationDisposition,
        dwFlagsAndAttributes,
        hTemplateFile);
    //be = CreateFileA("test.txt", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    //printf("成功调用正常函数 createfile\n");
    if (iathook(orfun, myfun) != 1) {
        printf("重新hook 失败\n");
        return 0;
    }
    return be;
}
/*
orfun : 原始函数地址
myfun : 自己的函数地址
*/
int iathook(PROC orfun, PROC myfun) {
    DWORD  oldProtect;
    //修改CreateFileA的内存地址块 5个大小为可读写
    if (!VirtualProtect(orfun, 5, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        printf("VirtualProtect false\n");
        return 0;
    }
    //备份原始函数的前5个字节
    memcpy(createfileback, orfun, 5);
    //0xe9为汇编代码jmp的二进制值
    unsigned char jmp_0xe9 = 0xE9;//od  call-----jmp
    //这是一个jmp用的偏移地址，从CreateFileA位置跳转到MY_CreateFileA
    unsigned int jmp_addr = (char*)myfun - ((char*)orfun + 5); //jmp到我们空白的代码上来

    //call jmp jnz   地址 都是偏移量
    //将target 原CreateFileA 的内存前五个 改写，覆盖为一个jmp指令，和一个jmp需要的偏移地址，刚好五个字节大小
    memcpy(orfun, &jmp_0xe9, 1);// e8----------------------e9
    memcpy((char*)orfun + 1, &jmp_addr, 4);//12 34 56 78 ---------detour
    //恢复原内存保护属性
    VirtualProtect(orfun, 5, oldProtect, &oldProtect);
    return 1;
}
BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {

        PROC myfun = (PROC)&MY_CreateFileA;
        PROC orgfun = (PROC)GetProcAddress(GetModuleHandle("kernel32.dll"), "CreateFileA");
        int res = iathook(orgfun, myfun);
        if (res == 1) {
            printf("hook succesed\n");
        }
        else {
            printf("hook failed\n");
        }
        break;
    }

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

