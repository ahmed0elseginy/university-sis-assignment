#include "databasemanager.h"
#include <QStandardPaths>
#include <QDir>
#include <QCoreApplication>

DatabaseManager::DatabaseManager()
{
}

DatabaseManager::~DatabaseManager()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::connect()
{
    if (m_db.isOpen()) {
        return true;
    }

    // Check available drivers
    if (QSqlDatabase::isDriverAvailable("QMYSQL")) {
        m_db = QSqlDatabase::addDatabase("QMYSQL");
        m_db.setHostName("localhost");
        m_db.setDatabaseName("university_sis");
        m_db.setUserName("university_sis");
        m_db.setPassword("university_sis@123");
        qDebug() << "Using MySQL Database";
    } else {
        // Fallback to SQLite if MySQL driver is missing (Common on Windows/Qt)
        qDebug() << "MySQL Driver (QMYSQL) not found. Falling back to SQLite.";
        m_db = QSqlDatabase::addDatabase("QSQLITE");
        
        // Save database in the application directory or standard location
        QString dbPath = QDir(QCoreApplication::applicationDirPath()).filePath("university_sis.db");
        m_db.setDatabaseName(dbPath);
        qDebug() << "SQLite Database Path:" << dbPath;
    }

    if (!m_db.open()) {
        qDebug() << "Error: Connection with database failed";
        qDebug() << m_db.lastError().text();
        return false;
    } else {
        qDebug() << "Database: connection ok";
        // Enable foreign keys for SQLite
        if (m_db.driverName() == "QSQLITE") {
            QSqlQuery query;
            query.exec("PRAGMA foreign_keys = ON");
        }
        
        initSchema();
        return true;
    }
}

bool DatabaseManager::isOpen() const
{
    return m_db.isOpen();
}

QSqlDatabase DatabaseManager::getDatabase() const
{
    return m_db;
}

void DatabaseManager::initSchema()
{
    QSqlQuery query;
    bool isSqlite = (m_db.driverName() == "QSQLITE");
    
    QString autoInc = isSqlite ? "INTEGER PRIMARY KEY AUTOINCREMENT" : "INT AUTO_INCREMENT PRIMARY KEY";
    
    // Users Table
    query.exec(QString("CREATE TABLE IF NOT EXISTS users ("
               "user_id %1, "
               "password_hash VARCHAR(100) NOT NULL, "
               "role VARCHAR(20) NOT NULL, "
               "display_name VARCHAR(100))").arg(autoInc));

    // Students Table - UPDATED with username/password
    // Note: If table exists, these columns might be missing. Using simple check.
    query.exec(QString("CREATE TABLE IF NOT EXISTS students ("
               "student_id %1, "
               "name VARCHAR(100), "
               "year INT, "
               "department VARCHAR(50), "
               "section_id INT, "
               "username VARCHAR(50) UNIQUE, "
               "password VARCHAR(50))").arg(autoInc));
    
    // Attempt to alter table if columns missing (Migration logic)
    query.exec("ALTER TABLE students ADD COLUMN username VARCHAR(50)");
    query.exec("ALTER TABLE students ADD COLUMN password VARCHAR(50)");
    query.exec("CREATE UNIQUE INDEX IF NOT EXISTS idx_student_username ON students(username)");

    // Faculty Table
    query.exec(QString("CREATE TABLE IF NOT EXISTS faculty ("
               "faculty_id %1, "
               "name VARCHAR(100), "
               "email VARCHAR(100), "
               "department VARCHAR(50), "
               "position VARCHAR(50), "
               "username VARCHAR(50) UNIQUE, "
               "password VARCHAR(50))").arg(autoInc));

    // Migration for Faculty
    query.exec("ALTER TABLE faculty ADD COLUMN username VARCHAR(50)");
    query.exec("ALTER TABLE faculty ADD COLUMN password VARCHAR(50)");
    query.exec("CREATE UNIQUE INDEX IF NOT EXISTS idx_faculty_username ON faculty(username)");

    // Courses Table
    query.exec(QString("CREATE TABLE IF NOT EXISTS courses ("
               "course_id %1, "
               "name VARCHAR(100), "
               "year INT, "
               "hours INT)").arg(autoInc));

    // Sections Table
    query.exec(QString("CREATE TABLE IF NOT EXISTS sections ("
               "section_id %1, "
               "course_id INT, "
               "max_students INT, "
               "FOREIGN KEY (course_id) REFERENCES courses(course_id))").arg(autoInc));

    // Student_Section
    query.exec("CREATE TABLE IF NOT EXISTS student_section ("
               "student_id INT, "
               "section_id INT, "
               "PRIMARY KEY (student_id, section_id), "
               "FOREIGN KEY (student_id) REFERENCES students(student_id), "
               "FOREIGN KEY (section_id) REFERENCES sections(section_id))");

    // Grades Table
    query.exec("CREATE TABLE IF NOT EXISTS grades ("
               "student_id INT, "
               "course_id INT, "
               "a1 VARCHAR(2), "
               "a2 VARCHAR(2), "
               "final_exam VARCHAR(2), "
               "total VARCHAR(10), "
               "PRIMARY KEY (student_id, course_id), "
               "FOREIGN KEY (student_id) REFERENCES students(student_id), "
               "FOREIGN KEY (course_id) REFERENCES courses(course_id))");
               
    // SCALED ARCHITECTURE TABLES
    
    // 5. Payment & Finance
    query.exec(QString("CREATE TABLE IF NOT EXISTS payments ("
               "payment_id %1, "
               "student_id INT, "
               "amount DECIMAL(10,2), "
               "description VARCHAR(255), "
               "status VARCHAR(20), " // Paid, Pending, Overdue
               "date DATE, "
               "FOREIGN KEY (student_id) REFERENCES students(student_id))").arg(autoInc));

    // 6. Attendance
    query.exec(QString("CREATE TABLE IF NOT EXISTS attendance ("
               "attendance_id %1, "
               "student_id INT, "
               "course_id INT, "
               "date DATE, "
               "status VARCHAR(20), " // Present, Absent, Late
               "FOREIGN KEY (student_id) REFERENCES students(student_id), "
               "FOREIGN KEY (course_id) REFERENCES courses(course_id))").arg(autoInc));
               
    // 9. Notifications / News
    query.exec(QString("CREATE TABLE IF NOT EXISTS announcements ("
               "id %1, "
               "title VARCHAR(255), "
               "content TEXT, "
               "date DATETIME, "
               "target_role VARCHAR(50))").arg(autoInc)); // All, Student, Faculty

    // Infrastructure / Facility System
    query.exec(QString("CREATE TABLE IF NOT EXISTS buildings ("
               "building_id %1, "
               "name VARCHAR(100), "
               "code VARCHAR(20) UNIQUE, "
               "location VARCHAR(200))").arg(autoInc));

    query.exec(QString("CREATE TABLE IF NOT EXISTS rooms ("
               "room_id %1, "
               "building_id INT, "
               "room_number VARCHAR(20), "
               "type VARCHAR(50), " // Lab, Lecture Hall, etc.
               "capacity INT, "
               "FOREIGN KEY (building_id) REFERENCES buildings(building_id) ON DELETE CASCADE)").arg(autoInc));

    // Calendar Events Table
    query.exec(QString("CREATE TABLE IF NOT EXISTS calendar_events ("
               "id %1, "
               "date DATE NOT NULL, "
               "time TIME, "
               "title VARCHAR(200) NOT NULL, "
               "type VARCHAR(50), " // e.g., Class, Exam, Assignment Due, Meeting, Holiday, Other
               "description TEXT, "
               "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)").arg(autoInc));

    // Library Books Table
    query.exec(QString("CREATE TABLE IF NOT EXISTS books ("
               "book_id %1, "
               "isbn VARCHAR(20) UNIQUE, "
               "title VARCHAR(255) NOT NULL, "
               "author VARCHAR(255), "
               "publisher VARCHAR(255), "
               "publication_year INT, "
               "category VARCHAR(100), "
               "total_copies INT DEFAULT 1, "
               "available_copies INT DEFAULT 1, "
               "location VARCHAR(100))").arg(autoInc));

    // Book Loans/Transactions Table
    query.exec(QString("CREATE TABLE IF NOT EXISTS book_loans ("
               "loan_id %1, "
               "book_id INT NOT NULL, "
               "student_id INT, "
               "faculty_id INT, "
               "checkout_date DATE NOT NULL, "
               "due_date DATE NOT NULL, "
               "return_date DATE, "
               "status VARCHAR(20) DEFAULT 'Checked Out', " // Checked Out, Returned, Overdue
               "FOREIGN KEY (book_id) REFERENCES books(book_id) ON DELETE CASCADE, "
               "FOREIGN KEY (student_id) REFERENCES students(student_id) ON DELETE SET NULL, "
               "FOREIGN KEY (faculty_id) REFERENCES faculty(faculty_id) ON DELETE SET NULL)").arg(autoInc));

     // Seed admin user if not exists
     QSqlQuery seed;
     // SQLite uses INSERT OR IGNORE, MySQL uses INSERT IGNORE.
     // Standard SQL: check existence first or catch error. Simplest cross-db approach:
     seed.prepare("INSERT INTO users (user_id, password_hash, role, display_name) VALUES (1, '1234', 'ADMIN', 'Administrator')");
     if(!seed.exec()) {
         // Ignore error if already exists (constraint violation)
     }
     
     // Seed sample data
     seedSampleData();
}

void DatabaseManager::seedSampleData()
{
    QSqlQuery query(m_db);
    
    // Check if data already exists
    query.exec("SELECT COUNT(*) FROM students");
    bool hasData = false;
    if (query.next() && query.value(0).toInt() > 0) {
        hasData = true;
    }
    
    if (hasData) {
        return; // Don't seed if data already exists
    }
    
    // Seed Students
    query.exec("INSERT INTO students (name, year, department, username, password) VALUES "
               "('John Smith', 2, 'Computer Science', 'john.smith', 'pass123'), "
               "('Emily Johnson', 3, 'Mathematics', 'emily.j', 'pass123'), "
               "('Michael Brown', 1, 'Physics', 'michael.b', 'pass123'), "
               "('Sarah Davis', 4, 'Computer Science', 'sarah.d', 'pass123'), "
               "('David Wilson', 2, 'Engineering', 'david.w', 'pass123'), "
               "('Jessica Martinez', 3, 'Biology', 'jessica.m', 'pass123'), "
               "('Robert Taylor', 1, 'Chemistry', 'robert.t', 'pass123'), "
               "('Amanda Anderson', 4, 'Mathematics', 'amanda.a', 'pass123')");
    
    // Seed Faculty
    query.exec("INSERT INTO faculty (name, email, department, position, username, password) VALUES "
               "('Dr. James Miller', 'james.miller@university.edu', 'Computer Science', 'Professor', 'james.m', 'pass123'), "
               "('Dr. Patricia White', 'patricia.w@university.edu', 'Mathematics', 'Associate Professor', 'patricia.w', 'pass123'), "
               "('Dr. Robert Harris', 'robert.h@university.edu', 'Physics', 'Professor', 'robert.h', 'pass123'), "
               "('Dr. Linda Martin', 'linda.m@university.edu', 'Biology', 'Assistant Professor', 'linda.m', 'pass123'), "
               "('Dr. William Thompson', 'william.t@university.edu', 'Engineering', 'Professor', 'william.t', 'pass123')");
    
    // Seed Courses
    query.exec("INSERT INTO courses (name, year, hours) VALUES "
               "('Introduction to Programming', 1, 3), "
               "('Data Structures', 2, 4), "
               "('Database Systems', 3, 3), "
               "('Software Engineering', 4, 4), "
               "('Calculus I', 1, 4), "
               "('Linear Algebra', 2, 3), "
               "('Physics I', 1, 4), "
               "('Chemistry Fundamentals', 1, 3), "
               "('Biology 101', 1, 3), "
               "('Advanced Algorithms', 3, 4), "
               "('Machine Learning', 4, 3), "
               "('Computer Networks', 3, 3), "
               "('Operating Systems', 3, 4), "
               "('Web Development', 2, 3), "
               "('Mobile App Development', 3, 3), "
               "('Cybersecurity', 4, 3), "
               "('Artificial Intelligence', 4, 4), "
               "('Distributed Systems', 4, 3), "
               "('Computer Graphics', 3, 3), "
               "('Digital Signal Processing', 4, 3), "
               "('Embedded Systems', 3, 4), "
               "('Cloud Computing', 4, 3), "
               "('Big Data Analytics', 4, 3), "
               "('Quantum Computing', 4, 3)");
    
    // Seed Buildings
    query.exec("INSERT INTO buildings (name, code, location) VALUES "
               "('Science Building', 'SCI', 'North Campus'), "
               "('Engineering Hall', 'ENG', 'East Campus'), "
               "('Library Building', 'LIB', 'Central Campus'), "
               "('Arts Center', 'ART', 'South Campus'), "
               "('Student Center', 'STU', 'Central Campus'), "
               "('Computer Science Building', 'CS', 'North Campus'), "
               "('Mathematics Building', 'MATH', 'East Campus'), "
               "('Biology Lab', 'BIO', 'North Campus'), "
               "('Chemistry Lab', 'CHEM', 'North Campus'), "
               "('Physics Lab', 'PHYS', 'North Campus'), "
               "('Engineering Lab', 'ENGLAB', 'East Campus'), "
               "('Research Center', 'RES', 'South Campus')");
    
    // Seed Rooms
    query.exec("INSERT INTO rooms (building_id, room_number, type, capacity) VALUES "
               "(1, '101', 'Lecture Hall', 100), (1, '102', 'Lecture Hall', 80), (1, '201', 'Lab', 30), "
               "(2, '101', 'Lecture Hall', 120), (2, '202', 'Lab', 25), (2, '301', 'Lecture Hall', 90), "
               "(3, '101', 'Study Room', 20), (3, '201', 'Reading Room', 50), (3, '301', 'Conference Room', 15), "
               "(4, '101', 'Studio', 25), (4, '201', 'Workshop', 30), "
               "(5, '101', 'Meeting Room', 50), (5, '201', 'Event Hall', 200), "
               "(6, '101', 'Computer Lab', 40), (6, '201', 'Lecture Hall', 100), "
               "(7, '101', 'Lecture Hall', 80), (7, '201', 'Seminar Room', 30), "
               "(8, '101', 'Lab', 20), (8, '201', 'Lab', 20), "
               "(9, '101', 'Lab', 25), (9, '201', 'Lab', 25), "
               "(10, '101', 'Lab', 30), (10, '201', 'Lab', 30), "
               "(11, '101', 'Workshop', 35), (11, '201', 'Lab', 30), "
               "(12, '101', 'Conference Room', 20), (12, '201', 'Research Lab', 15)");
    
    // Seed Payments (sample revenue data)
    query.exec("INSERT INTO payments (student_id, amount, description, status, date) VALUES "
               "(1, 5000.00, 'Tuition Fee - Fall 2024', 'Paid', '2024-09-01'), "
               "(2, 5000.00, 'Tuition Fee - Fall 2024', 'Paid', '2024-09-01'), "
               "(3, 5000.00, 'Tuition Fee - Fall 2024', 'Paid', '2024-09-02'), "
               "(4, 5000.00, 'Tuition Fee - Fall 2024', 'Paid', '2024-09-02'), "
               "(5, 5000.00, 'Tuition Fee - Fall 2024', 'Paid', '2024-09-03'), "
               "(6, 5000.00, 'Tuition Fee - Fall 2024', 'Paid', '2024-09-03'), "
               "(7, 5000.00, 'Tuition Fee - Fall 2024', 'Paid', '2024-09-04'), "
               "(8, 5000.00, 'Tuition Fee - Fall 2024', 'Paid', '2024-09-04'), "
               "(1, 500.00, 'Lab Fee', 'Paid', '2024-09-15'), "
               "(2, 500.00, 'Lab Fee', 'Paid', '2024-09-15'), "
               "(3, 500.00, 'Lab Fee', 'Paid', '2024-09-16'), "
               "(4, 500.00, 'Lab Fee', 'Paid', '2024-09-16')");
    
    // Seed Library Books
    query.exec("INSERT INTO books (isbn, title, author, publisher, publication_year, category, total_copies, available_copies, location) VALUES "
               "('978-0131103627', 'The C++ Programming Language', 'Bjarne Stroustrup', 'Addison-Wesley', 2013, 'Computer Science', 5, 3, 'Main Library - CS Section'), "
               "('978-0201633610', 'Design Patterns: Elements of Reusable Object-Oriented Software', 'Erich Gamma, Richard Helm, Ralph Johnson, John Vlissides', 'Addison-Wesley', 1994, 'Computer Science', 3, 1, 'Main Library - CS Section'), "
               "('978-0132350884', 'Clean Code: A Handbook of Agile Software Craftsmanship', 'Robert C. Martin', 'Prentice Hall', 2008, 'Computer Science', 4, 4, 'Main Library - CS Section'), "
               "('978-0321751041', 'The Art of Computer Programming', 'Donald Knuth', 'Addison-Wesley', 2011, 'Computer Science', 2, 2, 'Main Library - Reference'), "
               "('978-1449331818', 'Learning React: Modern Patterns for Developing React Apps', 'Alex Banks, Eve Porcello', 'O''Reilly Media', 2020, 'Computer Science', 3, 2, 'Main Library - CS Section'), "
               "('978-0596007126', 'Head First Design Patterns', 'Eric Freeman, Elisabeth Robson', 'O''Reilly Media', 2004, 'Computer Science', 4, 3, 'Main Library - CS Section'), "
               "('978-0134685991', 'Effective Java', 'Joshua Bloch', 'Addison-Wesley', 2018, 'Computer Science', 3, 2, 'Main Library - CS Section'), "
               "('978-0135957059', 'The Pragmatic Programmer', 'Andrew Hunt, David Thomas', 'Addison-Wesley', 2019, 'Computer Science', 5, 4, 'Main Library - CS Section'), "
               "('978-1491950358', 'Building Microservices', 'Sam Newman', 'O''Reilly Media', 2021, 'Computer Science', 2, 1, 'Main Library - CS Section'), "
               "('978-0596009205', 'Head First Java', 'Kathy Sierra, Bert Bates', 'O''Reilly Media', 2005, 'Computer Science', 6, 5, 'Main Library - CS Section'), "
               "('978-0132778046', 'Introduction to Algorithms', 'Thomas H. Cormen, Charles E. Leiserson, Ronald L. Rivest, Clifford Stein', 'MIT Press', 2009, 'Computer Science', 4, 3, 'Main Library - CS Section'), "
               "('978-1491929125', 'Python Crash Course', 'Eric Matthes', 'No Starch Press', 2019, 'Computer Science', 5, 4, 'Main Library - CS Section'), "
               "('978-0134092669', 'Database System Concepts', 'Abraham Silberschatz, Henry F. Korth, S. Sudarshan', 'McGraw-Hill', 2019, 'Computer Science', 3, 2, 'Main Library - CS Section'), "
               "('978-1119806916', 'Operating System Concepts', 'Abraham Silberschatz, Peter Baer Galvin, Greg Gagne', 'Wiley', 2021, 'Computer Science', 4, 3, 'Main Library - CS Section'), "
               "('978-0133594140', 'Computer Networks', 'Andrew S. Tanenbaum, David J. Wetherall', 'Prentice Hall', 2013, 'Computer Science', 3, 2, 'Main Library - CS Section')");
}
