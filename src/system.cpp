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
    set<int> extant_pids;
    for (Process& k : processes_){ extant_pids.insert(k.Pid());}
    // add new pids if they are not already exist
    for(int pid : pids){
        if(extant_pids.find(pid) == extant_pids.end()){
            processes_.emplace_back(pid);
        }
    }
    // CPU Utilisation for each process
    // for(auto& process : processes_) {
    //     process.CpuUtilization();
    // }

    for(size_t i=0; i<processes_.size(); ++i){

        // if(i == 100){
        //     int k; 
        //     std::cin >> k;
        // }
        processes_[i].CpuUtilization();
    }
    // Eventuell sortieren
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
