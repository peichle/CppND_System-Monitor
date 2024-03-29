#include "processor.h"
#include "linux_parser.h"

#include<iostream>


// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {

// https://stackoverflow.com/questions/3017162/how-to-get-total-cpu-usage-in-linux-using-c/3017438#3017438    
    long active = LinuxParser::ActiveJiffies();
    long idle = LinuxParser::IdleJiffies();

    long duration_active = active - prev_active_ticks;
    long duration_idle = idle - prev_idle_ticks;
    long total_duration = duration_active + duration_idle;

    float util = (float) duration_active / total_duration;

    prev_active_ticks = active;
    prev_idle_ticks = idle;

    return util; 
}