#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <iomanip>

#include "utils.h"

std::string curr_date_time(bool compact)
{
        auto now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);
        std::tm timeinfo = *std::localtime(&time);

        // Format the date and time into a string
        std::ostringstream oss;
        if (compact)
                oss << std::put_time(&timeinfo, "%Y%m%d-%H%M%S");
        else
                oss << std::put_time(&timeinfo, "%d-%m-%Y_%H-%M-%S");

        std::string currdatetime = oss.str();

        return currdatetime;
}
