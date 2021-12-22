#include <string>
#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
std::string Format::ElapsedTime(long seconds) { 
    long hr, min, sec;
    hr = seconds/3600;
    min = (seconds - 3600*hr)/60;
    sec = (seconds - 3600*hr)%60;

    std::string HH, MM, SS;
    if (hr > 99) {
        HH = "-99";
    } else if (hr >= 10 && hr <= 99 ){
        HH = std::to_string(hr);
    } else {
        HH = "0" + std::to_string(hr);
    }

    if (min >= 10 && min < 60) {
        MM = std::to_string(min);
    } else if (min < 10) {
        MM = "0" + std::to_string(min);
    } else {
        MM = "-99";
    }

    if (sec >= 10 && sec < 60) {
        SS = std::to_string(sec);
    } else if (sec < 10) {
        SS = "0" + std::to_string(sec);
    } else {
        SS = "-99";
    }
    return HH + ":" + MM + ":" + SS; 
}