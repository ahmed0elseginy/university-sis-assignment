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
               "section_id INT, "
               "course_id INT, "
               "date DATE, "
               "status VARCHAR(20), " // Present, Absent, Late
               "FOREIGN KEY (student_id) REFERENCES students(student_id), "
               "FOREIGN KEY (section_id) REFERENCES sections(section_id), "
               "FOREIGN KEY (course_id) REFERENCES courses(course_id))").arg(autoInc));
    
    // Migration for Attendance: Add section_id column if missing (SQLite ignores if column exists)
    // Note: Foreign key constraint can only be added in CREATE TABLE, not ALTER TABLE in SQLite
    query.exec("ALTER TABLE attendance ADD COLUMN section_id INT");
               
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
     seed.prepare("INSERT INTO users (user_id, password_hash, role, display_name) VALUES (1, '1234', 'ADMIN', 'المدير')");
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
    
    // Seed Students (Arabic names)
    query.exec("INSERT INTO students (name, year, department, username, password) VALUES "
               "('أحمد محمد علي', 2, 'علوم الحاسب', 'ahmed.mohamed', 'pass123'), "
               "('فاطمة حسن إبراهيم', 3, 'الرياضيات', 'fatima.hassan', 'pass123'), "
               "('محمد خالد أحمد', 1, 'الفيزياء', 'mohamed.khaled', 'pass123'), "
               "('سارة علي محمود', 4, 'علوم الحاسب', 'sara.ali', 'pass123'), "
               "('عمر يوسف عبدالله', 2, 'الهندسة', 'omar.youssef', 'pass123'), "
               "('مريم سالم حسين', 3, 'الأحياء', 'mariam.salem', 'pass123'), "
               "('يوسف أحمد إسماعيل', 1, 'الكيمياء', 'youssef.ahmed', 'pass123'), "
               "('نورا محمد عبدالرحمن', 4, 'الرياضيات', 'nora.mohamed', 'pass123'), "
               "('خالد محمود سعيد', 2, 'علوم الحاسب', 'khaled.mahmoud', 'pass123'), "
               "('ليلى أحمد فؤاد', 3, 'الهندسة', 'layla.ahmed', 'pass123')");
    
    // Seed Faculty (Arabic names with nctu.eg.edu domain)
    query.exec("INSERT INTO faculty (name, email, department, position, username, password) VALUES "
               "('د. محمد أحمد الشريف', 'mohamed.alsharif@nctu.eg.edu', 'علوم الحاسب', 'أستاذ', 'mohamed.sharif', 'pass123'), "
               "('د. سامية محمود حسن', 'samia.hassan@nctu.eg.edu', 'الرياضيات', 'أستاذ مساعد', 'samia.hassan', 'pass123'), "
               "('د. أحمد يوسف إبراهيم', 'ahmed.youssef@nctu.eg.edu', 'الفيزياء', 'أستاذ', 'ahmed.youssef', 'pass123'), "
               "('د. هدى علي محمود', 'huda.ali@nctu.eg.edu', 'الأحياء', 'مدرس مساعد', 'huda.ali', 'pass123'), "
               "('د. خالد محمد عبدالله', 'khaled.abdullah@nctu.eg.edu', 'الهندسة', 'أستاذ', 'khaled.abdullah', 'pass123'), "
               "('د. منى حسن فؤاد', 'mona.fouad@nctu.eg.edu', 'الكيمياء', 'أستاذ مساعد', 'mona.fouad', 'pass123'), "
               "('د. عمرو إسماعيل أحمد', 'amr.ismail@nctu.eg.edu', 'علوم الحاسب', 'أستاذ', 'amr.ismail', 'pass123'), "
               "('د. نورا سعيد محمود', 'nora.saeed@nctu.eg.edu', 'الرياضيات', 'مدرس مساعد', 'nora.saeed', 'pass123')");
    
    // Seed Courses (Arabic course names)
    query.exec("INSERT INTO courses (name, year, hours) VALUES "
               "('مقدمة في البرمجة', 1, 3), "
               "('هياكل البيانات', 2, 4), "
               "('أنظمة قواعد البيانات', 3, 3), "
               "('هندسة البرمجيات', 4, 4), "
               "('حساب التفاضل والتكامل الأول', 1, 4), "
               "('الجبر الخطي', 2, 3), "
               "('الفيزياء العامة الأولى', 1, 4), "
               "('أساسيات الكيمياء', 1, 3), "
               "('الأحياء العامة 101', 1, 3), "
               "('الخوارزميات المتقدمة', 3, 4), "
               "('تعلم الآلة', 4, 3), "
               "('شبكات الحاسب', 3, 3), "
               "('نظم التشغيل', 3, 4), "
               "('تطوير الويب', 2, 3), "
               "('تطوير تطبيقات الجوال', 3, 3), "
               "('الأمن السيبراني', 4, 3), "
               "('الذكاء الاصطناعي', 4, 4), "
               "('النظم الموزعة', 4, 3), "
               "('رسوميات الحاسب', 3, 3), "
               "('معالجة الإشارات الرقمية', 4, 3), "
               "('النظم المدمجة', 3, 4), "
               "('الحوسبة السحابية', 4, 3), "
               "('تحليل البيانات الضخمة', 4, 3), "
               "('الحوسبة الكمية', 4, 3)");
    
    // Seed Sections
    query.exec("INSERT INTO sections (course_id, max_students) VALUES "
               "(1, 50), (1, 50), (2, 40), (2, 40), (3, 35), "
               "(4, 30), (5, 60), (5, 60), (6, 45), (7, 55), "
               "(8, 40), (9, 40), (10, 35), (11, 30), (12, 35), "
               "(13, 40), (14, 40), (15, 35), (16, 30), (17, 30), "
               "(18, 35), (19, 35), (20, 40), (21, 30), (22, 30), (23, 25)");
    
    // Seed Student-Section enrollments
    query.exec("INSERT INTO student_section (student_id, section_id) VALUES "
               "(1, 1), (1, 3), (1, 14), (2, 5), (2, 9), (2, 11), "
               "(3, 7), (3, 11), (4, 1), (4, 3), (4, 6), (4, 13), "
               "(5, 1), (5, 8), (5, 14), (6, 11), (6, 12), (7, 7), "
               "(7, 8), (8, 5), (8, 9), (9, 1), (9, 3), (9, 14), "
               "(10, 1), (10, 8), (10, 14)");
    
    // Seed Attendance
    query.exec("INSERT INTO attendance (student_id, section_id, course_id, date, status) VALUES "
               "(1, 1, 1, '2024-09-15', 'Present'), (1, 1, 1, '2024-09-17', 'Present'), "
               "(1, 1, 1, '2024-09-19', 'Late'), (1, 1, 1, '2024-09-22', 'Present'), "
               "(2, 5, 3, '2024-09-16', 'Present'), (2, 5, 3, '2024-09-18', 'Absent'), "
               "(2, 5, 3, '2024-09-20', 'Present'), (3, 7, 7, '2024-09-15', 'Present'), "
               "(3, 7, 7, '2024-09-17', 'Present'), (4, 1, 1, '2024-09-15', 'Present'), "
               "(4, 1, 1, '2024-09-17', 'Present'), (5, 1, 1, '2024-09-15', 'Present'), "
               "(5, 1, 1, '2024-09-17', 'Late'), (6, 11, 9, '2024-09-16', 'Present'), "
               "(7, 7, 7, '2024-09-15', 'Absent'), (8, 5, 3, '2024-09-16', 'Present'), "
               "(9, 1, 1, '2024-09-15', 'Present'), (10, 1, 1, '2024-09-15', 'Present')");
    
    // Seed Grades
    query.exec("INSERT INTO grades (student_id, course_id, a1, a2, final_exam, total) VALUES "
               "(1, 1, 'A', 'B+', 'A', 'A'), (1, 2, 'B', 'B+', 'A-', 'B+'), "
               "(1, 14, 'A-', 'A', 'A', 'A'), (2, 3, 'B+', 'B', 'A', 'B+'), "
               "(2, 6, 'A', 'A-', 'A', 'A'), (2, 9, 'B', 'B+', 'B+', 'B+'), "
               "(3, 7, 'A-', 'A', 'A-', 'A-'), (3, 8, 'B+', 'B', 'B+', 'B+'), "
               "(4, 1, 'A', 'A', 'A', 'A'), (4, 2, 'A-', 'A', 'A-', 'A-'), "
               "(4, 4, 'A', 'A-', 'A', 'A'), (4, 10, 'B+', 'A-', 'A-', 'A-'), "
               "(5, 1, 'B+', 'A-', 'B+', 'A-'), (5, 5, 'A', 'A', 'A', 'A'), "
               "(5, 14, 'B', 'B+', 'A-', 'B+'), (6, 9, 'A-', 'A', 'A', 'A'), "
               "(6, 12, 'B+', 'B+', 'A-', 'B+'), (7, 7, 'B', 'B+', 'B', 'B+'), "
               "(7, 8, 'B+', 'B', 'B+', 'B+'), (8, 3, 'A', 'A-', 'A', 'A'), "
               "(8, 6, 'A', 'A', 'A', 'A'), (9, 1, 'A-', 'A', 'A-', 'A'), "
               "(9, 2, 'B+', 'A-', 'A-', 'A-'), (9, 14, 'A', 'A-', 'A', 'A'), "
               "(10, 1, 'B+', 'A-', 'B+', 'A-'), (10, 5, 'A', 'A-', 'A', 'A'), "
               "(10, 14, 'B+', 'B+', 'A-', 'B+')");
    
    // Seed Buildings (Arabic names)
    query.exec("INSERT INTO buildings (name, code, location) VALUES "
               "('مبنى العلوم', 'SCI', 'الحرم الشمالي'), "
               "('قاعة الهندسة', 'ENG', 'الحرم الشرقي'), "
               "('مبنى المكتبة', 'LIB', 'الحرم المركزي'), "
               "('مركز الفنون', 'ART', 'الحرم الجنوبي'), "
               "('مركز الطلاب', 'STU', 'الحرم المركزي'), "
               "('مبنى علوم الحاسب', 'CS', 'الحرم الشمالي'), "
               "('مبنى الرياضيات', 'MATH', 'الحرم الشرقي'), "
               "('مختبر الأحياء', 'BIO', 'الحرم الشمالي'), "
               "('مختبر الكيمياء', 'CHEM', 'الحرم الشمالي'), "
               "('مختبر الفيزياء', 'PHYS', 'الحرم الشمالي'), "
               "('مختبر الهندسة', 'ENGLAB', 'الحرم الشرقي'), "
               "('مركز الأبحاث', 'RES', 'الحرم الجنوبي')");
    
    // Seed Rooms (Arabic types)
    query.exec("INSERT INTO rooms (building_id, room_number, type, capacity) VALUES "
               "(1, '101', 'قاعة محاضرات', 100), (1, '102', 'قاعة محاضرات', 80), (1, '201', 'مختبر', 30), "
               "(2, '101', 'قاعة محاضرات', 120), (2, '202', 'مختبر', 25), (2, '301', 'قاعة محاضرات', 90), "
               "(3, '101', 'غرفة دراسة', 20), (3, '201', 'قاعة قراءة', 50), (3, '301', 'قاعة اجتماعات', 15), "
               "(4, '101', 'استوديو', 25), (4, '201', 'ورشة عمل', 30), "
               "(5, '101', 'قاعة اجتماعات', 50), (5, '201', 'قاعة فعاليات', 200), "
               "(6, '101', 'مختبر حاسب', 40), (6, '201', 'قاعة محاضرات', 100), "
               "(7, '101', 'قاعة محاضرات', 80), (7, '201', 'قاعة ندوات', 30), "
               "(8, '101', 'مختبر', 20), (8, '201', 'مختبر', 20), "
               "(9, '101', 'مختبر', 25), (9, '201', 'مختبر', 25), "
               "(10, '101', 'مختبر', 30), (10, '201', 'مختبر', 30), "
               "(11, '101', 'ورشة عمل', 35), (11, '201', 'مختبر', 30), "
               "(12, '101', 'قاعة اجتماعات', 20), (12, '201', 'مختبر أبحاث', 15)");
    
    // Seed Payments (Arabic descriptions)
    query.exec("INSERT INTO payments (student_id, amount, description, status, date) VALUES "
               "(1, 5000.00, 'رسوم دراسية - الفصل الدراسي الأول 2024', 'Paid', '2024-09-01'), "
               "(2, 5000.00, 'رسوم دراسية - الفصل الدراسي الأول 2024', 'Paid', '2024-09-01'), "
               "(3, 5000.00, 'رسوم دراسية - الفصل الدراسي الأول 2024', 'Paid', '2024-09-02'), "
               "(4, 5000.00, 'رسوم دراسية - الفصل الدراسي الأول 2024', 'Paid', '2024-09-02'), "
               "(5, 5000.00, 'رسوم دراسية - الفصل الدراسي الأول 2024', 'Paid', '2024-09-03'), "
               "(6, 5000.00, 'رسوم دراسية - الفصل الدراسي الأول 2024', 'Paid', '2024-09-03'), "
               "(7, 5000.00, 'رسوم دراسية - الفصل الدراسي الأول 2024', 'Paid', '2024-09-04'), "
               "(8, 5000.00, 'رسوم دراسية - الفصل الدراسي الأول 2024', 'Paid', '2024-09-04'), "
               "(9, 5000.00, 'رسوم دراسية - الفصل الدراسي الأول 2024', 'Paid', '2024-09-05'), "
               "(10, 5000.00, 'رسوم دراسية - الفصل الدراسي الأول 2024', 'Paid', '2024-09-05'), "
               "(1, 500.00, 'رسوم المختبر', 'Paid', '2024-09-15'), "
               "(2, 500.00, 'رسوم المختبر', 'Paid', '2024-09-15'), "
               "(3, 500.00, 'رسوم المختبر', 'Paid', '2024-09-16'), "
               "(4, 500.00, 'رسوم المختبر', 'Paid', '2024-09-16'), "
               "(5, 500.00, 'رسوم المختبر', 'Paid', '2024-09-17'), "
               "(6, 500.00, 'رسوم المختبر', 'Paid', '2024-09-17'), "
               "(1, 300.00, 'رسوم المكتبة', 'Paid', '2024-09-20'), "
               "(2, 300.00, 'رسوم المكتبة', 'Paid', '2024-09-20'), "
               "(3, 300.00, 'رسوم المكتبة', 'Pending', '2024-09-25'), "
               "(4, 300.00, 'رسوم المكتبة', 'Pending', '2024-09-25')");
    
    // Seed Announcements (Arabic)
    query.exec("INSERT INTO announcements (title, content, date, target_role) VALUES "
               "('بدء الفصل الدراسي الأول', 'نود إعلامكم بأن الفصل الدراسي الأول يبدأ يوم الأحد الموافق 15 سبتمبر 2024. نتمنى لجميع الطلاب عاماً دراسياً موفقاً.', '2024-09-10 10:00:00', 'All'), "
               "('جدول الامتحانات النهائية', 'تم نشر جدول الامتحانات النهائية للفصل الدراسي الأول. يرجى مراجعة الموقع الرسمي للجامعة.', '2024-09-20 14:30:00', 'Student'), "
               "('اجتماع أعضاء هيئة التدريس', 'سيتم عقد اجتماع لأعضاء هيئة التدريس يوم الخميس الموافق 25 سبتمبر في قاعة الاجتماعات الرئيسية.', '2024-09-22 09:00:00', 'Faculty'), "
               "('ورشة عمل عن الذكاء الاصطناعي', 'تدعو الجامعة جميع الطلاب لحضور ورشة عمل عن الذكاء الاصطناعي وتطبيقاته يوم السبت الموافق 5 أكتوبر.', '2024-09-28 16:00:00', 'Student'), "
               "('تحديثات المكتبة', 'تم تحديث مجموعة الكتب في المكتبة. يمكن للطلاب استعارة الكتب الجديدة اعتباراً من اليوم.', '2024-09-30 11:00:00', 'All'), "
               "('إعلان مهم: رسوم الفصل الدراسي', 'نذكر جميع الطلاب بضرورة سداد رسوم الفصل الدراسي قبل نهاية الشهر الجاري لتجنب أي تأخير.', '2024-10-01 10:00:00', 'Student'), "
               "('مؤتمر البحث العلمي', 'سيتم تنظيم مؤتمر للبحث العلمي في ديسمبر 2024. نرحب بمشاركة أعضاء هيئة التدريس والطلاب.', '2024-10-05 12:00:00', 'All')");
    
    // Seed Calendar Events (Arabic)
    query.exec("INSERT INTO calendar_events (date, time, title, type, description) VALUES "
               "('2024-09-15', '08:00:00', 'بدء الفصل الدراسي الأول', 'Class', 'أول يوم في الفصل الدراسي الأول'), "
               "('2024-09-20', '10:00:00', 'اختبار قصير - مقدمة في البرمجة', 'Exam', 'اختبار قصير في مقرر مقدمة في البرمجة'), "
               "('2024-09-25', '14:00:00', 'اجتماع أعضاء هيئة التدريس', 'Meeting', 'اجتماع دوري لأعضاء هيئة التدريس'), "
               "('2024-10-06', '00:00:00', 'عيد الأضحى', 'Holiday', 'عطلة رسمية بمناسبة عيد الأضحى المبارك'), "
               "('2024-10-07', '00:00:00', 'عيد الأضحى', 'Holiday', 'عطلة رسمية بمناسبة عيد الأضحى المبارك'), "
               "('2024-10-15', '23:59:00', 'آخر موعد لتسليم الواجب الأول', 'Assignment Due', 'آخر موعد لتسليم الواجب الأول في مقرر هياكل البيانات'), "
               "('2024-10-20', '10:00:00', 'اختبار - أنظمة قواعد البيانات', 'Exam', 'اختبار في مقرر أنظمة قواعد البيانات'), "
               "('2024-11-10', '23:59:00', 'آخر موعد لتسليم مشروع هندسة البرمجيات', 'Assignment Due', 'آخر موعد لتسليم مشروع مقرر هندسة البرمجيات'), "
               "('2024-11-20', '00:00:00', 'عيد الفطر', 'Holiday', 'عطلة رسمية بمناسبة عيد الفطر'), "
               "('2024-12-01', '08:00:00', 'بدء فترة الامتحانات النهائية', 'Exam', 'بدء فترة الامتحانات النهائية للفصل الدراسي الأول'), "
               "('2024-12-20', '17:00:00', 'نهاية الفصل الدراسي الأول', 'Other', 'نهاية الفصل الدراسي الأول وبدء الإجازة'), "
               "('2024-12-25', '00:00:00', 'رأس السنة الميلادية', 'Holiday', 'عطلة رسمية بمناسبة رأس السنة الميلادية')");
    
    // Seed Library Books (Arabic titles and authors)
    query.exec("INSERT INTO books (isbn, title, author, publisher, publication_year, category, total_copies, available_copies, location) VALUES "
               "('978-0131103627', 'لغة البرمجة C++', 'بيارن ستروستروب', 'أديسون ويسلي', 2013, 'علوم الحاسب', 5, 3, 'المكتبة الرئيسية - قسم علوم الحاسب'), "
               "('978-0201633610', 'أنماط التصميم: عناصر البرمجة الكائنية القابلة لإعادة الاستخدام', 'إريك جاما، ريتشارد هيلم، رالف جونسون، جون فليسايدز', 'أديسون ويسلي', 1994, 'علوم الحاسب', 3, 1, 'المكتبة الرئيسية - قسم علوم الحاسب'), "
               "('978-0132350884', 'الكود النظيف: دليل البرمجة المرنة', 'روبرت سي مارتن', 'برنتيس هول', 2008, 'علوم الحاسب', 4, 4, 'المكتبة الرئيسية - قسم علوم الحاسب'), "
               "('978-0321751041', 'فن البرمجة', 'دونالد كنوث', 'أديسون ويسلي', 2011, 'علوم الحاسب', 2, 2, 'المكتبة الرئيسية - مرجع'), "
               "('978-1449331818', 'تعلم React: أنماط حديثة لتطوير تطبيقات React', 'أليكس بانكس، إيف بورسيلو', 'أورايلي ميديا', 2020, 'علوم الحاسب', 3, 2, 'المكتبة الرئيسية - قسم علوم الحاسب'), "
               "('978-0596007126', 'أنماط التصميم للعقول المتقدمة', 'إريك فريمان، إليزابيث روبسون', 'أورايلي ميديا', 2004, 'علوم الحاسب', 4, 3, 'المكتبة الرئيسية - قسم علوم الحاسب'), "
               "('978-0134685991', 'Java الفعال', 'جوشوا بلوخ', 'أديسون ويسلي', 2018, 'علوم الحاسب', 3, 2, 'المكتبة الرئيسية - قسم علوم الحاسب'), "
               "('978-0135957059', 'المبرمج العملي', 'أندرو هنت، ديفيد توماس', 'أديسون ويسلي', 2019, 'علوم الحاسب', 5, 4, 'المكتبة الرئيسية - قسم علوم الحاسب'), "
               "('978-1491950358', 'بناء الخدمات الصغيرة', 'سام نيومان', 'أورايلي ميديا', 2021, 'علوم الحاسب', 2, 1, 'المكتبة الرئيسية - قسم علوم الحاسب'), "
               "('978-0596009205', 'Java للعقول المتقدمة', 'كاثي سييرا، بيرت بيتس', 'أورايلي ميديا', 2005, 'علوم الحاسب', 6, 5, 'المكتبة الرئيسية - قسم علوم الحاسب'), "
               "('978-0132778046', 'مقدمة في الخوارزميات', 'توماس إتش كورمن، تشارلز إي لايسرسون، رونالد إل ريفيست، كليفورد شتاين', 'مطبعة MIT', 2009, 'علوم الحاسب', 4, 3, 'المكتبة الرئيسية - قسم علوم الحاسب'), "
               "('978-1491929125', 'دورة تعلم Python السريعة', 'إريك ماثيس', 'نو ستارش برس', 2019, 'علوم الحاسب', 5, 4, 'المكتبة الرئيسية - قسم علوم الحاسب'), "
               "('978-0134092669', 'مفاهيم أنظمة قواعد البيانات', 'أبراهام سيلبرشاتز، هنري إف كورث، س. سودارشان', 'ماغراو هيل', 2019, 'علوم الحاسب', 3, 2, 'المكتبة الرئيسية - قسم علوم الحاسب'), "
               "('978-1119806916', 'مفاهيم نظم التشغيل', 'أبراهام سيلبرشاتز، بيتر بير جالفين، جريج جاجن', 'وايلي', 2021, 'علوم الحاسب', 4, 3, 'المكتبة الرئيسية - قسم علوم الحاسب'), "
               "('978-0133594140', 'شبكات الحاسب', 'أندرو إس تانينباوم، ديفيد جي ويثيرال', 'برنتيس هول', 2013, 'علوم الحاسب', 3, 2, 'المكتبة الرئيسية - قسم علوم الحاسب')");
}
