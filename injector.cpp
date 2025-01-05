#include <Windows.h>
#include <tlhelp32.h>
#include <iostream>
using namespace std;


char *get_dll_name(char *dllPath) {
    char *dllName = dllPath;
    for (int i = strlen(dllPath) - 1; i >= 0; i--) {
        if (dllPath[i] == '\\') {
            dllName = dllPath + i + 1;
            break;
        }
        if (i == 0) 
            dllName = dllPath;
    }
    return dllName;
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        cerr << "Usage: " << argv[0] << " <process_name>" << " <.dll path>" << endl;
        return 1;
    }
    char *processName = argv[1];
    char *dllPath = argv[2];
    HANDLE snapshot = 0;
    PROCESSENTRY32 pe32 = { 0 };

    pe32.dwSize = sizeof(PROCESSENTRY32);
    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    
    int i = 0;

    Process32First(snapshot, &pe32);
    do {
        if (strcmp((const char *)pe32.szExeFile, processName) == 0) {
            // cerr << process_name << " found" << endl;
            i = 1;
            break ;
        }
    } while (Process32Next(snapshot, &pe32));

    if (i == 0) {
        cerr << processName << " not found" << endl;
        return 1;
    }

    HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
    if(process == NULL) {
        cerr << "OpenProcess failed" << endl;
        CloseHandle(snapshot);
        return 1;
    }

    if (GetModuleHandle(get_dll_name(dllPath)) != NULL) {
        cerr << "DLL already injected!" << endl;
        CloseHandle(process);
        CloseHandle(snapshot);
        return 1;
    }
    
    void *lpBaseAddress = VirtualAllocEx(process, NULL, strlen(dllPath) + 1, MEM_COMMIT, PAGE_READWRITE);
    if (lpBaseAddress == NULL) {
        cerr << "VirtualAllocEx failed" << endl;
        CloseHandle(process);
        CloseHandle(snapshot);
        return 1;
    }

    if (WriteProcessMemory(process, lpBaseAddress, dllPath, strlen(dllPath) + 1, NULL) == 0) {
        cerr << "WriteProcessMemory failed (look .dll path)" << endl;
        VirtualFreeEx(process, lpBaseAddress, 0, MEM_RELEASE);
        CloseHandle(process);
        CloseHandle(snapshot);
        return 1;
    }

    HMODULE kernel32 = GetModuleHandle("kernel32.dll");
    if (kernel32 == NULL) {
        cerr << "GetModuleHandle for kernel32 failed" << endl;
        VirtualFreeEx(process, lpBaseAddress, 0, MEM_RELEASE);
        CloseHandle(process);
        CloseHandle(snapshot);
        return 1;
    }

    const FARPROC lpFunctionAdress = GetProcAddress(kernel32, "LoadLibraryA");
    if (lpFunctionAdress == NULL) {
        cerr << "GetProcAddress for LoadLibraryA failed" << endl;
        VirtualFreeEx(process, lpBaseAddress, 0, MEM_RELEASE);
        CloseHandle(process);
        CloseHandle(snapshot);
        return 1;
    }

    HANDLE thread = CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)lpFunctionAdress, lpBaseAddress, 0, NULL);
    if (thread == NULL || thread == INVALID_HANDLE_VALUE) {
        cerr << "CreateRemoteThread failed" << endl;
        return 1;
    }

    // DWORD exitCode = 0;
    // WaitForSingleObject(thread, INFINITE);
    // // GetExitCodeThread(thread, (LPDWORD)&exitCode);

    // VirtualFreeEx(process, lpBaseAddress, 0, MEM_RELEASE);
    // CloseHandle(thread);
    // CloseHandle(process);
    cerr << "DLL injected!" << endl;
    return 0;
}