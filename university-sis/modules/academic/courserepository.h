#ifndef COURSEREPOSITORY_H
#define COURSEREPOSITORY_H

#include "course.h"
#include <vector>
#include <optional>

class CourseRepository {
public:
    CourseRepository();
    
    bool addCourse(const Course& course);
    bool updateCourse(const Course& course);
    bool deleteCourse(int id);
    
    std::vector<Course> getAllCourses();
    std::optional<Course> getCourseById(int id);
};

#endif // COURSEREPOSITORY_H
