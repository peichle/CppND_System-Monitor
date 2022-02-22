#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
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

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
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
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string line, key;
  float value, memTotal, memFree;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if(stream.is_open())
  {
    while (std::getline(stream, line))
    {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:") {
          memTotal = value;
        }
        if (key == "MemFree:") {
          memFree = value;
        }
      }
    }
  }
  return (memTotal - memFree)/memTotal; 
}

// Read and return the system uptime
long LinuxParser::UpTime() { 
  string totalUptime, idleuptime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> totalUptime;
  }
  return std::stol(totalUptime);  
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  // Number of ticks the system made since the last boot 
  long uptime = UpTime();
  long sys_frequ = sysconf(_SC_CLK_TCK);

  return uptime * sys_frequ; 
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  string line, name, val;
  vector<string> pid_str {};
  long processjiffies{0};
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i =0 ; i < 17 ; i++)
    {
      linestream >> val ;
      pid_str.push_back(val);
    }
  }
  processjiffies = std::stoi(pid_str[13]) + std::stoi(pid_str[14]) +
                   std::stoi(pid_str[15]) + std::stoi(pid_str[16]);
  
  return processjiffies; 
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  std::vector<std::string> ticks_ = CpuUtilization();
  long activ{0};
  // sum up all active jiffies
  for(auto k : ticks_){
    activ += std::stol(k);
  }
  return activ; 
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  std::vector<std::string> ticks_ = CpuUtilization();
  return std::stol(ticks_[3]) + std::stol(ticks_[4]); 
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  string line;
  string key;
  vector<string> jiffies{};
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> key;
    while (linestream >> key) 
    {
      jiffies.emplace_back(key);
    }
  }
  return jiffies;
}

vector<string> LinuxParser::CpuUtilization(long a, long b) { 
  return {};
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line;
  string key;
  int value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      //std::cout << line << std::endl;
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return value;
        }
      }
    }
  } 
  return 0;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line;
  string key;
  int value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      //std::cout << line << std::endl;
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return value;
        }
      }
    }
  } 
  return 0;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) { return string(); }

// Read and return the uptime of a process
long int LinuxParser::UpTime(int pid) {
  string line;
  string val;
  int i= 0;
  long uptime = 0; 
  std::ifstream stream (kProcDirectory + to_string(pid) + kStatFilename);
  if(stream.is_open())
  {
    std::getline(stream,line);
    std::istringstream linestream(line);
    for(i=0; i<22; i++)
     linestream >>val;
  }
  uptime = std::stoi(val); 
  return uptime; 
}  
