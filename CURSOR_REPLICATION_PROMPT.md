# Complete Prompt for Replicating University SIS Application

Copy and paste this entire prompt into a new Cursor session to recreate the University Student Information System application.

---

## Project Overview

Create a modern C++ desktop application called "University SIS" (Student Information System) using Qt 6 framework. This is a comprehensive university management system with role-based access control, multiple modules, and a modern macOS-inspired UI design.

## Technology Stack

- **Framework**: Qt 6 (Widgets, Sql components)
- **Language**: C++17
- **Database**: SQLite (default) with MySQL support
- **Build System**: CMake 3.16+
- **UI Framework**: Qt Widgets
- **Platform**: Windows Desktop Application

## Project Structure

```
university-sis/
├── main.cpp                          # Application entry point
├── mainwindow.h/cpp                  # Main application shell with sidebar navigation
├── CMakeLists.txt                    # Build configuration
├── resources.qrc                     # Qt resource file
├── database/
│   ├── databasemanager.h/cpp         # Singleton database connection manager
│   └── schema.sql                    # SQL schema reference
├── modules/                          # Data layer (Repository pattern)
│   ├── student/
│   │   ├── student.h                 # Student data model struct
│   │   └── studentrepository.h/cpp   # Student CRUD operations
│   ├── academic/
│   │   ├── course.h
│   │   ├── courserepository.h/cpp
│   │   ├── section.h
│   │   └── sectionrepository.h/cpp
│   ├── enrollment/
│   │   ├── enrollment.h
│   │   └── enrollmentrepository.h/cpp
│   ├── attendance/
│   │   ├── attendance.h
│   │   └── attendancerepository.h/cpp
│   ├── faculty/
│   │   ├── faculty.h
│   │   └── facultyrepository.h/cpp
│   ├── finance/
│   │   ├── payment.h
│   │   └── paymentrepository.h/cpp
│   └── facility/
│       ├── building.h
│       ├── room.h
│       └── facilityrepository.h/cpp
├── ui/                               # Presentation layer
│   ├── login/
│   │   └── logindialog.h/cpp         # Login screen with role-based auth
│   ├── basesystemwidget.h/cpp        # Base class for all system modules
│   ├── studentportal.h/cpp           # Student dashboard view
│   ├── profile/
│   │   └── profilewidget.h/cpp       # User profile view
│   ├── student/
│   │   └── studentdialog.h/cpp       # Student CRUD dialog
│   ├── academic/
│   │   ├── academicsystem.h/cpp      # Academic management UI
│   │   ├── coursedialog.h/cpp
│   │   └── sectiondialog.h/cpp
│   ├── enrollment/
│   │   ├── enrollmentsystem.h/cpp
│   │   └── enrollmentdialog.h/cpp
│   ├── attendance/
│   │   ├── attendancesystem.h/cpp
│   │   └── attendancedialog.h/cpp
│   ├── faculty/
│   │   ├── facultysystem.h/cpp
│   │   └── facultydialog.h/cpp
│   ├── finance/
│   │   ├── financesystem.h/cpp
│   │   └── paymentdialog.h/cpp
│   ├── facility/
│   │   ├── facilitysystem.h/cpp
│   │   ├── buildingdialog.h/cpp
│   │   └── roomdialog.h/cpp
│   ├── calendar/
│   │   └── calendarsystem.h/cpp
│   ├── grades/
│   │   └── gradessystem.h/cpp
│   ├── library/
│   │   └── librarysystem.h/cpp
│   ├── news/
│   │   └── newssystem.h/cpp
│   └── reports/
│       └── reportssystem.h/cpp
└── utils/
    └── thememanager.h/cpp             # Light/Dark theme management
```

## Core Architecture

### 1. Database Manager (Singleton Pattern)
- **Location**: `database/databasemanager.h/cpp`
- **Purpose**: Centralized database connection management
- **Features**:
  - Auto-detects SQLite or MySQL
  - Creates database schema automatically if tables don't exist
  - Seeds sample data on first run
  - Singleton pattern ensures single connection instance
- **Connection Details**:
  - SQLite: `university_sis.db` (default, local file)
  - MySQL: `localhost`, database: `university_sis`, user: `university_sis`, password: `university_sis@123`

### 2. Repository Pattern
- Each module has a repository class that handles all database operations
- Methods: `add*()`, `update*()`, `delete*()`, `getAll*()`, `getById()`, `search*()`
- All repositories use prepared statements to prevent SQL injection
- Return `bool` for success/failure, or `std::optional<T>` for queries

### 3. UI Architecture
- **MainWindow**: Contains sidebar navigation (QListWidget) and content area (QStackedWidget)
- **BaseSystemWidget**: Base class for all module UIs ensuring consistent styling
- **Role-Based Menu**: Sidebar items filtered by user role
- **Theme System**: Light/Dark theme toggle with ThemeManager

### 4. User Roles & Access Control
- **Administrator**: Full access to all modules
- **Student**: Limited access (Student Portal, Enrollment, Attendance view, Finance view)
- **Faculty Member**: Academic management, Attendance, Student Portal
- **Finance Officer**: Finance module, Dashboard

## Database Schema

### Core Tables

```sql
-- Users (Authentication)
CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username VARCHAR(50) UNIQUE NOT NULL,
    password VARCHAR(255) NOT NULL,
    role VARCHAR(20) DEFAULT 'student',
    user_id INTEGER,  -- Links to student/faculty ID
    FOREIGN KEY (user_id) REFERENCES students(id) ON DELETE CASCADE
);

-- Students
CREATE TABLE IF NOT EXISTS students (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name VARCHAR(100) NOT NULL,
    year INTEGER,
    department VARCHAR(100),
    section_id INTEGER
);

-- Courses
CREATE TABLE IF NOT EXISTS courses (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name VARCHAR(200) NOT NULL,
    year INTEGER,
    hours INTEGER
);

-- Sections
CREATE TABLE IF NOT EXISTS sections (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    course_id INTEGER NOT NULL,
    max_capacity INTEGER,
    FOREIGN KEY (course_id) REFERENCES courses(id) ON DELETE CASCADE
);

-- Enrollments
CREATE TABLE IF NOT EXISTS enrollments (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    student_id INTEGER NOT NULL,
    section_id INTEGER NOT NULL,
    enrollment_date DATE,
    FOREIGN KEY (student_id) REFERENCES students(id) ON DELETE CASCADE,
    FOREIGN KEY (section_id) REFERENCES sections(id) ON DELETE CASCADE
);

-- Attendance
CREATE TABLE IF NOT EXISTS attendance (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    student_id INTEGER NOT NULL,
    section_id INTEGER NOT NULL,
    date DATE NOT NULL,
    status VARCHAR(20) NOT NULL,  -- 'Present', 'Absent', 'Late'
    FOREIGN KEY (student_id) REFERENCES students(id) ON DELETE CASCADE,
    FOREIGN KEY (section_id) REFERENCES sections(id) ON DELETE CASCADE
);

-- Grades
CREATE TABLE IF NOT EXISTS grades (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    enrollment_id INTEGER NOT NULL,
    a1_score DECIMAL(5,2),
    a2_score DECIMAL(5,2),
    final_exam DECIMAL(5,2),
    total_grade DECIMAL(5,2),
    FOREIGN KEY (enrollment_id) REFERENCES enrollments(id) ON DELETE CASCADE
);

-- Faculty
CREATE TABLE IF NOT EXISTS faculty (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name VARCHAR(100) NOT NULL,
    department VARCHAR(100),
    position VARCHAR(100)
);

-- Payments
CREATE TABLE IF NOT EXISTS payments (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    student_id INTEGER NOT NULL,
    amount DECIMAL(10,2) NOT NULL,
    description TEXT,
    status VARCHAR(20) DEFAULT 'Pending',  -- 'Paid', 'Pending', 'Overdue'
    payment_date DATE,
    FOREIGN KEY (student_id) REFERENCES students(id) ON DELETE CASCADE
);

-- Buildings
CREATE TABLE IF NOT EXISTS buildings (
    building_id INTEGER PRIMARY KEY AUTOINCREMENT,
    name VARCHAR(100),
    code VARCHAR(20) UNIQUE,
    location VARCHAR(200)
);

-- Rooms
CREATE TABLE IF NOT EXISTS rooms (
    room_id INTEGER PRIMARY KEY AUTOINCREMENT,
    building_id INTEGER,
    room_number VARCHAR(20),
    type VARCHAR(50),
    capacity INTEGER,
    FOREIGN KEY (building_id) REFERENCES buildings(building_id) ON DELETE CASCADE
);

-- Calendar Events
CREATE TABLE IF NOT EXISTS calendar_events (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    date DATE NOT NULL,
    time TIME,
    title VARCHAR(200) NOT NULL,
    type VARCHAR(50),
    description TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Library Books
CREATE TABLE IF NOT EXISTS books (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    isbn VARCHAR(50) UNIQUE,
    title VARCHAR(200) NOT NULL,
    author VARCHAR(100),
    publisher VARCHAR(100),
    year INTEGER,
    category VARCHAR(100),
    total_copies INTEGER DEFAULT 1,
    available_copies INTEGER DEFAULT 1,
    location VARCHAR(100)
);

-- Library Loans
CREATE TABLE IF NOT EXISTS loans (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    book_id INTEGER NOT NULL,
    student_id INTEGER NOT NULL,
    checkout_date DATE NOT NULL,
    due_date DATE NOT NULL,
    return_date DATE,
    status VARCHAR(20) DEFAULT 'Active',  -- 'Active', 'Returned', 'Overdue'
    FOREIGN KEY (book_id) REFERENCES books(id) ON DELETE CASCADE,
    FOREIGN KEY (student_id) REFERENCES students(id) ON DELETE CASCADE
);

-- News/Announcements
CREATE TABLE IF NOT EXISTS news (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    title VARCHAR(200) NOT NULL,
    content TEXT,
    target_role VARCHAR(20),  -- 'All', 'Student', 'Faculty', etc.
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

## UI Design Specifications

### Design Language: macOS-Inspired Modern UI

#### Color Scheme (Light Theme)
- Background: `#F5F5F7`
- Card Background: `#FFFFFF`
- Border: `#D1D1D6`
- Primary Text: `#1D1D1F`
- Secondary Text: `#8E8E93`
- Accent: `#007AFF`
- Shadow: `rgba(0, 0, 0, 0.08)`

#### Color Scheme (Dark Theme)
- Background: `#1C1C1E`
- Card Background: `#2C2C2E`
- Border: `#3A3A3C`
- Primary Text: `#FFFFFF`
- Secondary Text: `#8E8E93`
- Accent: `#0A84FF`
- Shadow: `rgba(0, 0, 0, 0.3)`

#### Typography
- Headers: 32px, font-weight: 600
- Titles: 13px, font-weight: 400
- Body: 15px, font-weight: 400
- Buttons: 15px, font-weight: 500

#### Component Styles
- **Cards**: Rounded corners (10px), subtle shadow, 0.5px border
- **Buttons**: Rounded (8px), flat design, hover effects
- **Input Fields**: Rounded (8px), border on focus
- **Sidebar**: Fixed width (260px), light background
- **Table**: Clean borders, alternating row colors

### MainWindow Layout
- **Left Sidebar**: Fixed width 260px, QListWidget with menu items
- **Right Content**: QStackedWidget containing all module widgets
- **Top Bar**: Theme toggle button, user info
- **Status Bar**: Connection status, user role

## Module Specifications

### 1. Login System (`ui/login/logindialog.h/cpp`)
- Username/password input fields
- Role-based authentication
- Returns session info (username, role, userId)
- Background image support
- Error handling for invalid credentials

### 2. Dashboard
- Statistics cards showing:
  - Total Courses
  - Total Students
  - Total Faculty
  - Library Books
  - Facilities
  - Revenue
- Real-time data from database
- Refresh button
- macOS-style card design

### 3. Student Management
- **Model**: `modules/student/student.h` (id, name, year, department, section_id)
- **Repository**: Full CRUD operations
- **UI**: Table view with Add/Edit/Delete buttons
- **Dialog**: Form with all student fields
- **Search**: Filter by name, department, year

### 4. Academic System
- **Courses**: Create, edit, delete courses (name, year, hours)
- **Sections**: Create sections linked to courses with max capacity
- **UI**: Tabbed interface (Courses tab, Sections tab)
- **Validation**: Section capacity checking

### 5. Enrollment System
- Enroll students in sections
- View all enrollments
- Drop enrollments
- Check section capacity before enrollment
- Filter by student, course, section

### 6. Attendance System
- Record attendance (Present/Absent/Late)
- View attendance by course, date, student
- Edit/Delete attendance records
- Calculate attendance percentage
- Calendar-based date selection

### 7. Grades System
- Record grades (A1, A2, Final Exam)
- Calculate total grade automatically
- View grades by student, course
- Edit/Delete grades
- GPA calculation
- Display in student portal

### 8. Finance System
- Create payment records
- Track payment status (Paid/Pending/Overdue)
- View payments by student
- Calculate revenue totals
- Payment date tracking

### 9. Faculty Management
- Add/Edit/Delete faculty members
- Department and position tracking
- Faculty authentication support
- Search and filter

### 10. Facility Management
- **Buildings**: Add buildings with code and location
- **Rooms**: Add rooms linked to buildings (type, capacity)
- Hierarchical view (Buildings → Rooms)
- Search functionality

### 11. Calendar System
- Create calendar events (Class, Exam, Meeting, etc.)
- Date and time selection
- Event types with color coding
- View calendar grid/list
- Edit/Delete events

### 12. Library System
- **Books**: Add books (ISBN, title, author, publisher, year, category)
- Track total and available copies
- Book location tracking
- **Loans**: Checkout/Return books
- Due date tracking
- Overdue detection
- Loan history

### 13. News & Announcements
- Create announcements
- Target specific roles (All, Student, Faculty, etc.)
- Rich text content
- Date/time tracking
- View/Edit/Delete announcements

### 14. Student Portal
- Personal dashboard for students
- View enrolled courses
- View grades and GPA
- View attendance records
- View financial/payment information
- Upcoming events
- Quick statistics

### 15. Reports System
- Generate reports for:
  - Students
  - Courses
  - Attendance
  - Financial
- Statistics and analytics
- Export functionality (future)

### 16. Profile Widget
- Display user information
- Show role
- Logout functionality
- Edit profile (future)

## Implementation Requirements

### CMakeLists.txt Structure
```cmake
cmake_minimum_required(VERSION 3.16)
project(university-sis VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql)

# Add all source files
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    # ... all other source files
    resources.qrc
)

qt_add_executable(university-sis ${PROJECT_SOURCES})
target_link_libraries(university-sis PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql)
```

### Key Implementation Patterns

#### Repository Pattern Example
```cpp
class StudentRepository {
public:
    bool addStudent(const Student& student);
    bool updateStudent(int id, const Student& student);
    bool deleteStudent(int id);
    std::vector<Student> getAllStudents();
    std::optional<Student> getStudentById(int id);
    std::vector<Student> searchStudents(const QString& query);
};
```

#### UI Module Pattern
```cpp
class StudentSystem : public BaseSystemWidget {
    Q_OBJECT
public:
    explicit StudentSystem(QWidget* parent = nullptr);
    
private slots:
    void onAddClicked();
    void onEditClicked();
    void onDeleteClicked();
    void loadData();
    
private:
    QTableView* m_table;
    QStandardItemModel* m_model;
    StudentRepository m_repository;
};
```

#### Database Manager Pattern
```cpp
class DatabaseManager {
public:
    static DatabaseManager& instance();
    bool connect();
    QSqlDatabase getDatabase() const;
    void initSchema();
    void seedSampleData();
    
private:
    DatabaseManager();
    QSqlDatabase m_db;
};
```

## Sample Data Seeding

On first run, seed the database with:
- Default admin user: username `admin`, password `1234`, role `Administrator`
- Sample students (3-5 students)
- Sample courses (5-10 courses)
- Sample sections (linked to courses)
- Sample faculty (2-3 faculty members)
- Sample buildings and rooms

## Build Instructions

1. **Prerequisites**:
   - Qt 6 (Widgets, Sql components)
   - CMake 3.16+
   - C++17 compiler
   - SQLite (included with Qt) or MySQL server

2. **Build Steps**:
   ```bash
   mkdir build
   cd build
   cmake ..
   cmake --build .
   ```

3. **Run**:
   - Windows: `build/university-sis.exe`
   - Linux/Mac: `./build/university-sis`

## Features Checklist

### ✅ Core Features (Must Implement)
- [ ] User authentication with role-based access
- [ ] Dashboard with statistics
- [ ] Student CRUD operations
- [ ] Course and Section management
- [ ] Enrollment system
- [ ] Attendance tracking
- [ ] Grades management
- [ ] Finance/Payment tracking
- [ ] Faculty management
- [ ] Facility management (Buildings/Rooms)
- [ ] Calendar system
- [ ] Library system (Books/Loans)
- [ ] News/Announcements
- [ ] Student Portal
- [ ] Profile view
- [ ] Light/Dark theme toggle

### 🟡 Enhanced Features (Recommended)
- [ ] Search and filter in all modules
- [ ] Input validation
- [ ] Error handling and user feedback
- [ ] Data export (CSV/PDF)
- [ ] Report generation
- [ ] Bulk operations

## Code Quality Standards

1. **Use prepared statements** for all SQL queries
2. **Check query execution** results before proceeding
3. **Use Qt's parent-child** system for memory management
4. **Follow naming conventions**: camelCase for variables, PascalCase for classes
5. **Separate concerns**: UI logic separate from business logic
6. **Error handling**: Show user-friendly error messages
7. **Input validation**: Validate all user inputs before database operations

## Testing Requirements

- Test all CRUD operations for each module
- Test role-based access control
- Test database connection (SQLite and MySQL)
- Test theme switching
- Test search and filter functionality
- Test data validation

## Additional Notes

- The application should gracefully handle database connection failures
- All dialogs should have proper validation
- Use Qt's signal/slot mechanism for communication
- Follow Qt's coding style guidelines
- Ensure the UI is responsive and intuitive
- Add loading indicators for long operations
- Implement proper error messages

---

## Start Building

Begin by creating the project structure, then implement in this order:

1. **Phase 1**: Database Manager, Login System, MainWindow shell
2. **Phase 2**: Student module (as template), Repository pattern
3. **Phase 3**: All other modules following the same pattern
4. **Phase 4**: Student Portal, Dashboard, Theme system
5. **Phase 5**: Polish, validation, error handling

Use the existing codebase as reference for patterns, but implement everything from scratch following these specifications.

---

**End of Prompt**

