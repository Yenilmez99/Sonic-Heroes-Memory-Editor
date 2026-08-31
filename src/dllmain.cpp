#include <windows.h>
#include <tlhelp32.h>
#include <filesystem>
#include <string>
#include <winuser.h>

bool IsProcessRunning(const std::wstring& processName) {
    bool exists = false;
    PROCESSENTRY32W entry;
    entry.dwSize = sizeof(PROCESSENTRY32W);

    // get snapshot
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return false;

    if (Process32FirstW(snapshot, &entry)) {
        do {
            // ignore upper/lower case with _wcsicmp
            if (_wcsicmp(entry.szExeFile, processName.c_str()) == 0) {
                exists = true;
                break;
            }
        } while (Process32NextW(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return exists;
}

DWORD WINAPI LaunchEditor(LPVOID lpParam) {

    if (IsProcessRunning(L"Sonic Heroes Memory Editor.exe")) {
        return 0;
    }

    HMODULE hModule = (HMODULE)lpParam;
    char dllPathStr[MAX_PATH];
    
    // get dlls full path (example: C:\Mods\MyMod\launcher.dll)
    GetModuleFileNameA(hModule, dllPathStr, MAX_PATH);
    
    // change it and find exes path
    std::filesystem::path dllPath(dllPathStr);
    std::filesystem::path exePath = dllPath.parent_path() / "Sonic Heroes Memory Editor.exe";

    STARTUPINFOA si = { sizeof(STARTUPINFOA) };
    PROCESS_INFORMATION pi;

    // run exe
    BOOL success = CreateProcessA(
        exePath.string().c_str(),
        NULL,                     
        NULL,                     
        NULL,                     
        FALSE,                    
        0,                        
        NULL,                     
        dllPath.parent_path().string().c_str(),
        &si,
        &pi
    );

    if (success) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule); // disable thread attach/detach calls for performance
        CreateThread(nullptr, 0, LaunchEditor, hModule, 0, nullptr);
    }
    return TRUE;
}