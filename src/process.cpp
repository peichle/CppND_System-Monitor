#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;


Process::Process(int pid){
    pid_ = pid;
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
    long activeJiffies = LinuxParser::ActiveJiffies(pid_);
    long totalJiffies = LinuxParser::Jiffies();
    
    prev_active_ticks = activeJiffies;
    prev_ticks = totalJiffies;

    cpuUtilization_ = (float) activeJiffies / totalJiffies;
    
    return cpuUtilization_;
}

// Return the command that generated this process
string Process::Command() { return com_; }

// Return this process's memory utilization
string Process::Ram() { 
    // std::string ram_ = to_string(std::stoi(LinuxParser::Ram(pid_))/1024)
    std::string ram_ = LinuxParser::Ram(pid_);
    return ram_; 
//    return LinuxParser::Ram(Pid()); 
}

// Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { 
    return (this->cpuUtilization_ > a.cpuUtilization_);
}