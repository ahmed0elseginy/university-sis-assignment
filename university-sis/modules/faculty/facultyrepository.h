#ifndef FACULTYREPOSITORY_H
#define FACULTYREPOSITORY_H

#include "faculty.h"
#include <vector>
#include <optional>

class FacultyRepository {
public:
    FacultyRepository();
    
    bool addFaculty(const Faculty& faculty);
    bool updateFaculty(const Faculty& faculty);
    bool deleteFaculty(int id);
    
    std::vector<Faculty> getAllFaculty();
    std::optional<Faculty> getFacultyById(int id);
};

#endif // FACULTYREPOSITORY_H
