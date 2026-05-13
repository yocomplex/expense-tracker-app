#include "DateUtils.h"

#include <array>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace DateUtils {

std::string today() {
    std::time_t now = std::time(nullptr);
    std::tm localTime{};

#if defined(_WIN32)
    localtime_s(&localTime, &now);
#else
    localtime_r(&now, &localTime);
#endif

    std::ostringstream oss;
    oss << std::put_time(&localTime, "%Y-%m-%d");
    return oss.str();
}

bool isValidMonth(int month) {
    return month >= 1 && month <= 12;
}

int getMonthFromDate(const std::string& date) {
    // Expected format: YYYY-MM-DD
    if (date.size() < 7) {
        throw std::invalid_argument("Invalid date format: " + date);
    }

    return std::stoi(date.substr(5, 2));
}

std::string monthName(int month) {
    static const std::array<std::string, 12> months = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };

    if (!isValidMonth(month)) {
        return "Invalid month";
    }

    return months[month - 1];
}

}