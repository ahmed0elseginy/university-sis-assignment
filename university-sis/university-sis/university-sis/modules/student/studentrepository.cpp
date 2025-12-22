#include "studentrepository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

StudentRepository::StudentRepository() {
    // Database connection is managed by DatabaseManager globally or default connection
}

bool StudentRepository::addStudent(const Student& student)
{
    QSqlQuery query;
    query.prepare("INSERT INTO students (name, year, department, section_id, username, password) "
                  "VALUES (:name, :year, :dept, :section, :username, :password)");
    
    query.bindValue(":name", student.name);
    query.bindValue(":year", student.year);
    query.bindValue(":dept", student.department);
    query.bindValue(":section", student.sectionId);
    query.bindValue(":username", student.username);
    query.bindValue(":password", student.password);

    if (!query.exec()) {
        qDebug() << "StudentRepository::addStudent error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool StudentRepository::updateStudent(const Student& student)
{
    QSqlQuery query;
    query.prepare("UPDATE students SET name = :name, year = :year, "
                  "department = :dept, section_id = :section WHERE student_id = :id");
    
    query.bindValue(":name", student.name);
    query.bindValue(":year", student.year);
    query.bindValue(":dept", student.department);
    query.bindValue(":section", student.sectionId);
    query.bindValue(":id", student.id);

    if (!query.exec()) {
        qDebug() << "StudentRepository::updateStudent error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool StudentRepository::deleteStudent(int id)
{
    QSqlQuery query;
    // Note: Depends on cascade delete settings or if other tables reference this student
    query.prepare("DELETE FROM students WHERE student_id = :id");
    query.bindValue(":id", id);
    
    if (!query.exec()) {
        qDebug() << "StudentRepository::deleteStudent error:" << query.lastError().text();
        return false;
    }
    return true;
}

std::optional<Student> StudentRepository::getStudentById(int id)
{
    QSqlQuery query;
    query.prepare("SELECT student_id, name, year, department, section_id FROM students WHERE student_id = :id");
    query.bindValue(":id", id);
    
    if (query.exec() && query.next()) {
        Student s;
        s.id = query.value(0).toInt();
        s.name = query.value(1).toString();
        s.year = query.value(2).toInt();
        s.department = query.value(3).toString();
        s.sectionId = query.value(4).toInt();
        return s;
    }
    return std::nullopt;
}

std::vector<Student> StudentRepository::getAllStudents()
{
    std::vector<Student> students;
    QSqlQuery query("SELECT student_id, name, year, department, section_id, username, password FROM students");
    
    while (query.next()) {
        Student s;
        s.id = query.value(0).toInt();
        s.name = query.value(1).toString();
        s.year = query.value(2).toInt();
        s.department = query.value(3).toString();
        s.sectionId = query.value(4).toInt();
        s.username = query.value(5).toString();
        s.password = query.value(6).toString();
        students.push_back(s);
    }
    return students;
}

std::optional<Student> StudentRepository::authenticate(const QString& username, const QString& password)
{
    QSqlQuery query;
    query.prepare("SELECT student_id, name, year, department, section_id FROM students WHERE username = :user AND password = :pass");
    query.bindValue(":user", username);
    query.bindValue(":pass", password);
    
    if (query.exec() && query.next()) {
        Student s;
        s.id = query.value(0).toInt();
        s.name = query.value(1).toString();
        s.year = query.value(2).toInt();
        s.department = query.value(3).toString();
        s.sectionId = query.value(4).toInt();
        s.username = username; 
        return s;
    }
    return std::nullopt;
}

// Removed emailExists since email is no longer in schema
bool StudentRepository::emailExists(const QString& email, int excludeId)
{
    Q_UNUSED(email);
    Q_UNUSED(excludeId);
    return false;
}
