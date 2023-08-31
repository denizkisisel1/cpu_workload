#include <iostream>
#include <windows.h>

int main() {
    PDH_HQUERY cpuQuery;
    PDH_HCOUNTER cpuTotal;
    // Initialize the PDH library
    PdhOpenQuery(NULL, NULL, &cpuQuery);
    PdhAddCounter(cpuQuery, "\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
    PdhCollectQueryData(cpuQuery);

    while (true) {
        PDH_FMT_COUNTERVALUE counterVal;
        PdhCollectQueryData(cpuQuery);
        PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
        
        std::cout << "Current CPU Usage: " << counterVal.doubleValue << "%" << std::endl;
        Sleep(1000);  // Sleep for 1 second
    }
    PdhCloseQuery(cpuQuery);    // Clean up
    return 0;
}
