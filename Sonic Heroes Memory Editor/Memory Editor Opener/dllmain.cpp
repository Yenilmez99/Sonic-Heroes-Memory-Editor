#include <Windows.h>
#include <string>

std::string GetRunningFilePath(std::string FileName) {
    // Find file path by file name
    HMODULE hModule = NULL;
    char path[MAX_PATH];
    std::string FilePath = "";
    LPCSTR FileNameLPCSTR = FileName.c_str();

    GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_PIN, FileNameLPCSTR, &hModule);
    GetModuleFileNameA(hModule, path, MAX_PATH);
    FilePath = path;

    if (FilePath.find_last_of("\\/") != std::string::npos)
        FilePath = FilePath.substr(0, FilePath.find_last_of("\\/"));

    return FilePath;
}

DWORD WINAPI MainCore(HMODULE hModule) {
    std::string FilePath = GetRunningFilePath("Memory Editor Opener.dll") + "\\Sonic Heroes Memory Editor.exe";
    ShellExecuteA(NULL, "open", FilePath.c_str(), NULL, NULL, SW_SHOW);
    return 0;
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
        HANDLE hThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MainCore, hModule, 0, nullptr);
        if (hThread != nullptr) CloseHandle(hThread);
        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}