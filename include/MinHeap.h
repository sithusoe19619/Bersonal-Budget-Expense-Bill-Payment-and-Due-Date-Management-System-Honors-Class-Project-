#ifndef MINHEAP_H
#define MINHEAP_H

#include <string>
#include "Bill.h"

class MinHeap {
private:
    Bill* heap;
    int size;
    int capacity;

    void heapifyUp(int index);
    void heapifyDown(int index);

public:
    MinHeap(int initialCapacity = 16);
    ~MinHeap();
    MinHeap(const MinHeap&) = delete;
    MinHeap& operator=(const MinHeap&) = delete;

    void insert(const Bill& bill);
    Bill extractMin();
    Bill peek() const;
    bool isEmpty() const;
    bool markPaidByName(const std::string& name, const Date& paymentDate);
};

#endif // MINHEAP_H
