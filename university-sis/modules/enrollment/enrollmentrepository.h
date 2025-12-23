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
    
    // Returns enrollments with student names (JOIN query)
    std::vector<Enrollment> getAllEnrollmentsWithNames();
    
    // Returns list of student IDs enrolled in a specific section
    std::vector<int> getStudentIdsBySection(int sectionId);
};

#endif // ENROLLMENTREPOSITORY_H
