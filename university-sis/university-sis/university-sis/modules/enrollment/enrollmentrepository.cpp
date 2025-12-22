#include "enrollmentrepository.h"
#include "../../database/databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

EnrollmentRepository::EnrollmentRepository() {}

bool EnrollmentRepository::enrollStudent(const Enrollment& enrollment) {
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare("INSERT INTO student_section (student_id, section_id) VALUES (:sid, :secid)");
    query.bindValue(":sid", enrollment.studentId);
    query.bindValue(":secid", enrollment.sectionId);
    
    if (!query.exec()) {
        qDebug() << "Enroll Error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool EnrollmentRepository::unenrollStudent(int studentId, int sectionId) {
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare("DELETE FROM student_section WHERE student_id = :sid AND section_id = :secid");
    query.bindValue(":sid", studentId);
    query.bindValue(":secid", sectionId);
    
    if (!query.exec()) {
        qDebug() << "Unenroll Error:" << query.lastError().text();
        return false;
    }
    return true;
}

std::vector<Enrollment> EnrollmentRepository::getAllEnrollments() {
    std::vector<Enrollment> list;
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query("SELECT student_id, section_id FROM student_section", db);
    
    while (query.next()) {
        Enrollment e;
        e.studentId = query.value(0).toInt();
        e.sectionId = query.value(1).toInt();
        list.push_back(e);
    }
    return list;
}
