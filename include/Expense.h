#ifndef EXPENSE_H
#define EXPENSE_H

#include <string>
#include "Date.h"

class Expense {
public:
    Date date;
    std::string category;
    double amount;
    std::string description;

    Expense() : amount(0.0) {}

    void display() const;
};

#endif // EXPENSE_H
