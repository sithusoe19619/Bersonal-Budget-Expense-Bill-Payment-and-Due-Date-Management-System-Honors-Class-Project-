#ifndef BILL_H
#define BILL_H

#include <string>
#include "Date.h"

class Bill {
public:
    std::string name;
    Date dueDate;
    double amountDue;
    bool isPaid;
    Date paidOn;

    Bill() : amountDue(0.0), isPaid(false) {}

    void markAsPaid(const Date& paymentDate);
    void display() const;
};

#endif // BILL_H
