#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include<iostream>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;


Process::Process(int pid) : pid_(pid){
    //cpuUtilization_ = CpuUtilization();
    com_ = LinuxParser::Command(pid_);
    user_ = LinuxParser::User(pid_);
    cpuUtilization_ = 0;
    prev_active_ticks = 0;
    prev_ticks = 0;    
}

// Return this process's ID
int Process::Pid() { return pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() {
// https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
    std::vector<string> times = LinuxParser::CpuUtilization(Pid());
    long total_time = std::stoi(times[0]) + std::stoi(times[1]);
    // adding children
    total_time = std::stoi(times[2]) + std::stoi(times[3]);
    long uptime = LinuxParser::UpTime(Pid());
    long starttime = std::stoi(times[4]);
    long hz = sysconf(_SC_CLK_TCK);
    long seconds = uptime - (starttime / hz);
    cpuUtilization_ = 100 * (total_time / hz) / seconds;
   
    return cpuUtilization_;
}

// Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid_); }

// Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime() - LinuxParser::UpTime(pid_); }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { 
    return (this->cpuUtilization_ > a.cpuUtilization_);
}