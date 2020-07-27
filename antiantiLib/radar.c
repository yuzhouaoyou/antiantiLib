#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Windows.h>
#include <TlHelp32.h>

extern void scanSha(unsigned char* buf, int len, char* tezhen, int address);
extern void memFuck(int address);
HANDLE hProc;
//十六到十
long hexToDec(char* s)
{
	int i, t=0;
	long sum = 0;
	for (i = 0; s[i]; i++)
	{
		if (s[i] >= '0' && s[i] <= '9')
			t = s[i] - '0';
		if (s[i] >= 'a' && s[i] <= 'z')
			t = s[i] - 'a' + 10;
		if (s[i] >= 'A' && s[i] <= 'Z')
			t = s[i] - 'A' + 10;
		sum = sum * 16 + t;
	}
	return sum;
}

//获取模块
MODULEENTRY32* getMod(DWORD pid,WCHAR* modName) {
	HANDLE hand = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	if (!hand)return NULL;
	MODULEENTRY32 mi;
	mi.dwSize = sizeof(MODULEENTRY32); //第一次使用必须初始化成员
	BOOL bRet = Module32First(hand, &mi);
	while (bRet)
	{
		if (!lstrcmpW(modName, mi.szModule)) break;
		
		bRet = Module32Next(hand, &mi);
	}
	CloseHandle(hand);
	return &mi;
}

//搜索内存
void scanMem(HANDLE hProc,MODULEENTRY32* mod, char* tezhen) {

	int address = mod->modBaseAddr;
	int len = mod->modBaseAddr + mod->modBaseSize;
	MEMORY_BASIC_INFORMATION mem;
	ZeroMemory(&mem, sizeof(mem));
	while (VirtualQueryEx(hProc, address, &mem, sizeof(mem)))
	{
		DWORD protect = mem.AllocationProtect;
		if (protect != PAGE_EXECUTE && protect != PAGE_NOACCESS && protect != PAGE_NOCACHE) {
			unsigned char* buff = malloc(mem.RegionSize);
			ReadProcessMemory(hProc, address, buff, mem.RegionSize, NULL);
			//todo 搜索特征
			//.......
			scanSha(buff, mem.RegionSize,tezhen, address);
			free(buff);
		}

		address += mem.RegionSize;
		if (address > len)
		{
			break;
		}
	}
	CloseHandle(hProc);
}

//获取十六进制长度
int getHexLenth(char* str) {
	int j = 0;
	for (size_t i = 0; i < str[i]; i+=2)
	{
		if (str[i] == ' ')continue;
		j++;
	}
	return j;
}

//搜索特征
void scanSha(unsigned char* buf,int len,char* tezhen,int address)
{
	int find = 0, hexLen = getHexLenth(tezhen);
	for (size_t i = 0; i < len; i++)
	{
		//计算剩余长度
		if (len - i < hexLen) break;
		//往后读hexLen个字节
		for (size_t j = 0, n = 0; j < hexLen; j++,n++)
		{
			char hex[3] = { tezhen[n++] ,tezhen[n++],0 };
			//对比字节
			if (!strcmp(hex, "??")) {
				find++;
			}
			else if (buf[i + j]!= hexToDec(hex)) {
				break;
			}
			else{
				find++;
			}
		}
		//查找个数是否对应
		if (find == hexLen) {
			memFuck(address + i);
		}
		find = 0;
	}
}

void memFuck(int address) {
	char hack = 2;
	WriteProcessMemory(hProc,address+6, &hack, sizeof(hack),0);
	CloseHandle(hProc);
	exit(0);
}

int main(int argc, char** argv)
{
	HWND hHwnd = FindWindowA(NULL, "Counter-Strike: Global Offensive");
	if (!hHwnd) {
		puts("未找到游戏!");
		getchar();
		exit(0);
	}
	int pid = NULL;
	GetWindowThreadProcessId(hHwnd, &pid);

	MODULEENTRY32* client = getMod(pid, L"client.dll");
	if (!client)return;
	hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);


	scanMem(hProc, client,"80 B9 ?? ?? ?? ?? ?? 74 12 8B 41 08");
	//char arr[] = { 1,2,3,4,5 };
	//scanSha(arr,5,"01 02 03 04 05");
	CloseHandle(hProc);
	puts("修改失败!");
	system("pause");
	return 0;
}

