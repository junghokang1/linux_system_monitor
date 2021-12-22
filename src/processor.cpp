#include "processor.h"
#include "linux_parser.h"
    
// Return the aggregate CPU utilization
float Processor::Utilization() { 
    std::vector<std::string> vCpuTimes = LinuxParser::CpuUtilization();
    float totalCpuTime=0.0, idleCpuTime=0.0;

    for (int iter=0; iter<(int)vCpuTimes.size() ; iter++){
        totalCpuTime += std::stof(vCpuTimes[iter]);
    }

    idleCpuTime = std::stof(vCpuTimes[LinuxParser::kIdle_]) + std::stof(vCpuTimes[LinuxParser::kIOwait_]);
    
    // Updated CPU time
    float newTotalCpuTime = totalCpuTime - oldTotalCpuTime_;
    float newIdleCpuTime = idleCpuTime - oldIdleCpuTime_;

    // Save the current CPU time for the next call
    oldTotalCpuTime_ = totalCpuTime;
    oldIdleCpuTime_ = idleCpuTime;

    return (newTotalCpuTime - newIdleCpuTime)/newTotalCpuTime;
}