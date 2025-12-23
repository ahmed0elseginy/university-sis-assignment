# Documentation Overview

Complete guide to building modern Qt desktop applications using this architecture.

## 📚 Documentation Files

### 1. **BUILDING_GUIDE.md** (Main Guide)
Comprehensive documentation covering:
- Architecture overview with diagrams
- Project structure explanation
- Core components (DatabaseManager, MainWindow, Repositories)
- Module pattern and data flow
- Step-by-step module creation
- UI/UX patterns and theming
- Database setup (SQLite/MySQL)
- Build configuration (CMake)

**Start here for**: Understanding the complete architecture and design patterns.

### 2. **QUICK_START_TEMPLATE.md** (Copy-Paste Ready)
Ready-to-use code templates including:
- Complete minimal module example (Product)
- All necessary files (Model, Repository, UI, Dialog)
- Database schema integration
- MainWindow registration
- CMakeLists.txt updates

**Start here for**: Quick implementation - copy, paste, and adapt.

---

## 🚀 Quick Start (5 Steps)

### 1. Understand the Architecture
Read `BUILDING_GUIDE.md` → "Architecture Overview" section

### 2. Choose Your Approach
- **New to Qt?** → Read full `BUILDING_GUIDE.md`
- **Need it now?** → Jump to `QUICK_START_TEMPLATE.md`

### 3. Create Your First Module
Follow `QUICK_START_TEMPLATE.md` and replace "Product" with your entity name.

### 4. Integrate
Add to `MainWindow::setupUi()` and `CMakeLists.txt` as shown in the template.

### 5. Customize
Extend with custom dialogs, validation, search, filters as needed.

---

## 📋 Module Creation Checklist

When creating a new module, ensure you have:

- [ ] Data model struct (`modules/[name]/[entity].h`)
- [ ] Repository class (`modules/[name]/[entity]repository.h/cpp`)
- [ ] Database table in `DatabaseManager::initSchema()`
- [ ] UI widget (`ui/[name]/[name]system.h/cpp`)
- [ ] Menu item in `MainWindow::setupUi()`
- [ ] Widget registration in content stack
- [ ] Files added to `CMakeLists.txt`
- [ ] Tested CRUD operations

---

## 🎨 Design Philosophy

### Modern & Clean
- macOS-inspired design language
- Minimal, flat interface
- Consistent spacing and typography
- Light/Dark theme support

### Modular Architecture
- Each feature is self-contained
- Repository pattern for data access
- Clear separation: UI ↔ Business Logic ↔ Data

### Developer-Friendly
- Consistent patterns across modules
- Easy to extend and maintain
- Type-safe C++ with Qt framework

---

## 🔑 Key Concepts

### Repository Pattern
All database operations go through repository classes. This provides:
- Centralized data access logic
- Easy testing and mocking
- Clean separation from UI

### Singleton Database Manager
Single connection point for all database operations:
- Automatic SQLite/MySQL fallback
- Schema initialization
- Connection management

### Stacked Widget Navigation
Sidebar menu switches between module widgets in a stacked widget container.

---

## 📦 Project Structure Summary

```
your-app/
├── main.cpp                 # Entry point
├── mainwindow.h/cpp         # Application shell
├── database/
│   └── databasemanager.*    # DB connection singleton
├── modules/                 # Data layer
│   └── [module]/
│       ├── [entity].h
│       └── [entity]repository.*
├── ui/                      # Presentation layer
│   ├── login/
│   ├── [module]/
│   └── basesystemwidget.*
└── utils/
    └── thememanager.*       # Styling system
```

---

## 🛠️ Technology Stack

- **Framework**: Qt 6 (or Qt 5)
- **Language**: C++17
- **Database**: SQLite (default) or MySQL
- **Build System**: CMake 3.16+
- **UI**: Qt Widgets (native desktop)

---

## 💡 Best Practices

1. **Always use prepared statements** for SQL queries (prevents injection)
2. **Return `std::optional<T>`** when a value might not exist
3. **Check query execution results** - handle errors gracefully
4. **Use Qt's parent-child system** for automatic memory management
5. **Follow naming conventions** - be consistent with existing code
6. **Keep repositories simple** - only data access, no business logic
7. **Validate user input** before database operations

---

## 📖 Learning Path

### Beginner
1. Read `BUILDING_GUIDE.md` sections: Architecture, Project Structure
2. Study an existing module (e.g., `modules/student/`)
3. Copy `QUICK_START_TEMPLATE.md` and adapt it
4. Test your module thoroughly

### Intermediate
1. Understand Repository pattern deeply
2. Create custom dialogs for better UX
3. Add search/filter functionality
4. Implement role-based access control
5. Add data validation and error handling

### Advanced
1. Add unit tests for repositories
2. Implement caching strategies
3. Add export/import features
4. Create plugin architecture
5. Optimize database queries

---

## 🐛 Common Pitfalls

1. **Forgetting to add files to CMakeLists.txt** → Build errors
2. **Not checking query.exec() results** → Silent failures
3. **SQL injection** → Always use prepared statements with bindings
4. **Memory leaks** → Use Qt's parent-child ownership
5. **UI not updating** → Remember to call `loadData()` after changes

---

## 🔗 Related Files in This Project

- `university-sis/database/databasemanager.*` - Database singleton
- `university-sis/modules/student/` - Example module
- `university-sis/ui/studentportal.*` - Example UI widget
- `university-sis/mainwindow.*` - Application shell
- `university-sis/utils/thememanager.*` - Styling system

---

## 📞 Quick Reference

### Create Repository Method Template
```cpp
bool YourRepository::addEntity(const Entity& entity) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("INSERT INTO table_name (col1, col2) VALUES (:val1, :val2)");
    query.bindValue(":val1", entity.field1);
    query.bindValue(":val2", entity.field2);
    return query.exec();
}
```

### Load Table Template
```cpp
void YourSystem::loadData() {
    m_model->removeRows(0, m_model->rowCount());
    auto items = m_repository.getAllItems();
    for (const auto& item : items) {
        int row = m_model->rowCount();
        m_model->insertRow(row);
        m_model->setData(m_model->index(row, 0), item.field1);
        // ... more columns
    }
}
```

### Register Module Template
```cpp
// In MainWindow::setupUi()
new QListWidgetItem("Your Module", m_sidebar);
auto yourWidget = new YourSystem(this);
addModule(yourWidget, "Your Module");
```

---

**Happy Building! 🚀**

For detailed information, refer to `BUILDING_GUIDE.md` and `QUICK_START_TEMPLATE.md`.

