#include "facilityrepository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVariant>

// Buildings
bool FacilityRepository::addBuilding(const Building& building)
{
    QSqlQuery query;
    query.prepare("INSERT INTO buildings (name, code, location) VALUES (:name, :code, :loc)");
    query.bindValue(":name", building.name);
    query.bindValue(":code", building.code);
    query.bindValue(":loc", building.location);
    if(!query.exec()){
        qDebug() << "addBuilding error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool FacilityRepository::updateBuilding(const Building& building)
{
    QSqlQuery query;
    query.prepare("UPDATE buildings SET name=:name, code=:code, location=:loc WHERE building_id=:id");
    query.bindValue(":name", building.name);
    query.bindValue(":code", building.code);
    query.bindValue(":loc", building.location);
    query.bindValue(":id", building.id);
    return query.exec();
}

bool FacilityRepository::deleteBuilding(int id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM buildings WHERE building_id=:id");
    query.bindValue(":id", id);
    return query.exec();
}

QList<Building> FacilityRepository::getAllBuildings()
{
    QList<Building> list;
    QSqlQuery query("SELECT building_id, name, code, location FROM buildings");
    while(query.next()){
        Building b;
        b.id = query.value(0).toInt();
        b.name = query.value(1).toString();
        b.code = query.value(2).toString();
        b.location = query.value(3).toString();
        list.append(b);
    }
    return list;
}

Building FacilityRepository::getBuildingById(int id)
{
    QSqlQuery query;
    query.prepare("SELECT building_id, name, code, location FROM buildings WHERE building_id=:id");
    query.bindValue(":id", id);
    Building b;
    if(query.exec() && query.next()){
        b.id = query.value(0).toInt();
        b.name = query.value(1).toString();
        b.code = query.value(2).toString();
        b.location = query.value(3).toString();
    }
    return b;
}

// Rooms
bool FacilityRepository::addRoom(const Room& room)
{
    QSqlQuery query;
    query.prepare("INSERT INTO rooms (building_id, room_number, type, capacity) VALUES (:bid, :num, :type, :cap)");
    query.bindValue(":bid", room.buildingId);
    query.bindValue(":num", room.roomNumber);
    query.bindValue(":type", room.type);
    query.bindValue(":cap", room.capacity);
    if(!query.exec()){
        qDebug() << "addRoom error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool FacilityRepository::updateRoom(const Room& room)
{
    QSqlQuery query;
    query.prepare("UPDATE rooms SET building_id=:bid, room_number=:num, type=:type, capacity=:cap WHERE room_id=:id");
    query.bindValue(":bid", room.buildingId);
    query.bindValue(":num", room.roomNumber);
    query.bindValue(":type", room.type);
    query.bindValue(":cap", room.capacity);
    query.bindValue(":id", room.id);
    return query.exec();
}

bool FacilityRepository::deleteRoom(int id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM rooms WHERE room_id=:id");
    query.bindValue(":id", id);
    return query.exec();
}

QList<Room> FacilityRepository::getRoomsByBuildingId(int buildingId)
{
    QList<Room> list;
    QSqlQuery query;
    query.prepare("SELECT room_id, building_id, room_number, type, capacity FROM rooms WHERE building_id=:bid");
    query.bindValue(":bid", buildingId);
    if(query.exec()){
        while(query.next()){
            Room r;
            r.id = query.value(0).toInt();
            r.buildingId = query.value(1).toInt();
            r.roomNumber = query.value(2).toString();
            r.type = query.value(3).toString();
            r.capacity = query.value(4).toInt();
            list.append(r);
        }
    }
    return list;
}

QList<Room> FacilityRepository::getAllRooms()
{
    QList<Room> list;
    QSqlQuery query("SELECT room_id, building_id, room_number, type, capacity FROM rooms");
    while(query.next()){
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
