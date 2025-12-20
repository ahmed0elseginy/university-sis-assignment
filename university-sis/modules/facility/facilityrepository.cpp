#include "facilityrepository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

FacilityRepository::FacilityRepository() {
}

QList<Building> FacilityRepository::getAllBuildings() {
    QList<Building> list;
    QSqlQuery query("SELECT id, name, code, location FROM buildings");
    while (query.next()) {
        Building b;
        b.id = query.value(0).toInt();
        b.name = query.value(1).toString();
        b.code = query.value(2).toString();
        b.location = query.value(3).toString();
        list.append(b);
    }
    return list;
}

Building FacilityRepository::getBuildingById(int id) {
    QSqlQuery query;
    query.prepare("SELECT id, name, code, location FROM buildings WHERE id = :id");
    query.bindValue(":id", id);
    if (query.exec() && query.next()) {
        Building b;
        b.id = query.value(0).toInt();
        b.name = query.value(1).toString();
        b.code = query.value(2).toString();
        b.location = query.value(3).toString();
        return b;
    }
    return Building();
}

bool FacilityRepository::addBuilding(const Building& building) {
    QSqlQuery query;
    query.prepare("INSERT INTO buildings (name, code, location) VALUES (:name, :code, :loc)");
    query.bindValue(":name", building.name);
    query.bindValue(":code", building.code);
    query.bindValue(":loc", building.location);
    
    if (!query.exec()) {
        qDebug() << "FacilityRepository::addBuilding error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool FacilityRepository::updateBuilding(const Building& building) {
    QSqlQuery query;
    query.prepare("UPDATE buildings SET name = :name, code = :code, location = :loc WHERE id = :id");
    query.bindValue(":name", building.name);
    query.bindValue(":code", building.code);
    query.bindValue(":loc", building.location);
    query.bindValue(":id", building.id);
    
    if (!query.exec()) {
        qDebug() << "FacilityRepository::updateBuilding error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool FacilityRepository::deleteBuilding(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM buildings WHERE id = :id");
    query.bindValue(":id", id);
    
    if (!query.exec()) {
        qDebug() << "FacilityRepository::deleteBuilding error:" << query.lastError().text();
        return false;
    }
    return true;
}

QList<Room> FacilityRepository::getAllRooms() {
    QList<Room> list;
    QSqlQuery query("SELECT id, building_id, room_number, type, capacity FROM rooms");
    while (query.next()) {
        Room r;
        r.id = query.value(0).toInt();
        r.buildingId = query.value(1).toInt();
        r.roomNumber = query.value(2).toString();
        r.type = query.value(3).toString();
        r.capacity = query.value(4).toInt();
        list.append(r);
    }
    return list;
}

bool FacilityRepository::addRoom(const Room& room) {
    QSqlQuery query;
    query.prepare("INSERT INTO rooms (building_id, room_number, type, capacity) VALUES (:bid, :num, :type, :cap)");
    query.bindValue(":bid", room.buildingId);
    query.bindValue(":num", room.roomNumber);
    query.bindValue(":type", room.type);
    query.bindValue(":cap", room.capacity);
    
    if (!query.exec()) {
        qDebug() << "FacilityRepository::addRoom error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool FacilityRepository::updateRoom(const Room& room) {
    QSqlQuery query;
    query.prepare("UPDATE rooms SET building_id = :bid, room_number = :num, type = :type, capacity = :cap WHERE id = :id");
    query.bindValue(":bid", room.buildingId);
    query.bindValue(":num", room.roomNumber);
    query.bindValue(":type", room.type);
    query.bindValue(":cap", room.capacity);
    query.bindValue(":id", room.id);
    
    if (!query.exec()) {
        qDebug() << "FacilityRepository::updateRoom error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool FacilityRepository::deleteRoom(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM rooms WHERE id = :id");
    query.bindValue(":id", id);
    
    if (!query.exec()) {
        qDebug() << "FacilityRepository::deleteRoom error:" << query.lastError().text();
        return false;
    }
    return true;
}
