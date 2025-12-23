#include "attendancerepository.h"
#include "../../database/databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

AttendanceRepository::AttendanceRepository() {}

bool AttendanceRepository::addAttendance(const Attendance& att) {
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare("INSERT INTO attendance (student_id, section_id, course_id, date, status) VALUES (:sid, :secid, :cid, :date, :stat)");
    query.bindValue(":sid", att.studentId);
    query.bindValue(":secid", att.sectionId);
    query.bindValue(":cid", att.courseId);
    query.bindValue(":date", att.date);
    query.bindValue(":stat", att.status);
    
    if (!query.exec()) {
        qDebug() << "Add Attendance Error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool AttendanceRepository::addMultipleAttendance(const std::vector<Attendance>& attendanceList) {
    if (attendanceList.empty()) {
        return true; // Nothing to do
    }
    
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    bool isSqlite = (db.driverName() == "QSQLITE");
    db.transaction(); // Start transaction for atomicity
    
    QSqlQuery query(db);
    
    // Use INSERT OR REPLACE for SQLite, regular INSERT for others
    // First delete existing records for the same student/section/date to avoid duplicates
    QSqlQuery deleteQuery(db);
    deleteQuery.prepare("DELETE FROM attendance WHERE student_id = :sid AND section_id = :secid AND date = :date");
    
    for (const auto& att : attendanceList) {
        // Delete existing record for this student/section/date combination
        deleteQuery.bindValue(":sid", att.studentId);
        deleteQuery.bindValue(":secid", att.sectionId);
        deleteQuery.bindValue(":date", att.date);
        deleteQuery.exec(); // Ignore errors if no record exists
        
        // Insert new record
        query.prepare("INSERT INTO attendance (student_id, section_id, course_id, date, status) VALUES (:sid, :secid, :cid, :date, :stat)");
        query.bindValue(":sid", att.studentId);
        query.bindValue(":secid", att.sectionId);
        query.bindValue(":cid", att.courseId);
        query.bindValue(":date", att.date);
        query.bindValue(":stat", att.status);
        
        if (!query.exec()) {
            qDebug() << "Add Multiple Attendance Error:" << query.lastError().text();
            db.rollback();
            return false;
        }
    }
    
    db.commit(); // Commit transaction if all inserts succeeded
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
    QSqlQuery query("SELECT attendance_id, student_id, section_id, course_id, date, status FROM attendance", db);
    
    while (query.next()) {
        Attendance a;
        a.id = query.value(0).toInt();
        a.studentId = query.value(1).toInt();
        a.sectionId = query.value(2).toInt();
        a.courseId = query.value(3).toInt();
        a.date = query.value(4).toDate();
        a.status = query.value(5).toString();
        list.push_back(a);
    }
    return list;
}

std::vector<Attendance> AttendanceRepository::getAttendanceWithNames() {
    std::vector<Attendance> list;
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    
    query.prepare("SELECT a.attendance_id, a.student_id, s.name as student_name, "
                  "a.section_id, a.course_id, c.name as course_name, "
                  "a.date, a.status "
                  "FROM attendance a "
                  "LEFT JOIN students s ON a.student_id = s.student_id "
                  "LEFT JOIN courses c ON a.course_id = c.course_id "
                  "ORDER BY a.date DESC, s.name");
    
    if (!query.exec()) {
        qDebug() << "Get Attendance With Names Error:" << query.lastError().text();
        return list;
    }
    
    while (query.next()) {
        Attendance a;
        a.id = query.value(0).toInt();
        a.studentId = query.value(1).toInt();
        a.studentName = query.value(2).toString();
        a.sectionId = query.value(3).toInt();
        a.courseId = query.value(4).toInt();
        a.courseName = query.value(5).toString();
        a.date = query.value(6).toDate();
        a.status = query.value(7).toString();
        list.push_back(a);
    }
    return list;
}

std::vector<Attendance> AttendanceRepository::getFilteredAttendance(
    const QString& studentNameFilter,
    int courseIdFilter,
    const QString& statusFilter,
    const QDate& dateFrom,
    const QDate& dateTo) {
    
    std::vector<Attendance> list;
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    
    QString sql = "SELECT a.attendance_id, a.student_id, s.name as student_name, "
                  "a.section_id, a.course_id, c.name as course_name, "
                  "a.date, a.status "
                  "FROM attendance a "
                  "LEFT JOIN students s ON a.student_id = s.student_id "
                  "LEFT JOIN courses c ON a.course_id = c.course_id "
                  "WHERE 1=1";
    
    if (!studentNameFilter.isEmpty()) {
        sql += " AND s.name LIKE :studentName";
    }
    if (courseIdFilter > 0) {
        sql += " AND a.course_id = :courseId";
    }
    if (!statusFilter.isEmpty()) {
        sql += " AND a.status = :status";
    }
    if (dateFrom.isValid()) {
        sql += " AND a.date >= :dateFrom";
    }
    if (dateTo.isValid()) {
        sql += " AND a.date <= :dateTo";
    }
    
    sql += " ORDER BY a.date DESC, s.name";
    
    query.prepare(sql);
    
    if (!studentNameFilter.isEmpty()) {
        query.bindValue(":studentName", "%" + studentNameFilter + "%");
    }
    if (courseIdFilter > 0) {
        query.bindValue(":courseId", courseIdFilter);
    }
    if (!statusFilter.isEmpty()) {
        query.bindValue(":status", statusFilter);
    }
    if (dateFrom.isValid()) {
        query.bindValue(":dateFrom", dateFrom);
    }
    if (dateTo.isValid()) {
        query.bindValue(":dateTo", dateTo);
    }
    
    if (!query.exec()) {
        qDebug() << "Get Filtered Attendance Error:" << query.lastError().text();
        return list;
    }
    
    while (query.next()) {
        Attendance a;
        a.id = query.value(0).toInt();
        a.studentId = query.value(1).toInt();
        a.studentName = query.value(2).toString();
        a.sectionId = query.value(3).toInt();
        a.courseId = query.value(4).toInt();
        a.courseName = query.value(5).toString();
        a.date = query.value(6).toDate();
        a.status = query.value(7).toString();
        list.push_back(a);
    }
    return list;
}