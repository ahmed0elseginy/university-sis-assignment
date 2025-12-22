#ifndef BUILDING_H
#define BUILDING_H

#include <QString>

struct Building {
    int id = -1;
    QString name;
    QString code;
    QString location;

    bool isValid() const {
        return !name.isEmpty() && !code.isEmpty();
    }
};

#endif // BUILDING_H
