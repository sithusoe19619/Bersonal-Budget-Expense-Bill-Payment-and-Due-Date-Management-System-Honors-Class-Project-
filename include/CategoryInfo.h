#ifndef CATEGORYINFO_H
#define CATEGORYINFO_H

#include <string>

class CategoryInfo {
public:
    std::string name;
    double budgetLimit;
    double totalSpent;

    CategoryInfo() : budgetLimit(0.0), totalSpent(0.0) {}

    void addExpense(double amount);
    double getRemainingBudget() const;
    bool isOverBudget() const;
};

#endif // CATEGORYINFO_H
