#include <windows.h>
#include <iphlpapi.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

#pragma comment(lib, "iphlpapi.lib")

using namespace std;

// Función para obtener la carga de la CPU
void getCPULoad() {
    FILETIME idleTime, kernelTime, userTime;
    if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        ULARGE_INTEGER idle, kernel, user;
        idle.LowPart = idleTime.dwLowDateTime;
        idle.HighPart = idleTime.dwHighDateTime;
        kernel.LowPart = kernelTime.dwLowDateTime;
        kernel.HighPart = kernelTime.dwHighDateTime;
        user.LowPart = userTime.dwLowDateTime;
        user.HighPart = userTime.dwHighDateTime;

        cout << "CPU Load - Idle: " << idle.QuadPart << ", Kernel: " << kernel.QuadPart << ", User: " << user.QuadPart << endl;
    }
}

// Función para obtener la carga de la RAM
void getRAMLoad() {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&memInfo)) {
        cout << "RAM Load - Total: " << memInfo.ullTotalPhys / (1024 * 1024) << " MB, Used: " << (memInfo.ullTotalPhys - memInfo.ullAvailPhys) / (1024 * 1024) << " MB" << endl;
    }
}

// Función para obtener información de los discos duros
void getDiskInfo() {
    DWORD drives = GetLogicalDrives();
    for (char d = 'A'; d <= 'Z'; ++d) {
        if (drives & (1 << (d - 'A'))) {
            char drivePath[] = { d, ':', '\\', '\0' };
            ULARGE_INTEGER freeBytesAvailable, totalNumberOfBytes, totalNumberOfFreeBytes;
            if (GetDiskFreeSpaceExA(drivePath, &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes)) {
                cout << "Drive: " << d << " Total: " << totalNumberOfBytes.QuadPart / (1024 * 1024) << " MB, Free: " << totalNumberOfFreeBytes.QuadPart / (1024 * 1024) << " MB" << endl;
            }
        }
    }
}

// Función para obtener la carga de red
void getNetworkLoad() {
    PMIB_IFTABLE ifTable;
    ULONG size = 0;
    GetIfTable(NULL, &size, FALSE);
    ifTable = (PMIB_IFTABLE)malloc(size);
    if (GetIfTable(ifTable, &size, FALSE) == NO_ERROR) {
        for (DWORD i = 0; i < ifTable->dwNumEntries; ++i) {
            cout << "Interface: " << ifTable->table[i].dwIndex << " In: " << ifTable->table[i].dwInOctets << " bytes, Out: " << ifTable->table[i].dwOutOctets << " bytes" << endl;
        }
    }
    free(ifTable);
}

int main() {
    while (true) {
        getCPULoad();
        getRAMLoad();
        getDiskInfo();
        getNetworkLoad();
        this_thread::sleep_for(chrono::seconds(1));
    }
    return 0;
}
