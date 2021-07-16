#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int p_id) : pid(p_id) {}

int Process::Pid() { return pid; }

float Process::CpuUtilization() { return float(LinuxParser::ActiveJiffies(pid)) / float(LinuxParser::Jiffies()); }

string Process::Command() { return LinuxParser::Command(pid); }

string Process::Ram() { return LinuxParser::Ram(pid); }

string Process::User() { return LinuxParser::User(pid); }

long int Process::UpTime() { return LinuxParser::UpTime(pid); }