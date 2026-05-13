#pragma once

#include "Expense.h"
#include "ExpenseRepository.h"

#include <optional>
#include <string>
#include <vector>

class ExpenseService {
public:
    explicit ExpenseService(ExpenseRepository repository);

    int addExpense(const std::string& description, double amount, const std::string& category = "General");

    bool updateExpense(
        int id,
        const std::optional<std::string>& description,
        const std::optional<double>& amount,
        const std::optional<std::string>& category
    );

    bool deleteExpense(int id);

    std::vector<Expense> listExpenses(const std::optional<std::string>& category = std::nullopt) const;

    double getTotalSummary() const;
    double getMonthlySummary(int month) const;

    void exportExpenses(const std::string& outputPath) const;

private:
    ExpenseRepository repository;

    int getNextId(const std::vector<Expense>& expenses) const;
};