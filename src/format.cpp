#include <string>

#include "format.h"
#include "system.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long int total_seconds) {
    string formatted_time = "";
    int hrs, mins, secs;
    div_t div_container = div(total_seconds, 60);
    mins = div_container.quot;
    secs = div_container.rem;
    div_container = div(mins, 60);
    hrs = div_container.quot;
    mins = div_container.rem;

    formatted_time = std::to_string(secs);
    if(secs < 10) { formatted_time = "0" + formatted_time; }
    formatted_time = std::to_string(mins) + ":" + formatted_time;
    if(mins < 10) { formatted_time = "0" + formatted_time; }
    formatted_time = std::to_string(hrs) + ":" + formatted_time;
    if(hrs < 10) { formatted_time = "0" + formatted_time; }

    return formatted_time; 
}