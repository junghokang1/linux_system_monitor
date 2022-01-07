#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// Return this process's ID
int Process::Pid() { 
    return pid_; 
}

// Return this process's CPU utilization
float Process::CpuUtilization() const { 
    return LinuxParser::ProcessCpuUtilization(pid_); 
}

// Return the command that generated this process
std::string Process::Command() { 
    return LinuxParser::Command(pid_); 
}

// Return this process's memory utilization
std::string Process::Ram() const { 
    return LinuxParser::Ram(pid_); 
}

// Return the user (name) that generated this process
std::string Process::User() { 
    return LinuxParser::User(pid_);
}

// Return the age of this process (in seconds)
long int Process::UpTime() { 
    return LinuxParser::UpTime() - LinuxParser::UpTime(pid_); 
}

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { 
    std::string ramValue1 = this->Ram();
    std::string ramValue2 = a.Ram();
    ramValue1.erase(std::remove(ramValue1.begin(), ramValue1.end(), ' '), ramValue1.end());
    ramValue2.erase(std::remove(ramValue2.begin(), ramValue2.end(), ' '), ramValue2.end());

    // First look at the RAM utilization to sort.
    if (ramValue1 != ramValue2){
        try {
            return std::stof(ramValue2) < std::stof(ramValue1); 
        } catch (const std::invalid_argument& arg) {
            return true;
        }
    }

    // If the memory utilization is identical, look at CPU utilization.
    return a.CpuUtilization() < this->CpuUtilization();
} 