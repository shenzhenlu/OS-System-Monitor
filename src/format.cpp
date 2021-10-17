#include <string>
#include <sstream>
#include <iomanip>

#include "format.h"

using std::string;
using std::stringstream;

// Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) { 
    int h{0}, m{0}, s{0};
    stringstream sstream;
    h = (seconds / 3600);
    m = (seconds / 60) % 60;
    s = seconds % 60;
    sstream << std::setfill('0') << std::setw(2) << h << ":" 
            << std::setfill('0') << std::setw(2) << m << ":" 
            << std::setfill('0') << std::setw(2) << s;
    
    return sstream.str();
}