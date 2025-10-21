#include <windows.h>
#include <fstream>
#include <sstream>

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

LONG CALLBACK SmartVehHandler(EXCEPTION_POINTERS* ExceptionInfo) {
    if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION) {
        DWORD64 addr = (DWORD64)ExceptionInfo->ExceptionRecord->ExceptionAddress;
        WriteLog("[VEH] 捕获访问冲突异常, 地址: 0x" + std::to_string(addr));
        ExceptionInfo->ContextRecord->Rip += 2;
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        WriteLog("[DllMain] veh_patch_smart.dll 已成功注入，正在监控异常...");
        AddVectoredExceptionHandler(1, SmartVehHandler);
    }
    return TRUE;
}
