# Finance Manager

A Personal Budget, Expense, Bill Payment and Due Date Management System written in C++.

## Project Structure

```
finance-manager/
├── include/        # Header files (.h)
├── src/            # Implementation files (.cpp)
├── Makefile        # Build instructions
├── README.md       # Project overview
└── reflection.md   # Design reflection and documentation
```

## Components

| Component      | Description                                      |
|----------------|--------------------------------------------------|
| `Expense`      | Represents a single spending event               |
| `Bill`         | Represents a financial obligation with due date  |
| `CategoryInfo` | Stores budget limit and running total per category |
| `HashMap`      | Custom hash map: category name → CategoryInfo    |
| `MinHeap`      | Priority queue of bills ordered by due date      |
| `BST`          | Binary search tree of expenses ordered by date   |
| `BudgetManager`| Central manager composing all data structures    |

## Build

```bash
make
```

## Run

```bash
./finance-manager
```
