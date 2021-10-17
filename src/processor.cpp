#include "processor.h"

// Return the aggregate CPU utilization
double Processor::Utilization() { 
    std::vector<string> v = LinuxParser::CpuUtilization();
    if (!v.empty()) {
        double total{1.0}, idle{1.0};
        std::for_each(v.begin(), v.begin() + 8, [&](string s){ total += std::stod(s); });
        std::for_each(v.begin() + 3, v.begin() + 5, [&](string s){ idle += std::stod(s); });
        double utilization = ((total-prevTotal_) - (idle-prevIdle_)) / (total-prevTotal_);
        prevTotal_ = total;
        prevIdle_ = idle;
        return utilization;
    }
    return 0.0; 
    }