#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
using std::replace;
using std::getline;
using std::ifstream;
using std::istringstream;
using std::stoi;

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (getline(filestream, line)) {
      replace(line.begin(), line.end(), ' ', '_');
      replace(line.begin(), line.end(), '=', ' ');
      replace(line.begin(), line.end(), '"', ' ');
      istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  string os;
  string kernel;
  string version;
  string line;
  ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    getline(stream, line);
    istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.emplace_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  long memTotal;
  long memFree;
  float memUsed;
  string line, key, value;
  ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (getline(filestream, line)) {
      istringstream linestream(line); 
      while (linestream >> key >> value) {
        if (key == "MemTotal:") { memTotal = stoi(value); }
        if (key == "MemFree:") { memFree = stoi(value); }
      }
    }
  }
  memUsed = memTotal - memFree;
  
  return memUsed/memTotal;
}

long int LinuxParser::UpTime() { 
  long seconds = 0;
  string line;
  ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    getline(stream, line);
    replace(line.begin(), line.end(), '.', ' ');
    istringstream linestream(line);
    linestream >> seconds;
  }
  return seconds;
}

long LinuxParser::Jiffies() { return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies(); }

long LinuxParser::ActiveJiffies(int pid) {
  string line;
  string value;
  string jiffie1, jiffie2, jiffie3, jiffie4;
  ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    getline(filestream, line);
    istringstream linestream(line);
    for (int i = 0; i < 12; i++) {
      linestream >> value;
    }
    linestream >> jiffie1;
    linestream >> jiffie2;
    linestream >> jiffie3;
    linestream >> jiffie4;
    return stol(jiffie1) + stol(jiffie2) + stol(jiffie3) + stol(jiffie4);
  }
  return 0;
}

long LinuxParser::ActiveJiffies() {
  vector<string> cpuData = LinuxParser::CpuUtilization();
  return (stol(cpuData[1]) + stol(cpuData[2]) + stol(cpuData[3]));
}

long LinuxParser::IdleJiffies() {
  vector<string> cpuData = LinuxParser::CpuUtilization();
  return (stol(cpuData[4]) + stol(cpuData[5]));
}

vector<string> LinuxParser::CpuUtilization() {
  vector<string> return_string;
  string line, value;
  ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    getline(stream, line);
    replace(line.begin(), line.end(), 'c', ' ');
    replace(line.begin(), line.end(), 'p', ' ');
    replace(line.begin(), line.end(), 'u', ' ');
    istringstream linestream(line);
    while(linestream >> value) {
      return_string.emplace_back(value);
    }
  }
  return return_string;
}

int LinuxParser::TotalProcesses() { return LinuxParser::Pids().size(); }

int LinuxParser::RunningProcesses() {
  string line;
  string key;
  string value;
  ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (getline(filestream, line)) {
      istringstream linestream(line); 
      while (linestream >> key >> value) {
        if(key == "procs_running") { return stoi(value); }
      }
    }
  }
  return 0;
}

string LinuxParser::Command(int pid) {
  string line;
  ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    getline(stream, line);
  }
  return line;
}

string LinuxParser::Ram(int pid) {
	string line;
  string key;
  long int value;
	ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
	if (filestream.is_open()) {
		while (getline(filestream, line)) {
			istringstream linestream(line); 
			while (linestream >> key >> value) {
				if(key == "VmRss:") { return to_string(value / 1024); } //VmRss used intead of VmSize for more accurate results
			}
		}
	}
  return string();
}

string LinuxParser::Uid(int pid) {
	string line;
  string key;
	int value;
	ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
	if (filestream.is_open()) {
		while (getline(filestream, line)) {
			istringstream linestream(line); 
			while (linestream >> key >> value) {
				if(key == "Uid:") { return to_string(value); }
			}
		}
	}
  return string();
}

string LinuxParser::User(int pid) {
	string line;
  string key;
  string value;
	ifstream filestream(kPasswordPath);
	if (filestream.is_open()) {
		while (getline(filestream, line)) {
			replace(line.begin(), line.end(), ':', ' ');
			replace(line.begin(), line.end(), 'x', ' ');
			istringstream linestream(line); 
			while (linestream >> key >> value) {
				if(value == LinuxParser::Uid(pid)) { return key; }
			}
		}
	}
  return string();
}

long LinuxParser::UpTime(int pid) {
  string line;
  string upTime;

  ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    getline(filestream, line);
    istringstream linestream(line);
    for (int steps = 0; steps < 22; steps++) {
      linestream >> upTime;
    }
    return LinuxParser::UpTime() - stol(upTime) / sysconf(_SC_CLK_TCK);
  }

  return stol(upTime);
}
