#ifndef BST_H
#define BST_H

#include <vector>
#include "Expense.h"
#include "Date.h"

class BST {
private:
    struct BSTNode {
        Expense data;
        BSTNode* left;
        BSTNode* right;
        BSTNode(const Expense& e) : data(e), left(nullptr), right(nullptr) {}
    };

    BSTNode* root;

    // Duplicate date strategy: equal dates go right, so all expenses are preserved
    BSTNode* insertHelper(BSTNode* node, const Expense& e);
    void inOrderHelper(BSTNode* node, std::vector<Expense>& result) const;
    void rangeHelper(BSTNode* node, const Date& start, const Date& end, std::vector<Expense>& result) const;
    void destroyTree(BSTNode* node);

public:
    BST();
    ~BST();
    BST(const BST&) = delete;
    BST& operator=(const BST&) = delete;

    void insert(const Expense& e);
    std::vector<Expense> search(const Date& d) const; // returns all expenses on a given date
    std::vector<Expense> rangeQuery(const Date& start, const Date& end) const;
    std::vector<Expense> inOrder() const;
};

#endif // BST_H
