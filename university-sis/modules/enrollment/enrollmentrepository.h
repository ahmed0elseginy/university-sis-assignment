#ifndef ENROLLMENTREPOSITORY_H
#define ENROLLMENTREPOSITORY_H

#include "enrollment.h"
#include <vector>

class EnrollmentRepository {
public:
    EnrollmentRepository();
    
    bool enrollStudent(const Enrollment& enrollment);
    bool unenrollStudent(int studentId, int sectionId);
    
    // Returns list of (Student ID, Section ID) pairs
    std::vector<Enrollment> getAllEnrollments();
};

#endif // ENROLLMENTREPOSITORY_H
