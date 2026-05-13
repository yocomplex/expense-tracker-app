cmake_minimum_required(VERSION 3.15)
project(expense_tracker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(expense-tracker
    src/main.cpp
    src/ExpenseRepository.cpp
    src/ExpenseService.cpp
    src/CommandParser.cpp
    src/DateUtils.cpp
)