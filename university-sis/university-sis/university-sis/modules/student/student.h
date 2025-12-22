#ifndef STUDENT_H
#define STUDENT_H

#include <QString>

/**
 * @brief The Student struct represents a student entity in the system.
 */
struct Student {
    int id = -1; // student_id
    QString name;
    int year = 1;
    QString department;
    int sectionId = 0;
    QString username;
    QString password;

    bool isValid() const {
        return !name.isEmpty() && !department.isEmpty() && year > 0;
    }
};

#endif // STUDENT_H
