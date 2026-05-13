#include <emscripten/bind.h>

#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace emscripten;

struct Expense {
    int id;
    std::string date;
    std::string description;
    double amount;
    std::string category;
};

class ExpenseTracker {
private:
    std::vector<Expense> expenses;
    int nextId = 1;

    std::string today() const {
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

public:
    int addExpense(const std::string& description, double amount, const std::string& category) {
        if (description.empty()) {
            throw std::invalid_argument("Description cannot be empty.");
        }

        if (amount <= 0) {
            throw std::invalid_argument("Amount must be greater than zero.");
        }

        Expense expense {
            nextId++,
            today(),
            description,
            amount,
            category.empty() ? "General" : category
        };

        expenses.push_back(expense);
        return expense.id;
    }

    bool updateExpense(int id, const std::string& description, double amount, const std::string& category) {
        auto it = std::find_if(expenses.begin(), expenses.end(), [id](const Expense& expense) {
            return expense.id == id;
        });

        if (it == expenses.end()) {
            return false;
        }

        if (!description.empty()) {
            it->description = description;
        }

        if (amount > 0) {
            it->amount = amount;
        }

        if (!category.empty()) {
            it->category = category;
        }

        return true;
    }

    bool deleteExpense(int id) {
        auto originalSize = expenses.size();

        expenses.erase(
            std::remove_if(expenses.begin(), expenses.end(), [id](const Expense& expense) {
                return expense.id == id;
            }),
            expenses.end()
        );

        return expenses.size() != originalSize;
    }

    std::vector<Expense> listExpenses() const {
        return expenses;
    }

    double getTotalSummary() const {
        double total = 0.0;

        for (const auto& expense : expenses) {
            total += expense.amount;
        }

        return total;
    }

    double getMonthlySummary(int month) const {
        if (month < 1 || month > 12) {
            throw std::invalid_argument("Month must be between 1 and 12.");
        }

        double total = 0.0;

        for (const auto& expense : expenses) {
            if (expense.date.size() >= 7) {
                int expenseMonth = std::stoi(expense.date.substr(5, 2));
                if (expenseMonth == month) {
                    total += expense.amount;
                }
            }
        }

        return total;
    }

    std::string exportCsv() const {
        std::ostringstream csv;
        csv << "id,date,description,amount,category\n";

        for (const auto& expense : expenses) {
            csv << expense.id << ","
                << expense.date << ","
                << expense.description << ","
                << std::fixed << std::setprecision(2) << expense.amount << ","
                << expense.category << "\n";
        }

        return csv.str();
    }
};

EMSCRIPTEN_BINDINGS(expense_tracker_module) {
    value_object<Expense>("Expense")
        .field("id", &Expense::id)
        .field("date", &Expense::date)
        .field("description", &Expense::description)
        .field("amount", &Expense::amount)
        .field("category", &Expense::category);

    register_vector<Expense>("ExpenseVector");

    class_<ExpenseTracker>("ExpenseTracker")
        .constructor<>()
        .function("addExpense", &ExpenseTracker::addExpense)
        .function("updateExpense", &ExpenseTracker::updateExpense)
        .function("deleteExpense", &ExpenseTracker::deleteExpense)
        .function("listExpenses", &ExpenseTracker::listExpenses)
        .function("getTotalSummary", &ExpenseTracker::getTotalSummary)
        .function("getMonthlySummary", &ExpenseTracker::getMonthlySummary)
        .function("exportCsv", &ExpenseTracker::exportCsv);
}