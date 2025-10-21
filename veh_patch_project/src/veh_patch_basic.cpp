#include <windows.h>
#include <fstream>
#include <sstream>

const DWORD64 TARGET_OFFSET = 0xE5EE7E;
DWORD64 g_ModuleBase = 0;

void WriteLog(const std::string& msg) {
    std::ofstream logFile("veh_patch.log", std::ios::app);
    if (logFile.is_open()) {
        SYSTEMTIME st;
        GetLocalTime(&st);
        logFile << "[" << st.wYear << "-" << st.wMonth << "-" << st.wDay << " "
                << st.wHour << ":" << st.wMinute << ":" << st.wSecond << "] "
                << msg << std::endl;
    }
}

LONG CALLBACK VehHandler(EXCEPTION_POINTERS* ExceptionInfo) {
    if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION) {
        DWORD64 crashAddr = (DWORD64)ExceptionInfo->ExceptionRecord->ExceptionAddress;
        DWORD64 targetAddr = g_ModuleBase + TARGET_OFFSET;

        if (crashAddr == targetAddr) {
            WriteLog("[VEH] 捕获到异常，地址: 0x" + std::to_string(crashAddr) + "，已跳过。");
            ExceptionInfo->ContextRecord->Rip += 2;
            return EXCEPTION_CONTINUE_EXECUTION;
        }
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        HMODULE hMain = GetModuleHandleA("worldserver.exe");
        if (hMain) {
            g_ModuleBase = (DWORD64)hMain;
            WriteLog("[DllMain] veh_patch_basic.dll 注入成功");
            AddVectoredExceptionHandler(1, VehHandler);
        }
    }
    return TRUE;
}
