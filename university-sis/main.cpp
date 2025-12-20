#include "mainwindow.h"
#include "database/databasemanager.h"
#include "modules/student/studentrepository.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QDebug>
#include <QMessageBox>

void runDatabaseSelfTest() {
    qDebug() << "=== Running Database Self-Test ===";
    
    // 1. Connection Test
    if (!DatabaseManager::instance().connect()) {
        qCritical() << "FAIL: Could not connect to database.";
        return;
    }
    qDebug() << "PASS: Database connected.";
    qDebug() << "Driver:" << DatabaseManager::instance().getDatabase().driverName();

    // 2. Write Test (Add Student)
    StudentRepository repo;
    Student testStudent;
    testStudent.name = "Test User";
    testStudent.year = 1;
    testStudent.department = "CS_TEST";
    testStudent.sectionId = 101;
    
    if (repo.addStudent(testStudent)) {
        qDebug() << "PASS: Successfully added test student.";
    } else {
        qCritical() << "FAIL: Failed to add test student.";
    }

    // 3. Read Test
    auto students = repo.getAllStudents();
    bool found = false;
    int testId = -1;
    for (const auto& s : students) {
        if (s.name == "Test User" && s.department == "CS_TEST") {
            found = true;
            testId = s.id;
            break;
        }
    }

    if (found) {
        qDebug() << "PASS: Successfully retrieved test student. ID:" << testId;
        
        // 4. Delete Test (Cleanup)
        if (repo.deleteStudent(testId)) {
            qDebug() << "PASS: Cleanup (delete) successful.";
        } else {
            qWarning() << "WARN: Cleanup failed.";
        }
    } else {
        qCritical() << "FAIL: Could not find the added test student.";
    }
    
    qDebug() << "=== Self-Test Complete ===";
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "university-sis_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    // Run the DB check
    runDatabaseSelfTest();

    MainWindow w;
    w.show();
    return a.exec();
}
