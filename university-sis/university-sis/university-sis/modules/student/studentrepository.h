#ifndef STUDENTREPOSITORY_H
#define STUDENTREPOSITORY_H

#include "student.h"
#include <vector>
#include <optional>
#include <QVariant>

/**
 * @brief The StudentRepository class handles database operations for the Student module.
 */
class StudentRepository {
public:
    StudentRepository();

    bool addStudent(const Student& student);
    bool updateStudent(const Student& student);
    bool deleteStudent(int id);
    std::optional<Student> getStudentById(int id);
    std::vector<Student> getAllStudents();
    std::optional<Student> authenticate(const QString& username, const QString& password);
    bool emailExists(const QString& email, int excludeId = -1);
};

#endif // STUDENTREPOSITORY_H
