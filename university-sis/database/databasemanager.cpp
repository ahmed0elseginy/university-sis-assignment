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
               "position VARCHAR(50))").arg(autoInc));

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

    // Facility Management
    query.exec(QString("CREATE TABLE IF NOT EXISTS buildings ("
               "id %1, "
               "name VARCHAR(100), "
               "code VARCHAR(20), "
               "location VARCHAR(200))").arg(autoInc));

    query.exec(QString("CREATE TABLE IF NOT EXISTS rooms ("
               "id %1, "
               "building_id INT, "
               "room_number VARCHAR(20), "
               "type VARCHAR(50), "
               "capacity INT, "
               "FOREIGN KEY (building_id) REFERENCES buildings(id) ON DELETE CASCADE)").arg(autoInc));

     // Seed admin user if not exists
     QSqlQuery seed;
     // SQLite uses INSERT OR IGNORE, MySQL uses INSERT IGNORE.
     // Standard SQL: check existence first or catch error. Simplest cross-db approach:
     seed.prepare("INSERT INTO users (user_id, password_hash, role, display_name) VALUES (1, 'admin123', 'ADMIN', 'Administrator')");
     if(!seed.exec()) {
         // Ignore error if already exists (constraint violation)
     }
}
