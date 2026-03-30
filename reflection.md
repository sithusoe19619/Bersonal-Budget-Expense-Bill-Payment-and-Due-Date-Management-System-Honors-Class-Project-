# Reflection

## System Design

This document describes the three core user actions that the Personal Budget, Expense, Bill Payment and Due Date Management System must support.

---

### Action 1: Log an Expense and Check if it Exceeds a Category Budget

The user selects a spending category (such as Food, Transportation, or Entertainment), enters an amount spent, a date, and an optional description. The system records this expense and immediately compares the running total for that category against the user-defined budget limit. If the total spending in that category now exceeds the budget, the system notifies the user with a warning showing how much over budget they are. This action allows users to stay aware of their spending habits in real time, category by category.

---

### Action 2: View the Next Upcoming Bill and Mark it as Paid

The user can request to see their next upcoming bill. The system scans all unpaid bills, finds the one with the nearest due date, and displays its name, due date, and amount owed. The user then has the option to mark that bill as paid. When marked paid, the system records the date of payment and updates the bill's status so it no longer appears as upcoming. This keeps users informed about what financial obligations are coming up and lets them track which bills have already been handled.

---

### Action 3: Query Spending History by Date Range

The user provides a start date and an end date. The system retrieves all expenses that were logged within that date range and displays them in order. The results show each expense's date, category, amount, and description, along with a subtotal per category and a grand total for the entire period. This action allows users to review their financial history, identify spending patterns, and understand where their money went over any given period of time.

---

## Component Brainstorm

The system is composed of five classes. Three are data-holding structs (Date, Expense, Bill) and two are manager classes that own collections and enforce business logic (BudgetManager, BillManager).

---

### Date

A reusable value type used by both Expense and Bill to represent calendar dates. Kept as its own component so date comparison and formatting logic lives in one place.

**Attributes:**
- `int day` — day of the month (1–31)
- `int month` — month of the year (1–12)
- `int year` — four-digit year

**Methods:**
- `bool isBefore(Date other)` — returns true if this date comes before another; used to find the next upcoming bill and to filter expenses by range
- `bool isAfter(Date other)` — returns true if this date comes after another
- `bool isBetween(Date start, Date end)` — returns true if this date falls within an inclusive range; used in the spending history query
- `string toString()` — formats the date as a readable string (e.g., "03/30/2026") for display

---

### Expense

Represents a single spending event recorded by the user. This is a plain data-holding struct with no business logic of its own.

**Attributes:**
- `Date date` — when the expense occurred
- `string category` — which budget category it belongs to (e.g., "Food", "Transport")
- `double amount` — how much was spent
- `string description` — optional note the user provides for context

**Methods:**
- `void display()` — prints a formatted one-line summary of the expense for use in history queries and receipts

---

### Bill

Represents a financial obligation with a due date. Tracks whether the bill has been paid and when.

**Attributes:**
- `string name` — label for the bill (e.g., "Electric Bill", "Rent")
- `Date dueDate` — when payment is due
- `double amountDue` — the amount owed
- `bool isPaid` — whether the bill has been settled
- `Date paidOn` — the date the user marked it as paid (only meaningful when isPaid is true)

**Methods:**
- `void markAsPaid(Date paymentDate)` — sets isPaid to true and records the paymentDate
- `void display()` — prints a formatted summary of the bill including its status and due date

---

### BudgetManager

The central manager for all expense-related operations. Owns the list of expenses and the per-category budget limits. Enforces the budget check on every new expense.

**Attributes:**
- `vector<Expense> expenses` — the full history of all logged expenses
- `map<string, double> budgetLimits` — maps each category name to its user-defined spending cap
- `map<string, double> categoryTotals` — tracks the running spend total per category, updated on every log

**Methods:**
- `void setBudget(string category, double limit)` — sets or updates the spending cap for a category
- `void logExpense(Expense e)` — adds the expense to the history, updates the running total for its category, and calls checkBudget
- `void checkBudget(string category)` — compares the category's running total against its limit and prints a warning if it is exceeded
- `double getCategoryTotal(string category)` — returns how much has been spent in a given category so far
- `vector<Expense> getExpensesByDateRange(Date start, Date end)` — filters and returns all expenses whose dates fall within the given range, used by Action 3

---

### BillManager

Owns and manages the list of bills. Responsible for finding the next due bill and processing payment.

**Attributes:**
- `vector<Bill> bills` — the complete list of all bills, paid and unpaid

**Methods:**
- `void addBill(Bill b)` — adds a new bill to the list
- `Bill* getNextUnpaidBill()` — scans all unpaid bills, finds the one with the earliest due date, and returns a pointer to it; returns nullptr if all bills are paid
- `void markBillAsPaid(string billName, Date paymentDate)` — finds the bill by name and calls its markAsPaid method
- `void displayAllBills()` — prints a summary of every bill with its status, for an overview screen
