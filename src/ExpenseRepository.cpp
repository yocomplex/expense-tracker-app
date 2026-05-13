#include "ExpenseRepository.h"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

ExpenseRepository::ExpenseRepository(std::string filePath)
    : filePath(std::move(filePath)) {
    ensureFileExists();
}

void ExpenseRepository::ensureFileExists() const {
    std::filesystem::path path(filePath);

    if (path.has_parent_path()) {
        std::filesystem::create_directories(path.parent_path());
    }

    if (!std::filesystem::exists(path)) {
        std::ofstream file(filePath);
        if (!file) {
            throw std::runtime_error("Could not create storage file: " + filePath);
        }
        file << "id,date,description,amount,category\n";
    }
}

std::vector<Expense> ExpenseRepository::loadAll() const {
    ensureFileExists();

    std::ifstream file(filePath);
    if (!file) {
        throw std::runtime_error("Could not open storage file: " + filePath);
    }

    std::vector<Expense> expenses;
    std::string line;

    // Skip header
    std::getline(file, line);

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        auto fields = parseCsvLine(line);
        if (fields.size() < 5) continue;

        Expense expense;
        expense.id = std::stoi(fields[0]);
        expense.date = fields[1];
        expense.description = fields[2];
        expense.amount = std::stod(fields[3]);
        expense.category = fields[4];

        expenses.push_back(expense);
    }

    return expenses;
}

void ExpenseRepository::saveAll(const std::vector<Expense>& expenses) const {
    ensureFileExists();

    std::ofstream file(filePath, std::ios::trunc);
    if (!file) {
        throw std::runtime_error("Could not write to storage file: " + filePath);
    }

    file << "id,date,description,amount,category\n";

    for (const auto& expense : expenses) {
        file << expense.id << ','
             << escapeCsv(expense.date) << ','
             << escapeCsv(expense.description) << ','
             << std::fixed << std::setprecision(2) << expense.amount << ','
             << escapeCsv(expense.category) << '\n';
    }
}

void ExpenseRepository::exportToCsv(const std::string& outputPath) const {
    auto expenses = loadAll();

    std::filesystem::path path(outputPath);
    if (path.has_parent_path()) {
        std::filesystem::create_directories(path.parent_path());
    }

    std::ofstream file(outputPath, std::ios::trunc);
    if (!file) {
        throw std::runtime_error("Could not export to file: " + outputPath);
    }

    file << "id,date,description,amount,category\n";

    for (const auto& expense : expenses) {
        file << expense.id << ','
             << escapeCsv(expense.date) << ','
             << escapeCsv(expense.description) << ','
             << std::fixed << std::setprecision(2) << expense.amount << ','
             << escapeCsv(expense.category) << '\n';
    }
}

std::string ExpenseRepository::escapeCsv(const std::string& value) {
    bool needsQuotes = value.find(',') != std::string::npos ||
                       value.find('"') != std::string::npos ||
                       value.find('\n') != std::string::npos;

    if (!needsQuotes) {
        return value;
    }

    std::string escaped = "\"";
    for (char c : value) {
        if (c == '"') {
            escaped += "\"\"";
        } else {
            escaped += c;
        }
    }
    escaped += "\"";

    return escaped;
}

std::vector<std::string> ExpenseRepository::parseCsvLine(const std::string& line) {
    std::vector<std::string> result;
    std::string current;
    bool inQuotes = false;

    for (std::size_t i = 0; i < line.size(); ++i) {
        char c = line[i];

        if (c == '"') {
            if (inQuotes && i + 1 < line.size() && line[i + 1] == '"') {
                current += '"';
                ++i;
            } else {
                inQuotes = !inQuotes;
            }
        } else if (c == ',' && !inQuotes) {
            result.push_back(current);
            current.clear();
        } else {
            current += c;
        }
    }

    result.push_back(current);
    return result;
}