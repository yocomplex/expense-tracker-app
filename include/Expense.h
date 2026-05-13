#pragma once

#include <string>

struct Expense {
    int id{};
    std::string date;        // YYYY-MM-DD
    std::string description;
    double amount{};
    std::string category;    // optional but useful for scaling
};