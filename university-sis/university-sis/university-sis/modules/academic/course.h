#ifndef COURSE_H
#define COURSE_H

#include <QString>

struct Course {
    int id = 0;
    QString name;
    int year = 1;
    int hours = 3;
    
    // Additional helpful method
    bool isValid() const { return !name.isEmpty() && hours > 0; }
};

#endif // COURSE_H
