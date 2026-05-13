#pragma once

#include <string>

namespace DateUtils {
    std::string today();
    bool isValidMonth(int month);
    int getMonthFromDate(const std::string& date);
    std::string monthName(int month);
}