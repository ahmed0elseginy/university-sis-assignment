#ifndef ROOM_H
#define ROOM_H

#include <QString>

struct Room {
    int id = -1;
    int buildingId = -1;
    QString roomNumber;
    QString type; // "Lab", "Lecture Hall", etc.
    int capacity = 0;

    bool isValid() const {
        return buildingId != -1 && !roomNumber.isEmpty();
    }
};

#endif // ROOM_H
