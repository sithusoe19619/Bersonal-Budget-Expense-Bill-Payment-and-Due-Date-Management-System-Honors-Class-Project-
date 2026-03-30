#ifndef DATE_H
#define DATE_H

#include <string>

struct Date {
    int day;
    int month;
    int year;

    Date() : day(0), month(0), year(0) {}
    Date(int d, int m, int y) : day(d), month(m), year(y) {}

    bool operator==(const Date& other) const;
    bool isBefore(const Date& other) const;
    bool isAfter(const Date& other) const;
    bool isBetween(const Date& start, const Date& end) const;
    std::string toString() const;
};

#endif // DATE_H
