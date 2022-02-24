#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <iostream>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"
#include "format.h"

#include<iostream>

using std::set;
using std::size_t;
using std::string;
using std::vector;

// Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() { 
    vector<int> pids = LinuxParser::Pids();
    // define pids
    set<int> new_pids;
    for (Process& k : processes_){ new_pids.insert(k.Pid());}
    // add new pids if they are not already exist
    for(int pid : pids){
        if(new_pids.find(pid) == new_pids.end()){
            processes_.emplace_back(pid);
        }
    }

    // calculate CPU Utilization for each process
    for(size_t i=0; i<processes_.size(); ++i){
        processes_[i].CpuUtilization();
    }
    // Sort the vector of processes
    std::sort(processes_.begin(),processes_.end());
    return processes_; 
}

// Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }
