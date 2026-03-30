#ifndef CATEGORYINFO_H
#define CATEGORYINFO_H

#include <string>

class CategoryInfo {
public:
    std::string name;
    double budgetLimit;
    double totalSpent;

    void addExpense(double amount);
    double getRemainingBudget() const;
    bool isOverBudget() const;
};

#endif // CATEGORYINFO_H
