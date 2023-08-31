#include <windows.h>
#include <shellapi.h>
#include <string>
#include <sstream>
#include <iostream>
#include <pdh.h>

NOTIFYICONDATA nid;
HANDLE hThread;
DWORD dwThread;

void ShowNotification(const std::wstring& title, const std::wstring& message) {
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.hWnd = GetConsoleWindow();
    nid.uID = 1;
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    nid.uCallbackMessage = WM_USER;
    nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcscpy_s(nid.szTip, message.c_str());
    Shell_NotifyIcon(NIM_ADD, &nid);
    Shell_NotifyIcon(NIM_MODIFY, &nid);
}

DWORD WINAPI UpdateCpuUsage(void*) {
    PDH_HQUERY cpuQuery;
    PDH_HCOUNTER cpuTotal;
    PdhOpenQuery(NULL, NULL, &cpuQuery);
    PdhAddCounter(cpuQuery, "\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
    PdhCollectQueryData(cpuQuery);

    while (true) {
        PDH_FMT_COUNTERVALUE counterVal;
        PdhCollectQueryData(cpuQuery);
        PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
        std::cout << "Current CPU Usage: " << counterVal.doubleValue << "%" << std::endl;

        double cpuUsage = counterVal.doubleValue; // Store CPU usage for notification
        std::wstringstream ss;
        ss << L"CPU Usage: " << cpuUsage << L"%";
        ShowNotification(L"CPU Monitor", ss.str());
        
        Sleep(1000);
    }
    
    PdhCloseQuery(cpuQuery);
    return 0;
}

int main() {
    hThread = CreateThread(NULL, 0, UpdateCpuUsage, NULL, 0, &dwThread);
    if (hThread == NULL) {
        return 1;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    CloseHandle(hThread);

    return 0;
}
