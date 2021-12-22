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
    return LinuxParser::UpTime(pid_); 
}

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { 
    // First look at the RAM utilization to sort.
    if (this->Ram() != a.Ram()){
        return std::stof(a.Ram()) < std::stof(this->Ram()); 
    }

    // If the memory utilization is identical, look at CPU utilization.
    return a.CpuUtilization() < this->CpuUtilization();
} 