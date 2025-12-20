#ifndef FACULTY_H
#define FACULTY_H

#include <QString>

struct Faculty {
    int id = 0;
    QString name;
    QString email;
    QString department;
    QString position; // e.g. Professor, Lecturer
};

#endif // FACULTY_H
