#ifndef DATE_H
#define DATE_H

#include <string>

struct Date {
    int day;
    int month;
    int year;

    bool isBefore(const Date& other) const;
    bool isAfter(const Date& other) const;
    bool isBetween(const Date& start, const Date& end) const;
    std::string toString() const;
};

#endif // DATE_H
