#ifndef ATTENDANCE_H
#define ATTENDANCE_H

#include <QString>
#include <QDate>

struct Attendance {
    int id = 0;
    int studentId = 0;
    QString studentName;  // Student name for display
    int sectionId = 0;  // Section ID for the attendance record
    int courseId = 0;   // Course ID (derived from section, kept for convenience)
    QString courseName;  // Course name for display
    QDate date = QDate::currentDate();
    QString status = "Present"; // Present, Absent, Late
};

#endif // ATTENDANCE_H
