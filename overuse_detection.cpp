#include <iostream>
#include <windows.h>
#include <pdh.h>
#include <thread>

class CPUMonitor {
public:
    CPUMonitor() {
        PdhOpenQuery(NULL, NULL, &queryHandle_);
        PdhAddCounter(queryHandle_, "\\Processor(_Total)\\% Processor Time", NULL, &counterHandle_);
        PdhCollectQueryData(queryHandle_);
    }
    ~CPUMonitor() {
        PdhCloseQuery(queryHandle_);
    }
    int CheckCPUUsage() {
        PdhCollectQueryData(queryHandle_);
        PDH_FMT_COUNTERVALUE counterValue;
        PdhGetFormattedCounterValue(counterHandle_, PDH_FMT_DOUBLE, NULL, &counterValue);
        double currentCPUUsage = counterValue.doubleValue;
        if (currentCPUUsage > 95.0) {
            return 1;
        }
        if (currentCPUUsage > 65.0) {
            // Check if it's been over 7 minutes
            auto now = std::chrono::system_clock::now();
            if (now - startTime_ > std::chrono::minutes(7)) {
                return 1;
            }
        } else {
            startTime_ = std::chrono::system_clock::now();
        }
        return 0;
    }
private:
    PDH_HQUERY queryHandle_;
    PDH_HCOUNTER counterHandle_;
    std::chrono::time_point<std::chrono::system_clock> startTime_;
};
int main() {
    CPUMonitor cpuMonitor;
    while (true) {
        int result = cpuMonitor.CheckCPUUsage();
        if (result == 1) {
            std::cout << "CPU usage is over 95% or over 65% for more than 7 minutes. Returning 1." << std::endl;
            return 1;
        }
        std::this_thread::sleep_for(std::chrono::seconds(10)); // Check CPU usage every 10 seconds
    }
    return 0;
}
