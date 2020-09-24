#include <Windows.h>
#include <TlHelp32.h>
#include <PsApi.h>
#include <string>

class Memory
{
public:
	unsigned long long BaseAddress(DWORD pid);
	DWORD GetProcessID(const std::wstring& pName);

    template<typename T>
    T Read(HANDLE hProcess, uintptr_t address)
    {
        T buffer;
        ReadProcessMemory(hProcess, (LPCVOID)address, &buffer, sizeof(T), NULL);
        return buffer;
    }

    template<typename T>
    void Write(HANDLE hProcess, uintptr_t address, T buffer)
    {
        WriteProcessMemory(hProcess, (LPVOID)address, &buffer, sizeof(buffer), NULL);
    }
};