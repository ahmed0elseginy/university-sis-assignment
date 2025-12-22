#include "facultyrepository.h"
#include "../../database/databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

FacultyRepository::FacultyRepository() {}

bool FacultyRepository::addFaculty(const Faculty& faculty) {
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare("INSERT INTO faculty (name, email, department, position, username, password) VALUES (:name, :email, :dept, :pos, :user, :pass)");
    query.bindValue(":name", faculty.name);
    query.bindValue(":email", faculty.email);
    query.bindValue(":dept", faculty.department);
    query.bindValue(":pos", faculty.position);
    query.bindValue(":user", faculty.username);
    query.bindValue(":pass", faculty.password);
    
    if (!query.exec()) {
        qDebug() << "Add Faculty Error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool FacultyRepository::updateFaculty(const Faculty& faculty) {
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare("UPDATE faculty SET name=:name, email=:email, department=:dept, position=:pos WHERE faculty_id=:id");
    query.bindValue(":name", faculty.name);
    query.bindValue(":email", faculty.email);
    query.bindValue(":dept", faculty.department);
    query.bindValue(":pos", faculty.position);
    query.bindValue(":id", faculty.id);
    
    if (!query.exec()) {
        qDebug() << "Update Faculty Error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool FacultyRepository::deleteFaculty(int id) {
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare("DELETE FROM faculty WHERE faculty_id = :id");
    query.bindValue(":id", id);
    
    if (!query.exec()) {
        qDebug() << "Delete Faculty Error:" << query.lastError().text();
        return false;
    }
    return true;
}

std::vector<Faculty> FacultyRepository::getAllFaculty() {
    std::vector<Faculty> list;
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query("SELECT faculty_id, name, email, department, position FROM faculty", db);
    
    while (query.next()) {
        Faculty f;
        f.id = query.value(0).toInt();
        f.name = query.value(1).toString();
        f.email = query.value(2).toString();
        f.department = query.value(3).toString();
        f.position = query.value(4).toString();
        list.push_back(f);
    }
    return list;
}

std::optional<Faculty> FacultyRepository::getFacultyById(int id) {
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare("SELECT faculty_id, name, email, department, position FROM faculty WHERE faculty_id = :id");
    query.bindValue(":id", id);
    
    if (query.exec() && query.next()) {
        Faculty f;
        f.id = query.value(0).toInt();
        f.name = query.value(1).toString();
        f.email = query.value(2).toString();
        f.department = query.value(3).toString();
        f.position = query.value(4).toString();
        return f;
    }
    return std::nullopt;
}

std::optional<Faculty> FacultyRepository::authenticate(const QString& username, const QString& password) {
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare("SELECT faculty_id, name, email, department, position, username FROM faculty WHERE username = :u AND password = :p");
    query.bindValue(":u", username);
    query.bindValue(":p", password);
    
    if (query.exec() && query.next()) {
        Faculty f;
        f.id = query.value(0).toInt();
        f.name = query.value(1).toString();
        f.email = query.value(2).toString();
        f.department = query.value(3).toString();
        f.position = query.value(4).toString();
        f.username = query.value(5).toString();
        return f;
    }
    return std::nullopt;
}
