#include "sectionrepository.h"
#include "../../database/databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

SectionRepository::SectionRepository() {}

bool SectionRepository::addSection(const Section& section) {
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare("INSERT INTO sections (course_id, max_students) VALUES (:cid, :max)");
    query.bindValue(":cid", section.courseId);
    query.bindValue(":max", section.maxStudents);
    
    if (!query.exec()) {
        qDebug() << "Add Section Error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool SectionRepository::updateSection(const Section& section) {
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare("UPDATE sections SET course_id=:cid, max_students=:max WHERE section_id=:id");
    query.bindValue(":cid", section.courseId);
    query.bindValue(":max", section.maxStudents);
    query.bindValue(":id", section.id);
    
    if (!query.exec()) {
        qDebug() << "Update Section Error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool SectionRepository::deleteSection(int id) {
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare("DELETE FROM sections WHERE section_id = :id");
    query.bindValue(":id", id);
    
    if (!query.exec()) {
        qDebug() << "Delete Section Error:" << query.lastError().text();
        return false;
    }
    return true;
}

std::vector<Section> SectionRepository::getAllSections() {
    std::vector<Section> list;
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query("SELECT section_id, course_id, max_students FROM sections", db);
    
    while (query.next()) {
        Section s;
        s.id = query.value(0).toInt();
        s.courseId = query.value(1).toInt();
        s.maxStudents = query.value(2).toInt();
        list.push_back(s);
    }
    return list;
}

std::optional<Section> SectionRepository::getSectionById(int id) {
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare("SELECT section_id, course_id, max_students FROM sections WHERE section_id = :id");
    query.bindValue(":id", id);
    
    if (query.exec() && query.next()) {
        Section s;
        s.id = query.value(0).toInt();
        s.courseId = query.value(1).toInt();
        s.maxStudents = query.value(2).toInt();
        return s;
    }
    return std::nullopt;
}
