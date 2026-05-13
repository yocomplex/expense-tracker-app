/*
 *******
 *******
    This application builds a simple expense tracker to manage finances.
    The application should allow users to add, delete, and view their expenses.
    The application should also provide a summary of the expenses.

    Users can:
        - Add an expense with a description and amount
        - Update an expense
        - Delete an expense
        - View all expenses
        - View a summary of all expenses
        - View a summary of all expenses for a specific month (of current year)
*******
*******
*/


#include "CommandParser.h"
#include "DateUtils.h"
#include "ExpenseRepository.h"
#include "ExpenseService.h"

#include <iomanip>
#include <iostream>
#include <optional>
#include <string>

void printHelp() {
    std::cout << "Expense Tracker CLI\n\n"
              << "Usage:\n"
              << "  expense-tracker add --description \"Lunch\" --amount 20 [--category Food]\n"
              << "  expense-tracker update --id 1 [--description \"Dinner\"] [--amount 25] [--category Food]\n"
              << "  expense-tracker delete --id 1\n"
              << "  expense-tracker list [--category Food]\n"
              << "  expense-tracker summary [--month 8]\n"
              << "  expense-tracker export --output exports/expenses.csv\n"
              << "  expense-tracker help\n";
}

void printExpenses(const std::vector<Expense>& expenses) {
    if (expenses.empty()) {
        std::cout << "No expenses found.\n";
        return;
    }

    std::cout << std::left
              << std::setw(6) << "ID"
              << std::setw(14) << "Date"
              << std::setw(25) << "Description"
              << std::setw(14) << "Amount"
              << std::setw(15) << "Category"
              << '\n';

    std::cout << std::string(74, '-') << '\n';

    for (const auto& expense : expenses) {
        std::cout << std::left
                  << std::setw(6) << expense.id
                  << std::setw(14) << expense.date
                  << std::setw(25) << expense.description
                  << "$" << std::fixed << std::setprecision(2)
                  << std::setw(13) << expense.amount
                  << std::setw(15) << expense.category
                  << '\n';
    }
}

int main(int argc, char* argv[]) {
    try {
        ExpenseRepository repository("data/expenses.csv");
        ExpenseService service(repository);

        ParsedCommand command = CommandParser::parse(argc, argv);

        if (command.action == "help") {
            printHelp();
            return 0;
        }

        if (command.action == "add") {
            std::string description = CommandParser::getRequiredString(command, "description");
            double amount = CommandParser::getRequiredDouble(command, "amount");
            std::string category = CommandParser::getOption(command, "category").value_or("General");

            int id = service.addExpense(description, amount, category);
            std::cout << "Expense added successfully (ID: " << id << ")\n";
            return 0;
        }

        if (command.action == "update") {
            int id = CommandParser::getRequiredInt(command, "id");

            std::optional<std::string> description = CommandParser::getOption(command, "description");
            std::optional<std::string> category = CommandParser::getOption(command, "category");

            std::optional<double> amount = std::nullopt;
            auto amountOption = CommandParser::getOption(command, "amount");
            if (amountOption.has_value()) {
                try {
                    amount = std::stod(*amountOption);
                } catch (...) {
                    throw std::invalid_argument("Option --amount must be a number.");
                }
            }

            if (!description.has_value() && !amount.has_value() && !category.has_value()) {
                throw std::invalid_argument("Nothing to update. Provide --description, --amount, or --category.");
            }

            bool updated = service.updateExpense(id, description, amount, category);
            if (!updated) {
                std::cout << "Expense not found.\n";
                return 1;
            }

            std::cout << "Expense updated successfully\n";
            return 0;
        }

        if (command.action == "delete") {
            int id = CommandParser::getRequiredInt(command, "id");

            bool deleted = service.deleteExpense(id);
            if (!deleted) {
                std::cout << "Expense not found.\n";
                return 1;
            }

            std::cout << "Expense deleted successfully\n";
            return 0;
        }

        if (command.action == "list") {
            auto category = CommandParser::getOption(command, "category");
            auto expenses = service.listExpenses(category);
            printExpenses(expenses);
            return 0;
        }

        if (command.action == "summary") {
            auto monthOption = CommandParser::getOption(command, "month");

            if (monthOption.has_value()) {
                int month;
                try {
                    month = std::stoi(*monthOption);
                } catch (...) {
                    throw std::invalid_argument("Option --month must be an integer between 1 and 12.");
                }

                double total = service.getMonthlySummary(month);
                std::cout << "Total expenses for " << DateUtils::monthName(month)
                          << ": $" << std::fixed << std::setprecision(2) << total << '\n';
            } else {
                double total = service.getTotalSummary();
                std::cout << "Total expenses: $" << std::fixed << std::setprecision(2) << total << '\n';
            }

            return 0;
        }

        if (command.action == "export") {
            std::string outputPath = CommandParser::getRequiredString(command, "output");
            service.exportExpenses(outputPath);
            std::cout << "Expenses exported successfully to " << outputPath << '\n';
            return 0;
        }

        std::cout << "Unknown command: " << command.action << "\n\n";
        printHelp();
        return 1;

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n\n";
        printHelp();
        return 1;
    }
}