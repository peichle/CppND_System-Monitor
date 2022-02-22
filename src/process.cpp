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


Process::Process(int pid) : pid_(pid) {
    cpuUtilization_ = CpuUtilization();
}

// Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() {
    long totalTime = LinuxParser::ActiveJiffies(pid_);
    long uptime = LinuxParser::UpTime(pid_);
    cpuUtilization_ = (static_cast<float> (totalTime)/sysconf(_SC_CLK_TCK))/uptime;
    return cpuUtilization_;
}

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(Pid()); }

// Return this process's memory utilization
string Process::Ram() { 
    return to_string(std::stoi(LinuxParser::Ram(pid_))/1024); 
//    return LinuxParser::Ram(Pid()); 
}

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(Pid()); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { 
    return (this->cpuUtilization_ < a.cpuUtilization_);
}