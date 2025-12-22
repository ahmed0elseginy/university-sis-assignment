#ifndef FACILITYREPOSITORY_H
#define FACILITYREPOSITORY_H

#include "building.h"
#include "room.h"
#include <QList>

class FacilityRepository {
public:
    // Buildings
    bool addBuilding(const Building& building);
    bool updateBuilding(const Building& building);
    bool deleteBuilding(int id);
    QList<Building> getAllBuildings();
    Building getBuildingById(int id);

    // Rooms
    bool addRoom(const Room& room);
    bool updateRoom(const Room& room);
    bool deleteRoom(int id);
    QList<Room> getRoomsByBuildingId(int buildingId);
    QList<Room> getAllRooms();
};

#endif // FACILITYREPOSITORY_H
