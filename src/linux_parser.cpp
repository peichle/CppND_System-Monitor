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
  string line, key, value;
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

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) { 
  long utime, stime, cutime, cstime;
  string temp, line;
  vector<string> pidData;
  std::ifstream pidstream(kProcDirectory + to_string(pid) + kStatFilename);
  if (pidstream.is_open()){
      std::getline(pidstream, line);
    for (unsigned int i=0;i<line.length();i++){
      if (line[i]==' '){
        pidData.push_back(temp);
        temp = "";
      }
      else{
        temp.push_back(line[i]);
      }
    }
    pidData.push_back(temp);
  }
  utime = stol(pidData[14]);
  stime = stol(pidData[15]);
  cutime = stol(pidData[16]);
  cstime = stol(pidData[17]);
  long activejiffs = utime+stime+cutime+cstime;
  return activejiffs;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  std::vector<std::string> all_jiffies = CpuUtilization();
  long activ{0};
  // sum up all active jiffies
  for(auto k : all_jiffies){
    activ += std::stol(k);
  }
  return activ; 
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  std::vector<std::string> all_jiffies = CpuUtilization();
  return std::stol(all_jiffies[3]) + std::stol(all_jiffies[4]); 
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


vector<string> LinuxParser::CpuUtilization(int pid){
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatFilename);

  if (stream.is_open()) {
    std::string utime, stime, cutime, cstime, starttime;

    std::vector<string> times;

    std::string line, value;
    while (std::getline(stream, line)) { 
      std::istringstream linestream(line);
      for (int iter = 1; iter <= 22; iter++) {
        linestream >> value;
        value.erase(std::remove(value.begin(), value.end(), ' '), value.end());
        if (iter != 14 && iter != 15 && iter != 16 && iter != 17 && iter != 22) {
          continue;
        }
        else if (iter == 14) {
          if(value.size() < 1){continue;}
          utime = value;    
        } 
        else if (iter == 15) {
          if(value.size() < 1){continue;}
          stime = value;
        }
        else if (iter == 16) {
          if(value.size() < 1){continue;}

          cutime = value;
        }
        else if (iter == 17) {
          if(value.size() < 1){continue;}

          cstime = value;
        }
        else if (iter == 22) {
          if(value.size() < 1){continue;}

          starttime = value;
        }
        else {
          continue;
        }
      }
      return {utime, stime, cutime, cstime, starttime};
    }
     
  }
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
string LinuxParser::Command(int pid) { 
  string cmd;
  std::ifstream procstream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  std::getline(procstream, cmd);
  if (cmd == "")
  {
    return "NONE";
  }
  return cmd;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  string line, key, value;
  std::ifstream procstream(kProcDirectory + to_string(pid) + kStatusFilename);
  while (std::getline(procstream, line)) {
    std::istringstream linestream(line);
    linestream >> key >> value;
    if (key == "VmSize:") {
      // bad_ram according to https://knowledge.udacity.com/questions/774058
      if(value == "bad_ram"){
        std::cout << value << std::endl; 
        value = "0";
      } 
      int value_to_MB = 0.001 * std::stoi(value);
      value = std::to_string(value_to_MB);
      return value;
    }
  }
  return std::string();
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
  return "MISSING_uid"; 
}

std::string LinuxParser::User(int pid) { 
  std::string uid = LinuxParser::Uid(pid);

  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    std::string line, user, random, uid_value;
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user >> random >> uid_value) {
        if (uid_value == uid) {
          return user;
        }
      }
    }
  }
  return "MISSING_user"; 
}


// Read and return the uptime of a process
long int LinuxParser::UpTime(int pid) {
  string line, value;
  std::ifstream stream (kProcDirectory + to_string(pid) + kStatFilename);
  if(stream.is_open())
  {
    std::getline(stream,line);
    std::istringstream linestream(line);
    for(int i=0; i<22; i++){ // 22 value is the process Up Time
      linestream >> value;
    }
  }
  long int uptime = std::stol(value); 
  uptime = uptime / sysconf(_SC_CLK_TCK);
  return uptime; 
}  
