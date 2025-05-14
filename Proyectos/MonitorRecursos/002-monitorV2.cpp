#include <windows.h>
#include <iphlpapi.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <iomanip>
#include <string>

#pragma comment(lib, "iphlpapi.lib")

using namespace std;
using namespace chrono;

// Constants
const int MONITOR_DURATION_SEC = 30;  // Total monitoring duration in seconds
const int UPDATE_INTERVAL_SEC = 1;    // Interval between updates in seconds

// Function to print a progress bar
void printProgressBar(int progress, int total) {
    const int barWidth = 50;
    float percentage = static_cast<float>(progress) / total;
    int filledWidth = static_cast<int>(barWidth * percentage);

    cout << "[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < filledWidth) {
            cout << "█";
        } else {
            cout << " ";
        }
    }
    cout << "] " << int(percentage * 100.0) << "%\r";
    cout.flush();
}

// Function to print header with visual markers
void printHeader(int iteration, int totalIterations) {
    system("cls");  // Clear screen (Windows)
    cout << "=== System Resource Monitor ===" << endl;
    cout << "Iteration: " << iteration << "/" << totalIterations << endl;
    cout << "-----------------------------" << endl;

    // Visual markers
    cout << "CPU Usage: ";
    printProgressBar(iteration, totalIterations);
    cout << endl;

    cout << "RAM Usage: ";
    printProgressBar(iteration, totalIterations);
    cout << endl;

    cout << "Disk Usage: ";
    printProgressBar(iteration, totalIterations);
    cout << endl;

    cout << "Network Load: ";
    printProgressBar(iteration, totalIterations);
    cout << endl;
}

// Function to get CPU load
bool getCPULoad(FILETIME& prevIdleTime, FILETIME& prevKernelTime, FILETIME& prevUserTime) {
    FILETIME idleTime, kernelTime, userTime;
    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        cerr << "Error getting system times." << endl;
        return false;
    }

    ULARGE_INTEGER idle, kernel, user, prevIdle, prevKernel, prevUser;
    idle.LowPart = idleTime.dwLowDateTime;
    idle.HighPart = idleTime.dwHighDateTime;
    kernel.LowPart = kernelTime.dwLowDateTime;
    kernel.HighPart = kernelTime.dwHighDateTime;
    user.LowPart = userTime.dwLowDateTime;
    user.HighPart = userTime.dwHighDateTime;

    prevIdle.LowPart = prevIdleTime.dwLowDateTime;
    prevIdle.HighPart = prevIdleTime.dwHighDateTime;
    prevKernel.LowPart = prevKernelTime.dwLowDateTime;
    prevKernel.HighPart = prevKernelTime.dwHighDateTime;
    prevUser.LowPart = prevUserTime.dwLowDateTime;
    prevUser.HighPart = prevUserTime.dwHighDateTime;

    ULONGLONG kernelDiff = kernel.QuadPart - prevKernel.QuadPart;
    ULONGLONG userDiff = user.QuadPart - prevUser.QuadPart;
    ULONGLONG idleDiff = idle.QuadPart - prevIdle.QuadPart;
    ULONGLONG totalDiff = kernelDiff + userDiff;

    if (totalDiff > 0) {
        double cpuUsage = 100.0 - (100.0 * idleDiff) / totalDiff;
        cout << fixed << setprecision(1);
        cout << "CPU Usage: " << cpuUsage << "%" << endl;
    }

    prevIdleTime = idleTime;
    prevKernelTime = kernelTime;
    prevUserTime = userTime;

    return true;
}

// Function to get RAM load
bool getRAMLoad() {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    if (!GlobalMemoryStatusEx(&memInfo)) {
        cerr << "Error getting memory info." << endl;
        return false;
    }

    double usedPercent = 100.0 - (memInfo.dwMemoryLoad * 100.0 / 1024.0);
    cout << "RAM - Total: " << memInfo.ullTotalPhys / (1024 * 1024) << " MB, "
         << "Used: " << (memInfo.ullTotalPhys - memInfo.ullAvailPhys) / (1024 * 1024) << " MB ("
         << memInfo.dwMemoryLoad << "%)" << endl;

    return true;
}

// Function to get disk info
bool getDiskInfo() {
    DWORD drives = GetLogicalDrives();
    bool success = true;

    for (char d = 'A'; d <= 'Z'; ++d) {
        if (drives & (1 << (d - 'A'))) {
            char drivePath[] = { d, ':', '\\', '\0' };
            ULARGE_INTEGER freeBytes, totalBytes, freeBytesToCaller;

            if (GetDiskFreeSpaceExA(drivePath, &freeBytesToCaller, &totalBytes, &freeBytes)) {
                double usedPercent = 100.0 - (100.0 * freeBytes.QuadPart / totalBytes.QuadPart);
                cout << "Drive " << d << ": " << totalBytes.QuadPart / (1024 * 1024) << " MB total, "
                     << freeBytes.QuadPart / (1024 * 1024) << " MB free ("
                     << fixed << setprecision(1) << usedPercent << "%)" << endl;
            } else {
                cerr << "Error getting disk info for drive " << d << endl;
                success = false;
            }
        }
    }

    return success;
}

// Function to get network load
bool getNetworkLoad(PMIB_IFTABLE& prevIfTable) {
    PMIB_IFTABLE ifTable;
    ULONG size = 0;

    if (GetIfTable(NULL, &size, FALSE) != ERROR_INSUFFICIENT_BUFFER) {
        cerr << "Error getting interface table size." << endl;
        return false;
    }

    ifTable = (PMIB_IFTABLE)malloc(size);
    if (!ifTable || GetIfTable(ifTable, &size, FALSE) != NO_ERROR) {
        cerr << "Error getting interface table." << endl;
        free(ifTable);
        return false;
    }

    if (prevIfTable) {
        for (DWORD i = 0; i < ifTable->dwNumEntries; ++i) {
            for (DWORD j = 0; j < prevIfTable->dwNumEntries; ++j) {
                if (ifTable->table[i].dwIndex == prevIfTable->table[j].dwIndex) {
                    DWORD inDiff = ifTable->table[i].dwInOctets - prevIfTable->table[j].dwInOctets;
                    DWORD outDiff = ifTable->table[i].dwOutOctets - prevIfTable->table[j].dwOutOctets;

                    cout << "Interface " << ifTable->table[i].dwIndex << ": "
                         << "In: " << inDiff / UPDATE_INTERVAL_SEC << " B/s, "
                         << "Out: " << outDiff / UPDATE_INTERVAL_SEC << " B/s" << endl;
                    break;
                }
            }
        }
    }

    if (prevIfTable) free(prevIfTable);
    prevIfTable = ifTable;

    return true;
}

int main() {
    FILETIME idleTime, kernelTime, userTime;
    PMIB_IFTABLE ifTable = nullptr;
    int totalIterations = MONITOR_DURATION_SEC / UPDATE_INTERVAL_SEC;

    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        cerr << "Initial error getting system times." << endl;
        return 1;
    }

    for (int i = 1; i <= totalIterations; ++i) {
        printHeader(i, totalIterations);

        bool success = true;
        success &= getCPULoad(idleTime, kernelTime, userTime);
        success &= getRAMLoad();
        success &= getDiskInfo();
        success &= getNetworkLoad(ifTable);

        if (!success) {
            cerr << "Errors occurred while getting some metrics." << endl;
        }

        if (i < totalIterations) {
            this_thread::sleep_for(seconds(UPDATE_INTERVAL_SEC));
        }
    }

    if (ifTable) free(ifTable);
    cout << "\nMonitoring completed. Duration: " << MONITOR_DURATION_SEC << " seconds." << endl;

    return 0;
}
