#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// Read data from the filesystem for Operating System
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// Read data from the filesystem for Kernel
std::string LinuxParser::Kernel() {
  std::string os, kernel, version;
  std::string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> os >> version >> kernel) {
      return kernel;
    }
  }
  return "bad_kernel";
}

// Get PIDs of all currently running processes
vector<int> LinuxParser::Pids() {
  std::vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = std::stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  float memTotal = 0.0;
  float memFree = 0.0;
  float memAvailable = 0.0;
  float buffers = 0.0;

  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    std::string line, item, itemsize, unit;
    try {
      while (std::getline(stream, line)) {
        std::istringstream linestream(line);
        while (linestream >> item >> itemsize >> unit) {
          // Remove space just in case
          itemsize.erase(std::remove(itemsize.begin(), itemsize.end(), ' '), itemsize.end());
          if (item == "MemTotal:") {
            memTotal = std::stof(itemsize);
          } else if (item == "MemFree:") {
            memFree = std::stof(itemsize);
          } else if (item == "MemAvailable:"){
            memAvailable = std::stof(itemsize);
          } else if (item == "Buffers:") {
            buffers = std::stof(itemsize);
          } else {
            if (memTotal*memFree*memAvailable*buffers != 0.0) {
              break;
            }
            else {
              continue;
            }
          }
        }
      }
      return (memTotal-memAvailable)/memTotal; 
    } catch (const std::invalid_argument& arg) {
      return 0.0;
    }
  }
  return 0.0;
}

// Read and return the system uptime
long LinuxParser::UpTime() { 
  std::string upTime, idleTime;
  std::string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);

  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      while(linestream >> upTime >> idleTime) {
        return std::stol(upTime);
      }
    }
  }
  return 1;
}


// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// Read and return CPU utilization
std::vector<std::string> LinuxParser::CpuUtilization() { 
  std::ifstream stream(kProcDirectory + kStatFilename);
  std::vector<std::string> vCpuStates(10);
  std::string key;

  if (stream.is_open()) {
    std::string line;
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while(linestream >> key >> vCpuStates[0] >> vCpuStates[1] >> vCpuStates[2] >> 
            vCpuStates[3] >> vCpuStates[4] >> vCpuStates[5] >> vCpuStates[6] >> 
            vCpuStates[7] >> vCpuStates[8] >> vCpuStates[9]) {
        if(key=="cpu") {
          return vCpuStates;
        }
      }
    }
  }
  return {};
}

// Read process-specific process utilization
float LinuxParser::ProcessCpuUtilization(int pid){
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatFilename);

  if (stream.is_open()) {
    float utime=-0.099, stime=-0.199, cutime=-0.299, cstime=-0.399, starttime=-0.499;
    float total_time, seconds, hertz, cpu_usage;
    std::string line, value;
    try {
      while (std::getline(stream, line)) { 
        std::istringstream linestream(line);
        for (int iter = 1; iter <= 22; iter++) {
          linestream >> value;
          value.erase(std::remove(value.begin(), value.end(), ' '), value.end());
          if (iter != 14 && iter != 15 && iter != 16 && iter != 17 && iter != 22) {
            continue;
          }
          else if (iter == 14) {
            utime = std::stof(value);    
          } 
          else if (iter == 15) {
            stime = std::stof(value);
          }
          else if (iter == 16) {
            cutime = std::stof(value);
          }
          else if (iter == 17) {
            cstime = std::stof(value);
          }
          else if (iter == 22) {
            starttime = std::stof(value);
          }
          else {
            continue;
          }
        }

        hertz = (float)sysconf(_SC_CLK_TCK);
        total_time = (float)(utime + stime + cutime + cstime);
        seconds = (float)LinuxParser::UpTime() - starttime/hertz;
        cpu_usage = total_time/hertz/seconds;

        return cpu_usage;
      }
    } catch (const std::invalid_argument& arg) {
      return 0.0;
    }
  }
  return 0.0;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  int totalProc = 0;
  if (stream.is_open()) {
    std::string line, item, value;
    
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> item >> value) {
        if (item == "processes") {
          totalProc = std::stoi(value); 
        } else {
          if (totalProc != 0) {
            break;
          } else {
            continue;
          }
        }
      }
    }
  }
  return totalProc; 
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  std::ifstream stream(kProcDirectory + kStatFilename);
  int procNum = 0;
  if (stream.is_open()) {
    std::string line, item, value;
    
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> item >> value) {
        if (item == "procs_running") {
          procNum = std::stoi(value); 
        } else {
          if (procNum != 0) {
            break;
          } else {
            continue;
          }
        }
      }
    }
  }
  return procNum; 
}

// Read and return the command associated with a process
std::string LinuxParser::Command(int pid) { 
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kCmdlineFilename);

  std::string line;

  if (stream.is_open()) {
    std::getline(stream, line);
    return line;   
  }

  return "bad_cmd"; 
}

// Read and return the memory used by a process
std::string LinuxParser::Ram(int pid) { 
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatusFilename);

  if (stream.is_open()) {
    std::string key, value, line;
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize") {
          return value;
        }
      }
    }
  }
  return "bad_ram"; 
}

// Read and return the user ID associated with a process
std::string LinuxParser::Uid(int pid) { 
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatusFilename);
    
  if (stream.is_open()) {
    std::string key, line, number;
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> number) {
        if (key == "Uid") {
          return number;
        }
      }
    }
  }

  return "bad_uid"; 
}

// Read and return the user associated with a process
std::string LinuxParser::User(int pid) { 
  std::string uid = LinuxParser::Uid(pid);

  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    std::string line, user, unknown, uid_value;
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user >> unknown >> uid_value) {
        if (uid_value == uid) {
          return user;
        }
      }
    }
  }
  return "bad_user"; 
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatFilename);

  if (stream.is_open()) {
    
    //float total_time, seconds, hertz, cpu_usage;
    std::string line, value;
    
    try {
      while (std::getline(stream, line)) { 
        std::istringstream linestream(line);
        for (int iter = 1; iter <= 22; iter++) {
          linestream >> value;
          if (iter < 22) {
            continue;
          }
          else {
            return std::stol(value)/sysconf(_SC_CLK_TCK);
          }
        }
      }
    } catch (const std::invalid_argument& arg) {
      return -100;
    }
  }
  return -200; 
}