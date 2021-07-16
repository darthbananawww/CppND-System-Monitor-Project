#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <algorithm>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

Processor& System::Cpu() {
  return cpu_; 
}
vector<Process>& System::Processes() { 
    processes_.clear();
    vector<int> processes = LinuxParser::Pids();
    for(unsigned int i = 0; i < processes.size(); i++){
        processes_.push_back(processes[i]);
    }
    std::sort(processes_.begin(), processes_.end(), CpuSort);
    return processes_;
}

bool System::CpuSort (Process i, Process j) { return i.CpuUtilization() > j.CpuUtilization(); }

std::string System::Kernel() { return LinuxParser::Kernel(); }

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }