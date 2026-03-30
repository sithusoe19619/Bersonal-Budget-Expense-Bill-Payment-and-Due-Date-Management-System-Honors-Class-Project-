# Design Phase Report
## Personal Budget, Expense, Bill Payment and Due Date Management System

**Course:** CSC 331H — Honors Project
**Institution:** Borough of Manhattan Community College (BMCC)
**Semester:** Spring 2026
**Submission Date:** March 30, 2026

---

## 1. Project Description

This project is a C++ Personal Budget, Expense, Bill Payment, and Due Date Management System. The system allows a user to log and categorize daily expenses, set spending limits per category, track upcoming bills by due date, and query spending history over any date range. All data structures are implemented from scratch — no STL containers are used for the core logic. The system is composed of seven distinct components organized into three layers: data classes, custom data structures, and a central manager.

The design covers three core user-facing actions:

**Action 1 — Log an Expense and Check Budget**
The user selects a spending category, enters an amount, a date, and an optional description. The system records the expense, updates the running total for that category, and immediately checks whether the total has exceeded the user-defined budget limit. If it has, the system prints a warning showing how much over budget the category is.

**Action 2 — View the Next Upcoming Bill and Mark it as Paid**
The user requests to see their next upcoming bill. The system finds the bill with the earliest due date among all unpaid bills and displays its name, due date, and amount owed. The user can then mark that bill as paid. The system records the payment date and updates the bill's status accordingly.

**Action 3 — Query Spending History by Date Range**
The user provides a start date and an end date. The system retrieves all expenses logged within that range and displays them in chronological order, along with category subtotals and a grand total for the period.

---

## 2. System Components

The system is composed of seven components. Three are data-holding classes (Date, Expense, Bill) and four are logic-carrying components (CategoryInfo, HashMap, MinHeap, BST, BudgetManager).

**Date**
A reusable value type representing a calendar date. Used by both Expense and Bill. It provides comparison methods used for BST ordering, MinHeap ordering, and range filtering.

- `int day` — day of the month (1–31)
- `int month` — month of the year (1–12)
- `int year` — four-digit year
- `bool operator==(const Date&) const` — exact equality check
- `bool isBefore(const Date&) const` — used for BST insertion and heap comparison
- `bool isAfter(const Date&) const` — used for range query boundary checking
- `bool isBetween(const Date& start, const Date& end) const` — used in Action 3 filtering
- `std::string toString() const` — formats the date for display output

**Expense**
Represents a single spending event. A plain data-holding class with no business logic of its own.

- `Date date` — when the expense occurred
- `std::string category` — the budget category it belongs to
- `double amount` — how much was spent
- `std::string description` — optional user note
- `void display() const` — prints a formatted one-line summary for history queries

**Bill**
Represents a financial obligation with a due date. Tracks payment status.

- `std::string name` — label for the bill (e.g., "Rent", "Electric")
- `Date dueDate` — when payment is due
- `double amountDue` — the amount owed
- `bool isPaid` — whether the bill has been settled
- `Date paidOn` — the date the user marked it paid
- `void markAsPaid(const Date& paymentDate)` — sets isPaid to true and records the date
- `void display() const` — prints a formatted summary with status and due date

**CategoryInfo**
Stores the budget state for a single spending category.

- `std::string name` — the category label
- `double budgetLimit` — the user-defined spending cap
- `double totalSpent` — the running total of all expenses logged in this category
- `void addExpense(double amount)` — increments totalSpent
- `double getRemainingBudget() const` — returns budgetLimit minus totalSpent
- `bool isOverBudget() const` — returns true when totalSpent exceeds budgetLimit

**HashMap**
An open-addressing hash map using linear probing. Maps category name strings to CategoryInfo objects. Stores all budget state and provides O(1) average-case lookup.

- Uses an Entry struct with an isDeleted tombstone flag to allow safe removal without breaking probe chains
- Copy constructor and copy assignment operator are deleted (Rule of Three — owns raw pointer)
- WARNING on `get()`: the returned pointer is invalidated if a subsequent `insert()` triggers a resize

**MinHeap**
A binary min-heap ordered by Bill::dueDate. Keeps the earliest-due bill at the root for O(1) retrieval.

- Supports insert, extractMin, peek, isEmpty, and markPaidByName
- `markPaidByName` searches the array linearly and modifies the bill in place — no re-heapify needed since the due date does not change
- Copy constructor and copy assignment operator are deleted (Rule of Three — owns raw pointer)

**BST**
A binary search tree ordered by Expense::date. Supports O(log n + k) range queries for Action 3.

- Equal dates go right, so all expenses on the same date are preserved (duplicate strategy)
- `search()` returns a vector<Expense> to correctly handle multiple expenses on the same date
- Private `insertHelper` and `rangeHelper` support recursive operations
- Copy constructor and copy assignment operator are deleted (Rule of Three — owns raw pointer)

**BudgetManager**
The sole public interface for the entire system. Owns BST, MinHeap, and HashMap as private members. No external code accesses the data structures directly — all operations go through BudgetManager.

- `addExpense(const Expense&)` — delegates to BST and HashMap; triggers budget check
- `addBill(const Bill&)` — delegates to MinHeap
- `setBudgetLimit(const std::string& category, double limit)` — sets or updates category cap
- `checkBudget(const std::string& category)` — reads from HashMap and warns if over limit
- `hasPendingBills() const` — safe guard before any heap access
- `getNextBill()` — returns the earliest-due bill via MinHeap::peek()
- `markBillPaid(const std::string& name, const Date&)` — delegates to MinHeap::markPaidByName()
- `getExpensesByRange(const Date& start, const Date& end)` — delegates to BST::rangeQuery()
- `generateReport()` — reads from BST and HashMap to produce a full spending summary

---

## 3. UML Class Diagram

The UML class diagram is provided as a separate image file: `umlInitial.png`.

The diagram shows 7 components and 9 relationships:
- BudgetManager composes BST, MinHeap, and HashMap (3 composition relationships)
- BST depends on Expense and Date; MinHeap depends on Bill and Date (dependency relationships)
- HashMap stores CategoryInfo as values (structural relationship)
- Date is a shared value type used by Expense and Bill

---

## 4. Data Structure Justification

### HashMap for Budget Tracking

Every time an expense is logged, the system must immediately look up that expense's category, retrieve the running total, add to it, and compare against the budget limit. This happens on every single log operation, so the lookup must be as fast as possible. A HashMap maps category name strings directly to CategoryInfo objects using a hash function, providing O(1) average-case lookup regardless of how many categories exist. Any other structure — a sorted array, a linked list, or a BST — would require O(log n) or O(n) to find the right category on every expense log. Since budget checking is the most frequent operation in the system, O(1) is the correct choice. Open-addressing with linear probing and an isDeleted tombstone was chosen because it is cache-friendly and supports safe deletion without breaking probe chains.

### MinHeap for Bills

The most common bill operation is Action 2: find the next upcoming bill. A MinHeap keeps the bill with the earliest due date at the root at all times, so retrieving it is O(1) with no scanning required. When a bill is updated or extracted, the heap reorganizes itself in O(log n). This is a direct match for the use case: the system always needs the minimum (earliest) due date, which is exactly what a MinHeap is optimized to provide. A plain array or vector would require scanning all unpaid bills every time — O(n) — and the performance would degrade as the bill count grows.

### BST for Expenses

Action 3 asks for all expenses between a start date and an end date. A BST stores expenses sorted by date, so the system can descend the tree to the start date in O(log n) and collect every node in range by continuing the traversal, stopping as soon as it passes the end date. The total cost is O(log n + k), where k is the number of results. A flat array or vector could only achieve this in O(n) by scanning every expense. As spending history accumulates over months or years, the BST keeps range queries efficient while the array approach degrades linearly.

---

## 5. Design Changes

The following changes were made during the header review phase, before implementation began. Each change was driven by a correctness issue discovered when tracing how components interact.

**Date — added operator==, default constructor, parameterized constructor**
The initial design had no equality comparison. BST::search() and MinHeap::markPaidByName() both need to compare two dates for exact equality — without operator==, neither could function correctly. A default constructor was added to prevent garbage int values on freshly created Date objects, and a parameterized constructor Date(int d, int m, int y) was added so dates can be created in a single line throughout the implementation.

**Expense — added default constructor**
The double amount field had no default value and would hold garbage data on a freshly created Expense object. A default constructor Expense() : amount(0.0) was added to ensure clean initialization.

**Bill — changed markAsPaid parameter to const Date&, added constructor**
markAsPaid originally took Date by value, inconsistent with how Date is passed everywhere else. Changed to const Date&. A default constructor Bill() : amountDue(0.0), isPaid(false) was added — without it, isPaid and amountDue would hold garbage values and any bill logic would be undefined behavior.

**CategoryInfo — added default constructor**
budgetLimit and totalSpent were uninitialized doubles. getRemainingBudget() and isOverBudget() would return garbage on any freshly created object. A constructor CategoryInfo() : budgetLimit(0.0), totalSpent(0.0) was added.

**HashMap — added isDeleted tombstone, Entry constructor, Rule of Three**
The Entry struct was missing an isDeleted flag. Open-addressing deletion requires a tombstone marker so that probe chains are not broken when an entry is removed — without it, get() would stop searching too early and miss valid entries beyond the deleted slot. An Entry constructor Entry() : occupied(false), isDeleted(false) was added because calling new Entry[n] without it leaves both booleans uninitialized. The Rule of Three was applied because HashMap owns a raw pointer Entry* buckets — the copy constructor and copy assignment operator were deleted to prevent shallow copies and double-frees.

**MinHeap — added Rule of Three, markPaidByName()**
The Rule of Three was applied since MinHeap owns Bill* heap. More importantly, markPaidByName(const std::string& name, const Date& paymentDate) was added because without it, BudgetManager::markBillPaid() had no way to update a specific bill inside the heap. The only available operations were extractMin(), peek(), and isEmpty(), none of which support finding a bill by name. markPaidByName searches the heap array linearly and updates isPaid and paidOn in place. No re-heapify is needed because the due date does not change.

**BST — added BSTNode constructor, inOrderHelper, explicit include, Rule of Three, duplicate strategy, changed search() return type**
BSTNode had no constructor, leaving its left and right raw pointers uninitialized. A constructor BSTNode(const Expense& e) : data(e), left(nullptr), right(nullptr) was added. A private inOrderHelper was missing despite inOrder() needing to recurse. Date.h was added as an explicit include since Date appears in method signatures and relying on transitive inclusion is fragile. The Rule of Three was applied since BST owns BSTNode* root. A comment was added documenting the duplicate date strategy — equal dates go right — so all expenses on the same day are preserved. Finally, search() was changed from Expense* to std::vector<Expense> to correctly handle multiple expenses on the same date, and made const.

**BudgetManager — added explicit includes, hasPendingBills()**
Expense, Bill, and Date were used in method signatures but only available through transitive includes. Explicit include directives were added for all three. hasPendingBills() const was added as a guard for getNextBill() — without it, calling getNextBill() on an empty heap would crash with no way for the caller to check first.

---

## 6. Algorithm Definition

### Action 1 — Log an Expense and Check Budget

**BudgetManager::addExpense(Expense e)**
```
insert e into expenseTree (BST)

if categoryMap does not contain e.category
    create new CategoryInfo with name = e.category
    insert it into categoryMap with key = e.category

get pointer to CategoryInfo for e.category from categoryMap
call categoryInfo.addExpense(e.amount)
call checkBudget(e.category)
```

**CategoryInfo::addExpense(double amount)**
```
totalSpent = totalSpent + amount
```

**BudgetManager::checkBudget(string category)**
```
get pointer to CategoryInfo for category from categoryMap
if categoryInfo.isOverBudget()
    print warning: category has exceeded its budget limit
    print how much over: totalSpent - budgetLimit
```

---

### Action 2 — View Next Bill and Mark as Paid

**BudgetManager::hasPendingBills()**
```
return not billHeap.isEmpty()
```

**BudgetManager::getNextBill()**
```
return billHeap.peek()
```

**MinHeap::peek()**
```
return heap[0]    // root is always the minimum (earliest due date)
```

**BudgetManager::markBillPaid(string name, Date paymentDate)**
```
call billHeap.markPaidByName(name, paymentDate)
```

**MinHeap::markPaidByName(string name, Date paymentDate)**
```
for i from 0 to size - 1
    if heap[i].name == name and heap[i].isPaid == false
        heap[i].isPaid = true
        heap[i].paidOn = paymentDate
        return true
return false    // bill not found
```

---

### Action 3 — Query Spending History by Date Range

**BudgetManager::getExpensesByRange(Date start, Date end)**
```
return expenseTree.rangeQuery(start, end)
```

**BST::rangeQuery(Date start, Date end)**
```
create empty result vector
call rangeHelper(root, start, end, result)
return result
```

**BST::rangeHelper(BSTNode* node, Date start, Date end, vector& result)**
```
if node is null
    return

if start is before node.data.date
    recurse left: rangeHelper(node.left, start, end, result)

if node.data.date is between start and end (inclusive)
    add node.data to result

if end is after node.data.date
    recurse right: rangeHelper(node.right, start, end, result)
```

---

### Key Data Structure Operations

**BST::insert(Expense e) — via insertHelper**
```
if node is null
    return new BSTNode(e)
if e.date is before node.data.date
    node.left = insertHelper(node.left, e)
else                                    // equal dates go right (duplicate strategy)
    node.right = insertHelper(node.right, e)
return node
```

**MinHeap::insert(Bill bill)**
```
if size == capacity
    resize heap array (double capacity)
heap[size] = bill
size = size + 1
heapifyUp(size - 1)
```

**MinHeap::heapifyUp(int index)**
```
while index > 0
    parent = (index - 1) / 2
    if heap[index].dueDate is before heap[parent].dueDate
        swap heap[index] and heap[parent]
        index = parent
    else
        break
```

**MinHeap::extractMin()**
```
min = heap[0]
heap[0] = heap[size - 1]
size = size - 1
heapifyDown(0)
return min
```

**MinHeap::heapifyDown(int index)**
```
while true
    left  = 2 * index + 1
    right = 2 * index + 2
    smallest = index

    if left < size and heap[left].dueDate is before heap[smallest].dueDate
        smallest = left
    if right < size and heap[right].dueDate is before heap[smallest].dueDate
        smallest = right
    if smallest == index
        break
    swap heap[index] and heap[smallest]
    index = smallest
```

**HashMap::insert(string key, CategoryInfo value)**
```
if size / capacity >= 0.7
    resize()
index = hash(key) % capacity
while buckets[index].occupied and not buckets[index].isDeleted
    if buckets[index].key == key
        buckets[index].value = value    // update existing
        return
    index = (index + 1) % capacity     // linear probe
buckets[index].key = key
buckets[index].value = value
buckets[index].occupied = true
buckets[index].isDeleted = false
size = size + 1
```

**HashMap::get(string key)**
```
index = hash(key) % capacity
while buckets[index].occupied or buckets[index].isDeleted
    if buckets[index].occupied and not buckets[index].isDeleted
        if buckets[index].key == key
            return pointer to buckets[index].value
    index = (index + 1) % capacity
return nullptr
```

---

## 7. Interface Design

BudgetManager is the only class the outside world interacts with directly. It owns BST, MinHeap, and HashMap as private members and exposes a public interface that hides all data structure complexity. No external code ever calls BST::insert(), MinHeap::peek(), or HashMap::get() directly — every operation enters the system through BudgetManager.

**BudgetManager delegates to BST** for all expense operations. addExpense() calls BST::insert() to store the expense. getExpensesByRange() calls BST::rangeQuery(). generateReport() calls BST::inOrder() to walk all expenses in date order. BudgetManager never accesses BSTNode directly — all tree traversal is encapsulated inside BST.

**BudgetManager delegates to MinHeap** for all bill operations. addBill() calls MinHeap::insert(). hasPendingBills() calls MinHeap::isEmpty(). getNextBill() calls MinHeap::peek(). markBillPaid() calls MinHeap::markPaidByName(). BudgetManager never calls heapifyUp() or heapifyDown() — those are private implementation details of MinHeap.

**BudgetManager delegates to HashMap** for all budget state. setBudgetLimit() calls HashMap::insert() to create or update a category entry. addExpense() calls HashMap::contains() to check whether a category already exists, then HashMap::get() to retrieve a live CategoryInfo pointer. BudgetManager never accesses Entry or calls hash() or resize() — those are private to HashMap.

**HashMap communicates with CategoryInfo** through its public interface. Once BudgetManager retrieves a CategoryInfo pointer via HashMap::get(), it calls CategoryInfo::addExpense() to increment totalSpent, CategoryInfo::isOverBudget() to check the limit, and CategoryInfo::getRemainingBudget() to compute the excess. BudgetManager never modifies totalSpent or budgetLimit directly.

**BST and MinHeap communicate with their data types** (Expense and Bill respectively) one-directionally. BST reads Expense::date to determine ordering and calls Expense::display() when printing. MinHeap reads Bill::dueDate for heap ordering and calls Bill::markAsPaid() and Bill::display() when needed. Neither Expense nor Bill knows anything about the structure that holds it.

**Date is a shared value type** passed by value or const reference across all interfaces. Its comparison methods — isBefore(), isAfter(), isBetween(), and operator==() — are used uniformly by BST for ordering and range filtering, by MinHeap for heap comparison, and by BudgetManager for boundary validation. Its toString() method is used by display() methods in Expense and Bill.

---

## 8. Data Flow Diagrams

### Action 1 — Log an Expense and Check Budget

```
User Input
  └─► BudgetManager::addExpense(Expense e)
        ├─► BST::insert(e)
        │     └─► BSTNode created, placed by date order
        ├─► HashMap::get(e.category)
        │     ├─► [category exists] returns CategoryInfo*
        │     └─► [category new]   HashMap::insert → new CategoryInfo created
        ├─► CategoryInfo::addExpense(e.amount)
        │     └─► totalSpent updated
        └─► BudgetManager::checkBudget(e.category)
              └─► CategoryInfo::isOverBudget()
                    ├─► [false] no output
                    └─► [true]  warning printed to user
                                  └─► CategoryInfo::getRemainingBudget()
                                        └─► amount over budget displayed
```

### Action 2 — View Next Bill and Mark as Paid

```
User Input
  └─► BudgetManager::hasPendingBills()
        └─► MinHeap::isEmpty()
              ├─► [true]  "No pending bills" displayed to user
              └─► [false] BudgetManager::getNextBill()
                            └─► MinHeap::peek()
                                  └─► heap[0] returned (earliest due date)
                                        └─► Bill::display() → shown to user

User confirms payment
  └─► BudgetManager::markBillPaid(name, paymentDate)
        └─► MinHeap::markPaidByName(name, paymentDate)
              └─► heap scanned linearly for matching name
                    ├─► [not found] returns false → user notified
                    └─► [found]     Bill::markAsPaid(paymentDate)
                                      └─► isPaid = true, paidOn = paymentDate
```

### Action 3 — Query Spending History by Date Range

```
User Input (start date, end date)
  └─► BudgetManager::getExpensesByRange(start, end)
        └─► BST::rangeQuery(start, end)
              └─► BST::rangeHelper(root, start, end, result)
                    └─► tree traversal
                          ├─► [node.date before start]  go right only
                          ├─► [node.date after end]     go left only
                          ├─► [node.date in range]      add to result, go both
                          └─► [node is null]            return
              └─► vector<Expense> returned to BudgetManager
                    └─► returned to caller
                          └─► Expense::display() called for each result
                                └─► date, category, amount, description printed
```

---

## 9. Traceability Matrix

This section traces each user-facing requirement through every layer of the design, from the core action down to the specific method signatures declared in `include/`.

**Action 1 — Log an Expense and Check Budget**

- Components: BudgetManager, BST, HashMap, CategoryInfo, Expense
- Header files: BudgetManager.h, BST.h, HashMap.h, CategoryInfo.h, Expense.h
- Methods: BudgetManager::addExpense(), BudgetManager::checkBudget(), BudgetManager::setBudgetLimit(), BST::insert(), HashMap::get(), HashMap::insert(), CategoryInfo::addExpense(), CategoryInfo::isOverBudget(), CategoryInfo::getRemainingBudget()

**Action 2 — View the Next Upcoming Bill and Mark it as Paid**

- Components: BudgetManager, MinHeap, Bill
- Header files: BudgetManager.h, MinHeap.h, Bill.h
- Methods: BudgetManager::hasPendingBills(), BudgetManager::getNextBill(), BudgetManager::markBillPaid(), MinHeap::isEmpty(), MinHeap::peek(), MinHeap::markPaidByName(), Bill::markAsPaid(), Bill::display()

**Action 3 — Query Spending History by Date Range**

- Components: BudgetManager, BST, Expense, Date
- Header files: BudgetManager.h, BST.h, Expense.h, Date.h
- Methods: BudgetManager::getExpensesByRange(), BST::rangeQuery(), BST::rangeHelper(), Date::isBefore(), Date::isAfter(), Date::isBetween(), Expense::display()

**Supporting concerns traced to specific methods:**

- Store and retrieve category budget limits: HashMap::insert(), HashMap::get(), HashMap::contains() in HashMap.h and CategoryInfo.h
- Prevent duplicate-date data loss: BST::insertHelper() in BST.h — equal dates go right, all expenses on the same day are preserved
- Prevent dangling pointer on bill lookup: MinHeap::markPaidByName() in MinHeap.h — modifies bill in place, no re-heapify needed
- Prevent empty-heap crash: BudgetManager::hasPendingBills() and MinHeap::isEmpty() in BudgetManager.h and MinHeap.h
- Safe memory management: Destructors and deleted copy constructor and copy assignment operator on HashMap, MinHeap, and BST
- Date comparison and formatting: Date::operator==(), Date::isBefore(), Date::isAfter(), Date::isBetween(), Date::toString() in Date.h

---

## 10. Unit Test Planning

Thirty tests are planned across all six modules. Each test specifies its input, expected output, and the requirement it validates.

### Date — 5 Tests

**Test 1 — Default constructor produces zeroed date**
Input: Date d. Expected: d.day == 0, d.month == 0, d.year == 0. Covers: uninitialized field fix.

**Test 2 — Parameterized constructor sets correct values**
Input: Date d(15, 6, 2026). Expected: d.day == 15, d.month == 6, d.year == 2026. Covers: Date(int, int, int).

**Test 3 — isBefore returns correct result**
Input: Date(1, 1, 2026).isBefore(Date(2, 1, 2026)). Expected: true. Covers: BST ordering, MinHeap ordering.

**Test 4 — operator== returns true for equal dates**
Input: Date(5, 3, 2026) == Date(5, 3, 2026). Expected: true. Covers: BST search, markPaidByName.

**Test 5 — isBetween returns true for date within range**
Input: Date(10, 3, 2026).isBetween(Date(1, 3, 2026), Date(31, 3, 2026)). Expected: true. Covers: Action 3 range filtering.

### CategoryInfo — 5 Tests

**Test 6 — Default constructor initializes to zero**
Input: CategoryInfo c. Expected: c.budgetLimit == 0.0, c.totalSpent == 0.0. Covers: uninitialized field fix.

**Test 7 — addExpense accumulates correctly**
Input: addExpense(50.0) then addExpense(30.0). Expected: totalSpent == 80.0. Covers: Action 1 running total.

**Test 8 — isOverBudget returns false when under limit**
Input: budgetLimit = 100.0, totalSpent = 80.0. Expected: isOverBudget() == false. Covers: Action 1 budget check.

**Test 9 — isOverBudget returns true when over limit**
Input: budgetLimit = 100.0, totalSpent = 120.0. Expected: isOverBudget() == true. Covers: Action 1 budget warning.

**Test 10 — getRemainingBudget returns correct difference**
Input: budgetLimit = 200.0, totalSpent = 75.0. Expected: getRemainingBudget() == 125.0. Covers: Action 1 output.

### HashMap — 5 Tests

**Test 11 — insert and get round-trip**
Input: insert ("Food", c), then get("Food"). Expected: returns pointer to the inserted CategoryInfo. Covers: Action 1 category lookup.

**Test 12 — contains returns false for missing key**
Input: empty map, contains("Rent"). Expected: false. Covers: addExpense new-category branch.

**Test 13 — remove makes key unreachable**
Input: insert ("Food", c), remove "Food", then get("Food"). Expected: nullptr. Covers: tombstone isDeleted correctness.

**Test 14 — get still finds entries after a removal (probe chain intact)**
Input: insert A, insert B at same hash slot (probed), remove A, get(B). Expected: returns B correctly. Covers: isDeleted tombstone not breaking probe chain.

**Test 15 — resize triggers and preserves all entries**
Input: insert 12 entries into a capacity-16 map (load > 0.7). Expected: all 12 entries still retrievable after resize. Covers: resize correctness.

### MinHeap — 5 Tests

**Test 16 — isEmpty returns true on new heap**
Input: MinHeap h. Expected: h.isEmpty() == true. Covers: hasPendingBills() guard.

**Test 17 — peek returns bill with earliest due date**
Input: insert bills with due dates 15/4, 3/4, 20/4. Expected: peek().dueDate == Date(3, 4, 2026). Covers: Action 2 next-bill retrieval.

**Test 18 — extractMin removes root and restores heap order**
Input: insert bills with due dates 15/4, 3/4, 20/4, call extractMin(). Expected: extracted bill has dueDate == Date(3, 4, 2026), new root is 15/4. Covers: heap order maintenance.

**Test 19 — markPaidByName updates correct bill**
Input: insert bills "Electric" and "Rent", call markPaidByName("Rent", date). Expected: "Rent" has isPaid == true, "Electric" unchanged. Covers: Action 2 mark-as-paid.

**Test 20 — markPaidByName returns false for unknown name**
Input: markPaidByName("Internet", date) on heap with no such bill. Expected: returns false. Covers: graceful failure handling.

### BST — 5 Tests

**Test 21 — inOrder returns expenses sorted by date**
Input: insert expenses on dates 10/3, 5/3, 20/3. Expected: inOrder() returns them in order 5/3, 10/3, 20/3. Covers: BST ordering correctness.

**Test 22 — Duplicate dates both preserved**
Input: insert two expenses on 10/3/2026. Expected: inOrder() contains both. Covers: duplicate date strategy (equal dates go right).

**Test 23 — search returns all expenses on a given date**
Input: insert three expenses, two on 10/3 and one on 15/3, search(Date(10,3,2026)). Expected: returns vector of size 2. Covers: search() returning vector instead of single pointer.

**Test 24 — rangeQuery returns only expenses within range**
Input: insert expenses on 1/3, 10/3, 20/3, 1/4; query range 5/3 to 25/3. Expected: returns expenses on 10/3 and 20/3 only. Covers: Action 3.

**Test 25 — rangeQuery returns empty vector for no matches**
Input: all expenses before start date. Expected: empty vector. Covers: Action 3 edge case.

### BudgetManager — 5 Integration Tests

**Test 26 — addExpense triggers budget warning when over limit**
Input: setBudgetLimit("Food", 100.0), add expenses totaling 110.0 in "Food". Expected: warning printed after total exceeds 100.0. Covers: Action 1 end-to-end.

**Test 27 — addExpense auto-creates category with no prior setBudgetLimit**
Input: addExpense on category "Transport" with no prior setBudgetLimit. Expected: no crash, category is created automatically. Covers: new-category branch in addExpense.

**Test 28 — hasPendingBills returns false on empty system**
Input: fresh BudgetManager, no bills added. Expected: hasPendingBills() == false. Covers: Action 2 empty guard.

**Test 29 — getNextBill returns earliest unpaid bill**
Input: add bills due 20/4, 5/4, 15/4. Expected: getNextBill().dueDate == Date(5, 4, 2026). Covers: Action 2 end-to-end.

**Test 30 — getExpensesByRange returns correct subset**
Input: add expenses on 1/3, 10/3, 25/3, 5/4; query 5/3 to 20/3. Expected: only 10/3 expense returned. Covers: Action 3 end-to-end.
