#ifndef ATTENDANCE_H
#define ATTENDANCE_H

#include <QString>
#include <QDate>

struct Attendance {
    int id = 0;
    int studentId = 0;
    int courseId = 0;
    QDate date = QDate::currentDate();
    QString status = "Present"; // Present, Absent, Late
};

#endif // ATTENDANCE_H
