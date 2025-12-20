#ifndef ATTENDANCEREPOSITORY_H
#define ATTENDANCEREPOSITORY_H

#include "attendance.h"
#include <vector>
#include <optional>

class AttendanceRepository {
public:
    AttendanceRepository();
    
    bool addAttendance(const Attendance& att);
    bool deleteAttendance(int id);
    
    std::vector<Attendance> getAllAttendance();
};

#endif // ATTENDANCEREPOSITORY_H
