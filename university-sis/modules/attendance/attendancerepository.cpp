#include "attendancerepository.h"
#include "../../database/databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

AttendanceRepository::AttendanceRepository() {}

bool AttendanceRepository::addAttendance(const Attendance& att) {
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare("INSERT INTO attendance (student_id, course_id, date, status) VALUES (:sid, :cid, :date, :stat)");
    query.bindValue(":sid", att.studentId);
    query.bindValue(":cid", att.courseId);
    query.bindValue(":date", att.date);
    query.bindValue(":stat", att.status);
    
    if (!query.exec()) {
        qDebug() << "Add Attendance Error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool AttendanceRepository::deleteAttendance(int id) {
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare("DELETE FROM attendance WHERE attendance_id = :id");
    query.bindValue(":id", id);
    
    if (!query.exec()) {
        qDebug() << "Delete Attendance Error:" << query.lastError().text();
        return false;
    }
    return true;
}

std::vector<Attendance> AttendanceRepository::getAllAttendance() {
    std::vector<Attendance> list;
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query("SELECT attendance_id, student_id, course_id, date, status FROM attendance", db);
    
    while (query.next()) {
        Attendance a;
        a.id = query.value(0).toInt();
        a.studentId = query.value(1).toInt();
        a.courseId = query.value(2).toInt();
        a.date = query.value(3).toDate();
        a.status = query.value(4).toString();
        list.push_back(a);
    }
    return list;
}
