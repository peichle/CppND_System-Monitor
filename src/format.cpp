#include <string>
#include <chrono>
#include <iostream>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    string hh = std::to_string(seconds/3600);
    string mm = std::to_string((seconds/60)%60);
    string ss = std::to_string(seconds%60);

    // https://knowledge.udacity.com/questions/155686
    ss.insert(0, 2 - ss.length(), '0');
    mm.insert(0, 2 - mm.length(), '0');
    hh.insert(0, 2 - hh.length(), '0');

    return hh + ":" + mm + ":" + ss; 
}