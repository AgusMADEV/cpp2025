#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <iomanip>
#include <string>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <map>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#endif

using namespace std;
using namespace chrono;

// Constants
const int MONITOR_DURATION_SEC = 30;  // Total monitoring duration in seconds
const int UPDATE_INTERVAL_SEC = 1;    // Interval between updates in seconds

// ANSI color codes
const string RESET = "\033[0m";
const string RED = "\033[31m";
const string GREEN = "\033[32m";
const string YELLOW = "\033[33m";
const string BLUE = "\033[34m";
const string MAGENTA = "\033[35m";
const string CYAN = "\033[36m";
const string WHITE = "\033[37m";

// Function to print a progress bar
void printProgressBar(int progress, int total, const string& color) {
    const int barWidth = 50;
    float percentage = static_cast<float>(progress) / total;
    int filledWidth = static_cast<int>(barWidth * percentage);

    cout << color;
    cout << "[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < filledWidth) {
            cout << "="; // Filled portion
        } else {
            cout << " "; // Empty portion
        }
    }
    cout << "] " << int(percentage * 100.0) << "%" << RESET << endl;
}

// Function to print header with visual markers
void printHeader(int iteration, int totalIterations) {
    system("cls");  // Clear screen (Windows)
    cout << MAGENTA << "=== System Resource Monitor ===" << RESET << endl;
    cout << CYAN << "Iteration: " << iteration << "/" << totalIterations << RESET << endl;
    cout << "-----------------------------" << endl;
}

// Function to get CPU load
bool getCPULoad() {
#ifdef _WIN32
    FILETIME idleTime, kernelTime, userTime;
    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        cerr << RED << "Error getting system times." << RESET << endl;
        return false;
    }

    ULARGE_INTEGER idle, kernel, user, prevIdle, prevKernel, prevUser;
    idle.LowPart = idleTime.dwLowDateTime;
    idle.HighPart = idleTime.dwHighDateTime;
    kernel.LowPart = kernelTime.dwLowDateTime;
    kernel.HighPart = kernelTime.dwHighDateTime;
    user.LowPart = userTime.dwLowDateTime;
    user.HighPart = userTime.dwHighDateTime;

    static FILETIME prevIdleTime, prevKernelTime, prevUserTime;
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
        cout << GREEN << "CPU Usage: " << fixed << setprecision(1) << cpuUsage << "%" << RESET << endl;
        printProgressBar(static_cast<int>(cpuUsage), 100, GREEN);
    }

    prevIdleTime = idleTime;
    prevKernelTime = kernelTime;
    prevUserTime = userTime;
#else
    // Linux implementation for CPU load
    ifstream statFile("/proc/stat");
    string line;
    static unsigned long long prevTotal = 0, prevIdle = 0;

    if (getline(statFile, line)) {
        istringstream iss(line);
        string cpu;
        unsigned long long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;

        iss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;

        unsigned long long total = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;
        unsigned long long totalDiff = total - prevTotal;
        unsigned long long idleDiff = idle - prevIdle;

        if (totalDiff > 0) {
            double cpuUsage = 100.0 - (100.0 * idleDiff) / totalDiff;
            cout << GREEN << "CPU Usage: " << fixed << setprecision(1) << cpuUsage << "%" << RESET << endl;
            printProgressBar(static_cast<int>(cpuUsage), 100, GREEN);
        }

        prevTotal = total;
        prevIdle = idle;
    }
#endif

    return true;
}

// Function to get RAM load
bool getRAMLoad() {
#ifdef _WIN32
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    if (!GlobalMemoryStatusEx(&memInfo)) {
        cerr << RED << "Error getting memory info." << RESET << endl;
        return false;
    }

    double usedPercent = static_cast<double>(memInfo.dwMemoryLoad);
    cout << BLUE << "RAM - Total: " << memInfo.ullTotalPhys / (1024 * 1024) << " MB, "
         << "Used: " << (memInfo.ullTotalPhys - memInfo.ullAvailPhys) / (1024 * 1024) << " MB ("
         << usedPercent << "%)" << RESET << endl;
    printProgressBar(static_cast<int>(usedPercent), 100, BLUE);
#else
    // Linux implementation for RAM load
    struct sysinfo memInfo;
    sysinfo(&memInfo);

    double totalRAM = memInfo.totalram;
    totalRAM /= (1024 * 1024);
    double freeRAM = memInfo.freeram;
    freeRAM /= (1024 * 1024);
    double usedRAM = totalRAM - freeRAM;
    double usedPercent = (usedRAM / totalRAM) * 100;

    cout << BLUE << "RAM - Total: " << totalRAM << " MB, "
         << "Used: " << usedRAM << " MB ("
         << usedPercent << "%)" << RESET << endl;
    printProgressBar(static_cast<int>(usedPercent), 100, BLUE);
#endif

    return true;
}

// Function to get disk info
bool getDiskInfo() {
#ifdef _WIN32
    DWORD drives = GetLogicalDrives();
    bool success = true;

    for (char d = 'A'; d <= 'Z'; ++d) {
        if (drives & (1 << (d - 'A'))) {
            char drivePath[] = { d, ':', '\\', '\0' };
            ULARGE_INTEGER freeBytes, totalBytes, freeBytesToCaller;

            if (GetDiskFreeSpaceExA(drivePath, &freeBytesToCaller, &totalBytes, &freeBytes)) {
                double usedPercent = 100.0 - (100.0 * freeBytes.QuadPart / totalBytes.QuadPart);
                cout << YELLOW << "Drive " << d << ": " << totalBytes.QuadPart / (1024 * 1024) << " MB total, "
                     << freeBytes.QuadPart / (1024 * 1024) << " MB free ("
                     << fixed << setprecision(1) << usedPercent << "%)" << RESET << endl;
                printProgressBar(static_cast<int>(usedPercent), 100, YELLOW);
            } else {
                cerr << RED << "Error getting disk info for drive " << d << RESET << endl;
                success = false;
            }
        }
    }
#else
    // Linux implementation for disk info
    struct statvfs stat;

    if (statvfs("/", &stat) == 0) {
        double totalSpace = stat.f_blocks * stat.f_frsize / (1024 * 1024);
        double freeSpace = stat.f_bfree * stat.f_frsize / (1024 * 1024);
        double usedSpace = totalSpace - freeSpace;
        double usedPercent = (usedSpace / totalSpace) * 100;

        cout << YELLOW << "Drive /: " << totalSpace << " MB total, "
             << freeSpace << " MB free ("
             << fixed << setprecision(1) << usedPercent << "%)" << RESET << endl;
        printProgressBar(static_cast<int>(usedPercent), 100, YELLOW);
    }
#endif

    return true;
}

// Function to get network load
bool getNetworkLoad() {
#ifdef _WIN32
    PMIB_IFTABLE ifTable;
    ULONG size = 0;

    if (GetIfTable(NULL, &size, FALSE) != ERROR_INSUFFICIENT_BUFFER) {
        cerr << RED << "Error getting interface table size." << RESET << endl;
        return false;
    }

    ifTable = (PMIB_IFTABLE)malloc(size);
    if (!ifTable || GetIfTable(ifTable, &size, FALSE) != NO_ERROR) {
        cerr << RED << "Error getting interface table." << RESET << endl;
        free(ifTable);
        return false;
    }

    static PMIB_IFTABLE prevIfTable = nullptr;

    if (prevIfTable) {
        for (DWORD i = 0; i < ifTable->dwNumEntries; ++i) {
            for (DWORD j = 0; j < prevIfTable->dwNumEntries; ++j) {
                if (ifTable->table[i].dwIndex == prevIfTable->table[j].dwIndex) {
                    DWORD inDiff = ifTable->table[i].dwInOctets - prevIfTable->table[j].dwInOctets;
                    DWORD outDiff = ifTable->table[i].dwOutOctets - prevIfTable->table[j].dwOutOctets;

                    // Only display interfaces with non-zero traffic
                    if (inDiff > 0 || outDiff > 0) {
                        cout << CYAN << "Interface " << ifTable->table[i].dwIndex << ": "
                             << "In: " << inDiff / UPDATE_INTERVAL_SEC << " B/s, "
                             << "Out: " << outDiff / UPDATE_INTERVAL_SEC << " B/s" << RESET << endl;
                    }
                    break;
                }
            }
        }
    }

    if (prevIfTable) free(prevIfTable);
    prevIfTable = ifTable;
#else
    // Linux implementation for network load
    struct ifaddrs *ifaddr, *ifa;
    static map<string, unsigned long> prevInBytes, prevOutBytes;

    if (getifaddrs(&ifaddr) == -1) {
        cerr << RED << "Error getting interface addresses." << RESET << endl;
        return false;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL || ifa->ifa_addr->sa_family != AF_PACKET)
            continue;

        string interfaceName = ifa->ifa_name;
        unsigned long inBytes = 0, outBytes = 0;

        ifstream statFile("/proc/net/dev");
        string line;
        while (getline(statFile, line)) {
            if (line.find(interfaceName) == 0) {
                istringstream iss(line);
                string iface;
                unsigned long dummy;
                iss >> iface;
                for (int i = 0; i < 8; ++i) {
                    if (i == 0) iss >> inBytes;
                    else if (i == 8) iss >> outBytes;
                    else iss >> dummy;
                }
                break;
            }
        }

        if (prevInBytes.find(interfaceName) != prevInBytes.end()) {
            unsigned long inDiff = inBytes - prevInBytes[interfaceName];
            unsigned long outDiff = outBytes - prevOutBytes[interfaceName];

            if (inDiff > 0 || outDiff > 0) {
                cout << CYAN << "Interface " << interfaceName << ": "
                     << "In: " << inDiff / UPDATE_INTERVAL_SEC << " B/s, "
                     << "Out: " << outDiff / UPDATE_INTERVAL_SEC << " B/s" << RESET << endl;
            }
        }

        prevInBytes[interfaceName] = inBytes;
        prevOutBytes[interfaceName] = outBytes;
    }

    freeifaddrs(ifaddr);
#endif

    return true;
}

int main() {
    int totalIterations = MONITOR_DURATION_SEC / UPDATE_INTERVAL_SEC;

    for (int i = 1; i <= totalIterations; ++i) {
        printHeader(i, totalIterations);

        bool success = true;
        success &= getCPULoad();
        success &= getRAMLoad();
        success &= getDiskInfo();
        success &= getNetworkLoad();

        if (!success) {
            cerr << RED << "Errors occurred while getting some metrics." << RESET << endl;
        }

        if (i < totalIterations) {
            this_thread::sleep_for(seconds(UPDATE_INTERVAL_SEC));
        }
    }

    cout << MAGENTA << "\nMonitoring completed. Duration: " << MONITOR_DURATION_SEC << " seconds." << RESET << endl;

    return 0;
}
