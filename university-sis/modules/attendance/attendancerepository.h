#ifndef ATTENDANCEREPOSITORY_H
#define ATTENDANCEREPOSITORY_H

#include "attendance.h"
#include <vector>
#include <optional>
#include <QString>
#include <QDate>

class AttendanceRepository {
public:
    AttendanceRepository();
    
    bool addAttendance(const Attendance& att);
    bool addMultipleAttendance(const std::vector<Attendance>& attendanceList);
    bool deleteAttendance(int id);
    
    std::vector<Attendance> getAllAttendance();
    std::vector<Attendance> getAttendanceWithNames();  // Gets attendance with student and course names
    std::vector<Attendance> getFilteredAttendance(
        const QString& studentNameFilter = QString(),
        int courseIdFilter = -1,
        const QString& statusFilter = QString(),
        const QDate& dateFrom = QDate(),
        const QDate& dateTo = QDate()
    );  // Gets filtered attendance with names
};

#endif // ATTENDANCEREPOSITORY_H
