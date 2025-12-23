# Building Guide: Modern Qt Desktop App Architecture

A concise guide to building modular Qt desktop applications with a clean, macOS-inspired design.

## 📋 Table of Contents

1. [Architecture Overview](#architecture-overview)
2. [Project Structure](#project-structure)
3. [Core Components](#core-components)
4. [Module Pattern](#module-pattern)
5. [Step-by-Step: Creating a New Module](#step-by-step-creating-a-new-module)
6. [UI/UX Patterns](#uiux-patterns)
7. [Database Setup](#database-setup)
8. [Build Configuration](#build-configuration)

---

## Architecture Overview

```
┌─────────────────────────────────────────┐
│         MainWindow (Shell)              │
│  ┌──────────┐  ┌────────────────────┐  │
│  │ Sidebar  │  │  StackedWidget     │  │
│  │ Menu     │  │  (Content Area)    │  │
│  │          │  │                    │  │
│  │ - Item 1 │  │  [Module Widgets]  │  │
│  │ - Item 2 │  │                    │  │
│  │ - Item N │  │                    │  │
│  └──────────┘  └────────────────────┘  │
└─────────────────────────────────────────┘
              ↓
┌─────────────────────────────────────────┐
│         Module Layer                    │
│  ┌──────────────┐  ┌─────────────────┐ │
│  │  UI Widget   │←→│  Repository     │ │
│  │  (View)      │  │  (Data Access)  │ │
│  └──────────────┘  └─────────────────┘ │
└─────────────────────────────────────────┘
              ↓
┌─────────────────────────────────────────┐
│      DatabaseManager (Singleton)        │
│           SQLite / MySQL                │
└─────────────────────────────────────────┘
```

**Key Principles:**
- **Separation of Concerns**: UI, Business Logic, Data Access are separate
- **Repository Pattern**: All database operations go through repositories
- **Singleton Database**: Single connection manager
- **Modular Design**: Each feature is a self-contained module

---

## Project Structure

```
your-app/
├── CMakeLists.txt
├── main.cpp
├── mainwindow.h/cpp          # Main application shell
├── database/
│   ├── databasemanager.h/cpp # Singleton DB connection
│   └── schema.sql            # Database schema (optional)
├── modules/
│   └── [module-name]/
│       ├── [entity].h        # Data model struct
│       └── [entity]repository.h/cpp  # CRUD operations
├── ui/
│   ├── login/
│   │   └── logindialog.h/cpp
│   ├── [module-name]/
│   │   └── [module]system.h/cpp  # Main module widget
│   └── basesystemwidget.h/cpp    # Base class for modules
└── utils/
    └── thememanager.h/cpp        # Theme/styling system
```

---

## Core Components

### 1. DatabaseManager (Singleton)

**Purpose**: Centralized database connection management

```cpp
// database/databasemanager.h
class DatabaseManager {
public:
    static DatabaseManager& instance();
    bool connect();
    QSqlDatabase getDatabase() const;
    void initSchema();  // Creates tables if missing
private:
    DatabaseManager();  // Private constructor
    QSqlDatabase m_db;
};
```

**Features:**
- Falls back to SQLite if MySQL unavailable
- Auto-creates schema on first run
- Singleton pattern ensures one connection

### 2. MainWindow (Application Shell)

**Purpose**: Container with sidebar navigation and content area

**Key Elements:**
- `QListWidget` for sidebar menu
- `QStackedWidget` for content pages
- Role-based menu visibility
- Theme toggle button

### 3. Repository Pattern

**Purpose**: Encapsulate all database operations

```cpp
// modules/student/studentrepository.h
class StudentRepository {
public:
    bool addStudent(const Student& student);
    bool updateStudent(const Student& student);
    bool deleteStudent(int id);
    std::optional<Student> getStudentById(int id);
    std::vector<Student> getAllStudents();
private:
    QSqlDatabase m_db = DatabaseManager::instance().getDatabase();
};
```

**Benefits:**
- Centralized data access logic
- Easy to test and mock
- Clean separation from UI

### 4. Module Widget

**Purpose**: UI component for each feature module

**Structure:**
- Inherits from `QWidget` or `BaseSystemWidget`
- Contains `QTableView` for data display
- Contains buttons for Add/Edit/Delete
- Uses repository for data operations

---

## Module Pattern

Every module follows this structure:

```
modules/[name]/
  ├── [entity].h              # Data model
  └── [entity]repository.h    # Data access

ui/[name]/
  ├── [name]system.h/cpp      # Main widget
  └── [name]dialog.h/cpp      # Add/Edit dialog (optional)
```

**Data Flow:**
```
User Action (Button Click)
    ↓
Module Widget Slot
    ↓
Repository Method
    ↓
DatabaseManager
    ↓
SQL Query Execution
    ↓
Result Returned to Widget
    ↓
UI Update (Refresh Table)
```

---

## Step-by-Step: Creating a New Module

### Step 1: Define Data Model

```cpp
// modules/task/task.h
#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QDate>

struct Task {
    int id = -1;
    QString title;
    QString description;
    QDate dueDate;
    QString status;  // "Pending", "In Progress", "Completed"
    int userId;
};

#endif // TASK_H
```

### Step 2: Create Repository

```cpp
// modules/task/taskrepository.h
#ifndef TASKREPOSITORY_H
#define TASKREPOSITORY_H

#include "task.h"
#include "database/databasemanager.h"
#include <vector>
#include <optional>

class TaskRepository {
public:
    TaskRepository();
    
    bool addTask(const Task& task);
    bool updateTask(const Task& task);
    bool deleteTask(int id);
    std::optional<Task> getTaskById(int id);
    std::vector<Task> getAllTasks();
    std::vector<Task> getTasksByUserId(int userId);
};

#endif // TASKREPOSITORY_H
```

```cpp
// modules/task/taskrepository.cpp
#include "taskrepository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

TaskRepository::TaskRepository() {}

bool TaskRepository::addTask(const Task& task) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("INSERT INTO tasks (title, description, due_date, status, user_id) "
                  "VALUES (:title, :description, :dueDate, :status, :userId)");
    query.bindValue(":title", task.title);
    query.bindValue(":description", task.description);
    query.bindValue(":dueDate", task.dueDate);
    query.bindValue(":status", task.status);
    query.bindValue(":userId", task.userId);
    
    if (!query.exec()) {
        qDebug() << "Error adding task:" << query.lastError().text();
        return false;
    }
    return true;
}

std::vector<Task> TaskRepository::getAllTasks() {
    std::vector<Task> tasks;
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.exec("SELECT task_id, title, description, due_date, status, user_id FROM tasks");
    
    while (query.next()) {
        Task task;
        task.id = query.value(0).toInt();
        task.title = query.value(1).toString();
        task.description = query.value(2).toString();
        task.dueDate = query.value(3).toDate();
        task.status = query.value(4).toString();
        task.userId = query.value(5).toInt();
        tasks.push_back(task);
    }
    return tasks;
}

// Implement other methods similarly...
```

### Step 3: Add Database Table

Add to `DatabaseManager::initSchema()`:

```cpp
// In databasemanager.cpp
query.exec(QString("CREATE TABLE IF NOT EXISTS tasks ("
           "task_id %1, "
           "title VARCHAR(200) NOT NULL, "
           "description TEXT, "
           "due_date DATE, "
           "status VARCHAR(20) DEFAULT 'Pending', "
           "user_id INT, "
           "FOREIGN KEY (user_id) REFERENCES users(user_id))").arg(autoInc));
```

### Step 4: Create UI Widget

```cpp
// ui/task/tasksystem.h
#ifndef TASKSYSTEM_H
#define TASKSYSTEM_H

#include <QWidget>
#include <QTableView>
#include <QStandardItemModel>
#include <QPushButton>
#include "../../modules/task/taskrepository.h"

class TaskSystem : public QWidget {
    Q_OBJECT
public:
    explicit TaskSystem(QWidget *parent = nullptr);

private slots:
    void loadTasks();
    void addTask();
    void editTask();
    void deleteTask();

private:
    void setupUi();
    
    TaskRepository m_repository;
    QTableView *m_table;
    QStandardItemModel *m_model;
    QPushButton *m_btnAdd;
    QPushButton *m_btnEdit;
    QPushButton *m_btnDelete;
};

#endif // TASKSYSTEM_H
```

```cpp
// ui/task/tasksystem.cpp
#include "tasksystem.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>

TaskSystem::TaskSystem(QWidget *parent) : QWidget(parent) {
    setupUi();
    loadTasks();
}

void TaskSystem::setupUi() {
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(40, 30, 40, 30);
    
    // Buttons
    auto btnLayout = new QHBoxLayout();
    m_btnAdd = new QPushButton("Add Task");
    m_btnAdd->setProperty("type", "primary");
    m_btnEdit = new QPushButton("Edit");
    m_btnDelete = new QPushButton("Delete");
    m_btnDelete->setProperty("type", "danger");
    
    btnLayout->addWidget(m_btnAdd);
    btnLayout->addWidget(m_btnEdit);
    btnLayout->addWidget(m_btnDelete);
    btnLayout->addStretch();
    
    connect(m_btnAdd, &QPushButton::clicked, this, &TaskSystem::addTask);
    connect(m_btnEdit, &QPushButton::clicked, this, &TaskSystem::editTask);
    connect(m_btnDelete, &QPushButton::clicked, this, &TaskSystem::deleteTask);
    
    // Table
    m_model = new QStandardItemModel(0, 5, this);
    m_model->setHorizontalHeaderLabels({"ID", "Title", "Due Date", "Status", "User ID"});
    
    m_table = new QTableView(this);
    m_table->setModel(m_model);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->verticalHeader()->hide();
    
    layout->addLayout(btnLayout);
    layout->addWidget(m_table);
}

void TaskSystem::loadTasks() {
    m_model->removeRows(0, m_model->rowCount());
    auto tasks = m_repository.getAllTasks();
    
    for (const auto& task : tasks) {
        int row = m_model->rowCount();
        m_model->insertRow(row);
        m_model->setData(m_model->index(row, 0), task.id);
        m_model->setData(m_model->index(row, 1), task.title);
        m_model->setData(m_model->index(row, 2), task.dueDate.toString("yyyy-MM-dd"));
        m_model->setData(m_model->index(row, 3), task.status);
        m_model->setData(m_model->index(row, 4), task.userId);
    }
}

void TaskSystem::addTask() {
    // Create dialog or inline form
    QMessageBox::information(this, "Add Task", "Implement add dialog here");
    loadTasks();  // Refresh
}

void TaskSystem::editTask() {
    auto selection = m_table->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, "No Selection", "Please select a task to edit.");
        return;
    }
    int id = m_model->data(selection.first().siblingAtColumn(0)).toInt();
    // Implement edit dialog
    loadTasks();
}

void TaskSystem::deleteTask() {
    auto selection = m_table->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, "No Selection", "Please select a task to delete.");
        return;
    }
    
    int id = m_model->data(selection.first().siblingAtColumn(0)).toInt();
    if (QMessageBox::question(this, "Confirm Delete", "Are you sure?") == QMessageBox::Yes) {
        if (m_repository.deleteTask(id)) {
            loadTasks();
        }
    }
}
```

### Step 5: Register in MainWindow

Add to `MainWindow::setupUi()`:

```cpp
// In mainwindow.cpp
#include "ui/task/tasksystem.h"

// Add to sidebar menu
new QListWidgetItem("Tasks", m_sidebar);

// Add to content stack
auto taskSystem = new TaskSystem(this);
addModule(taskSystem, "Tasks");
```

### Step 6: Update CMakeLists.txt

```cmake
set(PROJECT_SOURCES
    # ... existing files ...
    modules/task/taskrepository.h
    modules/task/taskrepository.cpp
    ui/task/tasksystem.h
    ui/task/tasksystem.cpp
)
```

---

## UI/UX Patterns

### Modern Design Principles

**Color Palette (Light Mode):**
- Background: `#F5F5F7` (Light gray)
- Content: `#FFFFFF` (White)
- Text: `#1D1D1F` (Dark)
- Accent: `#007AFF` (Blue)
- Borders: `#D1D1D6` (Light gray)
- Secondary Text: `#8E8E93` (Gray)

**Typography:**
- Font: `-apple-system, 'SF Pro Display', sans-serif`
- Base Size: `13px`
- Headings: `600` weight
- Body: `400` weight

**Components:**

1. **Buttons:**
   ```cpp
   // Primary Action
   button->setProperty("type", "primary");
   
   // Destructive
   button->setProperty("type", "danger");
   
   // Success
   button->setProperty("type", "success");
   ```

2. **Tables:**
   - Use `QStandardItemModel` with `QTableView`
   - Enable alternating row colors
   - Hide vertical headers
   - Stretch last column

3. **Layouts:**
   ```cpp
   layout->setContentsMargins(40, 30, 40, 30);  // Generous padding
   layout->setSpacing(20);  // Consistent spacing
   ```

4. **Cards/Widgets:**
   - White background (`#FFFFFF`)
   - Subtle border (`0.5px solid #D1D1D6`)
   - Border radius: `10px`
   - Light shadow effect

### ThemeManager

Use `ThemeManager` for consistent theming:

```cpp
#include "utils/thememanager.h"

// Apply theme
ThemeManager::applyTheme(ThemeManager::Light);  // or Dark

// Toggle theme
ThemeManager::toggleTheme();
```

---

## Database Setup

### SQLite (Default/Fallback)

**Pros:**
- No server required
- Perfect for development
- Single file database
- Cross-platform

**Cons:**
- Limited concurrent writes
- Not ideal for large-scale production

### MySQL (Optional)

**Configuration in `DatabaseManager::connect()`:**
```cpp
m_db = QSqlDatabase::addDatabase("QMYSQL");
m_db.setHostName("localhost");
m_db.setDatabaseName("your_app");
m_db.setUserName("your_user");
m_db.setPassword("your_password");
```

### Schema Initialization

```cpp
void DatabaseManager::initSchema() {
    QSqlQuery query;
    bool isSqlite = (m_db.driverName() == "QSQLITE");
    QString autoInc = isSqlite 
        ? "INTEGER PRIMARY KEY AUTOINCREMENT" 
        : "INT AUTO_INCREMENT PRIMARY KEY";
    
    // Create tables
    query.exec(QString("CREATE TABLE IF NOT EXISTS your_table ("
               "id %1, "
               "name VARCHAR(100))").arg(autoInc));
}
```

### Best Practices

1. **Use Prepared Statements:**
   ```cpp
   query.prepare("INSERT INTO users (name) VALUES (:name)");
   query.bindValue(":name", userName);
   query.exec();
   ```

2. **Handle Errors:**
   ```cpp
   if (!query.exec()) {
       qDebug() << "Error:" << query.lastError().text();
       return false;
   }
   ```

3. **Transactions for Multiple Operations:**
   ```cpp
   QSqlDatabase::database().transaction();
   // Multiple queries
   QSqlDatabase::database().commit();
   ```

---

## Build Configuration

### CMakeLists.txt Template

```cmake
cmake_minimum_required(VERSION 3.16)
project(YourApp VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    database/databasemanager.cpp
    database/databasemanager.h
    # Add your module files here
)

qt_add_executable(YourApp
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

target_link_libraries(YourApp PRIVATE 
    Qt${QT_VERSION_MAJOR}::Widgets 
    Qt${QT_VERSION_MAJOR}::Sql
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(YourApp)
endif()
```

### Build Commands

```bash
# Create build directory
mkdir build && cd build

# Configure
cmake ..

# Build
cmake --build .

# Run
./YourApp  # Linux/Mac
# or
YourApp.exe  # Windows
```

---

## Quick Reference Checklist

Creating a new module? Follow these steps:

- [ ] Create data model struct (`modules/[name]/[entity].h`)
- [ ] Create repository class (`modules/[name]/[entity]repository.h/cpp`)
- [ ] Add database table in `DatabaseManager::initSchema()`
- [ ] Create UI widget (`ui/[name]/[name]system.h/cpp`)
- [ ] Add menu item in `MainWindow::setupUi()`
- [ ] Register widget in content stack
- [ ] Update `CMakeLists.txt` with new files
- [ ] Test CRUD operations

---

## Tips & Best Practices

1. **Keep Repositories Simple**: Only data access logic, no business rules
2. **Use Optional Types**: Return `std::optional<T>` when value might not exist
3. **Error Handling**: Always check query execution results
4. **Consistent Naming**: Follow existing patterns (e.g., `StudentRepository`, `StudentSystem`)
5. **Role-Based Access**: Check user role in `MainWindow` before adding menu items
6. **Memory Management**: Qt's parent-child system handles most cleanup automatically
7. **SQL Injection**: Always use prepared statements with bindings
8. **Responsive UI**: Use layouts (not fixed positions) for flexible sizing

---

## Example: Minimal Module

Want a minimal example? Check `modules/student/` and `ui/studentportal.*` for a complete reference implementation.

---

**Happy Coding! 🚀**

