#pragma once

#include "Expense.h"

#include <string>
#include <vector>

class ExpenseRepository {
public:
    explicit ExpenseRepository(std::string filePath);

    std::vector<Expense> loadAll() const;
    void saveAll(const std::vector<Expense>& expenses) const;
    void exportToCsv(const std::string& outputPath) const;

private:
    std::string filePath;

    void ensureFileExists() const;
    static std::string escapeCsv(const std::string& value);
    static std::vector<std::string> parseCsvLine(const std::string& line);
};