#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Windows.h>

int main(int argc, char** argv)
{
	SetConsoleTitleA("绕过信任模式使用n卡滤镜");
	puts("绕过信任模式 BY 宇宙遨游");
	puts("正在等待游戏启动...");
	HWND hHwnd = NULL;
	while (!hHwnd)
	{
		hHwnd = FindWindowA(NULL, "Counter-Strike: Global Offensive");
		Sleep(1000);
	}
	puts("游戏已启动...");
	int pid=NULL;
	GetWindowThreadProcessId(hHwnd,&pid);
	if (pid == NULL)return;
	int address = GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtOpenFile");
	char original[5],source[5];
	memcpy(original, address,sizeof(original));
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	ReadProcessMemory(hProcess, address, source, sizeof(source), 0);
	if (WriteProcessMemory(hProcess, address, original, sizeof(original), 0)) {
		puts("成功操作");
	}
	else {
		puts("写入失败,请尝试管理员权限运行!");
		exit(0);
	}
	puts("请在滤镜开启后按回车键!");
	system("pause");
	WriteProcessMemory(hProcess, address, source, sizeof(source), 0);

	CloseHandle(hProcess);
	return 0;
}

