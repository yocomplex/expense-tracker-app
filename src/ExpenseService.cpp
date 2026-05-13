#include "ExpenseService.h"
#include "DateUtils.h"

#include <algorithm>
#include <stdexcept>

ExpenseService::ExpenseService(ExpenseRepository repository)
    : repository(std::move(repository)) {}

int ExpenseService::addExpense(const std::string& description, double amount, const std::string& category) {
    if (description.empty()) {
        throw std::invalid_argument("Description cannot be empty.");
    }

    if (amount <= 0) {
        throw std::invalid_argument("Amount must be greater than zero.");
    }

    auto expenses = repository.loadAll();

    Expense expense;
    expense.id = getNextId(expenses);
    expense.date = DateUtils::today();
    expense.description = description;
    expense.amount = amount;
    expense.category = category.empty() ? "General" : category;

    expenses.push_back(expense);
    repository.saveAll(expenses);

    return expense.id;
}

bool ExpenseService::updateExpense(
    int id,
    const std::optional<std::string>& description,
    const std::optional<double>& amount,
    const std::optional<std::string>& category
) {
    auto expenses = repository.loadAll();

    auto it = std::find_if(expenses.begin(), expenses.end(), [id](const Expense& expense) {
        return expense.id == id;
    });

    if (it == expenses.end()) {
        return false;
    }

    if (description.has_value()) {
        if (description->empty()) {
            throw std::invalid_argument("Description cannot be empty.");
        }
        it->description = *description;
    }

    if (amount.has_value()) {
        if (*amount <= 0) {
            throw std::invalid_argument("Amount must be greater than zero.");
        }
        it->amount = *amount;
    }

    if (category.has_value()) {
        it->category = category->empty() ? "General" : *category;
    }

    repository.saveAll(expenses);
    return true;
}

bool ExpenseService::deleteExpense(int id) {
    auto expenses = repository.loadAll();

    auto originalSize = expenses.size();

    expenses.erase(
        std::remove_if(expenses.begin(), expenses.end(), [id](const Expense& expense) {
            return expense.id == id;
        }),
        expenses.end()
    );

    if (expenses.size() == originalSize) {
        return false;
    }

    repository.saveAll(expenses);
    return true;
}

std::vector<Expense> ExpenseService::listExpenses(const std::optional<std::string>& category) const {
    auto expenses = repository.loadAll();

    if (!category.has_value()) {
        return expenses;
    }

    std::vector<Expense> filtered;

    std::copy_if(expenses.begin(), expenses.end(), std::back_inserter(filtered), [&](const Expense& expense) {
        return expense.category == *category;
    });

    return filtered;
}

double ExpenseService::getTotalSummary() const {
    auto expenses = repository.loadAll();
    double total = 0.0;

    for (const auto& expense : expenses) {
        total += expense.amount;
    }

    return total;
}

double ExpenseService::getMonthlySummary(int month) const {
    if (!DateUtils::isValidMonth(month)) {
        throw std::invalid_argument("Month must be between 1 and 12.");
    }

    auto expenses = repository.loadAll();
    double total = 0.0;

    for (const auto& expense : expenses) {
        if (DateUtils::getMonthFromDate(expense.date) == month) {
            total += expense.amount;
        }
    }

    return total;
}

void ExpenseService::exportExpenses(const std::string& outputPath) const {
    repository.exportToCsv(outputPath);
}

int ExpenseService::getNextId(const std::vector<Expense>& expenses) const {
    int maxId = 0;

    for (const auto& expense : expenses) {
        maxId = std::max(maxId, expense.id);
    }

    return maxId + 1;
}