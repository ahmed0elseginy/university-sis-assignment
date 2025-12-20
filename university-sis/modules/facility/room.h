#ifndef ROOM_H
#define ROOM_H

#include <QString>

struct Room {
    int id = -1;
    int buildingId = -1;
    QString roomNumber;
    QString type;
    int capacity = 0;
};

#endif // ROOM_H
