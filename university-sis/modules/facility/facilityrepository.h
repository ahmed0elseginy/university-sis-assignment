#ifndef FACILITYREPOSITORY_H
#define FACILITYREPOSITORY_H

#include <QString>
#include <QList>
#include <QVariant>
#include "building.h"
#include "room.h"

class FacilityRepository {
public:
    FacilityRepository();

    QList<Building> getAllBuildings();
    Building getBuildingById(int id);
    bool addBuilding(const Building& building);
    bool updateBuilding(const Building& building);
    bool deleteBuilding(int id);

    QList<Room> getAllRooms();
    bool addRoom(const Room& room);
    bool updateRoom(const Room& room);
    bool deleteRoom(int id);
};

#endif // FACILITYREPOSITORY_H
