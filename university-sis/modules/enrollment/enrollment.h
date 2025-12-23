#ifndef ENROLLMENT_H
#define ENROLLMENT_H

#include <QString>

struct Enrollment {
    int studentId = 0;
    QString studentName;  // Student name for display
    int sectionId = 0;
};

#endif // ENROLLMENT_H
