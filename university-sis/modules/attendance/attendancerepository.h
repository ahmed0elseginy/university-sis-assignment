#ifndef ATTENDANCEREPOSITORY_H
#define ATTENDANCEREPOSITORY_H

#include "attendance.h"
#include <vector>
#include <optional>

class AttendanceRepository {
public:
    AttendanceRepository();
    
    bool addAttendance(const Attendance& att);
    bool updateAttendance(const Attendance& att); // New
    bool deleteAttendance(int id);
    
    std::vector<Attendance> getAllAttendance();
    std::optional<Attendance> getAttendanceById(int id); // New
};

#endif // ATTENDANCEREPOSITORY_H
