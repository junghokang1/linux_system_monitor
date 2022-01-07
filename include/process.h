#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  // Constructor function
  Process(int pid):pid_(pid) {}
  
  int Pid();                               
  std::string User();                      
  std::string Command();                   
  float CpuUtilization() const;            
  std::string Ram() const;                 
  long int UpTime();                       
  bool operator<(Process const& a) const;  

 private:
  int pid_;
  std::string user_;
  std::string command_;
  float cpuUtilization_;
  std::string ram_;
  long int upTime_;
};

#endif