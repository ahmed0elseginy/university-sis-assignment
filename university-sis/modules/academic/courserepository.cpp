#include "courserepository.h"
#include "../../database/databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

CourseRepository::CourseRepository() {}

bool CourseRepository::addCourse(const Course& course) {
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare("INSERT INTO courses (name, year, hours) VALUES (:name, :year, :hours)");
    query.bindValue(":name", course.name);
    query.bindValue(":year", course.year);
    query.bindValue(":hours", course.hours);
    
    if (!query.exec()) {
        qDebug() << "Add Course Error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool CourseRepository::updateCourse(const Course& course) {
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare("UPDATE courses SET name=:name, year=:year, hours=:hours WHERE course_id=:id");
    query.bindValue(":name", course.name);
    query.bindValue(":year", course.year);
    query.bindValue(":hours", course.hours);
    query.bindValue(":id", course.id);
    
    if (!query.exec()) {
        qDebug() << "Update Course Error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool CourseRepository::deleteCourse(int id) {
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare("DELETE FROM courses WHERE course_id = :id");
    query.bindValue(":id", id);
    
    if (!query.exec()) {
        qDebug() << "Delete Course Error:" << query.lastError().text();
        return false;
    }
    return true;
}

std::vector<Course> CourseRepository::getAllCourses() {
    std::vector<Course> courses;
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query("SELECT course_id, name, year, hours FROM courses", db);
    
    while (query.next()) {
        Course c;
        c.id = query.value(0).toInt();
        c.name = query.value(1).toString();
        c.year = query.value(2).toInt();
        c.hours = query.value(3).toInt();
        courses.push_back(c);
    }
    return courses;
}

std::optional<Course> CourseRepository::getCourseById(int id) {
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare("SELECT course_id, name, year, hours FROM courses WHERE course_id = :id");
    query.bindValue(":id", id);
    
    if (query.exec() && query.next()) {
        Course c;
        c.id = query.value(0).toInt();
        c.name = query.value(1).toString();
        c.year = query.value(2).toInt();
        c.hours = query.value(3).toInt();
        return c;
    }
    return std::nullopt;
}
