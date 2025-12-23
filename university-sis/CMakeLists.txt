cmake_minimum_required(VERSION 3.16)

project(university-sis VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql LinguistTools)

set(TS_FILES university-sis_ar_EG.ts)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        database/databasemanager.cpp
        database/databasemanager.h
        ui/studentportal.cpp
        ui/studentportal.h
        modules/student/student.h
        modules/student/studentrepository.cpp
        modules/student/studentrepository.h
        ui/student/studentdialog.cpp
        ui/student/studentdialog.h
        utils/thememanager.cpp
        utils/thememanager.h
        ui/login/logindialog.cpp
        ui/login/logindialog.h
        ui/profile/profilewidget.h
        ui/profile/profilewidget.cpp
        ui/basesystemwidget.h
        ui/basesystemwidget.cpp

        # News System
        ui/news/newssystem.h
        ui/news/newssystem.cpp
        
        # Library System
        ui/library/librarysystem.h
        ui/library/librarysystem.cpp
        
        # Academic System
        ui/academic/academicsystem.h
        ui/academic/academicsystem.cpp
        ui/academic/coursedialog.h
        ui/academic/coursedialog.cpp
        modules/academic/course.h
        modules/academic/courserepository.h
        modules/academic/courserepository.cpp
        resources.qrc
        modules/academic/section.h
        modules/academic/sectionrepository.h
        modules/academic/sectionrepository.cpp
        ui/academic/sectiondialog.h
        ui/academic/sectiondialog.cpp

        # Finance System
        ui/finance/financesystem.h
        ui/finance/financesystem.cpp
        ui/finance/paymentdialog.h
        ui/finance/paymentdialog.cpp
        modules/finance/payment.h
        modules/finance/paymentrepository.h
        modules/finance/paymentrepository.cpp

        # Faculty System
        ui/faculty/facultysystem.h
        ui/faculty/facultysystem.cpp
        ui/faculty/facultydialog.h
        ui/faculty/facultydialog.cpp
        modules/faculty/faculty.h
        modules/faculty/facultyrepository.h
        modules/faculty/facultyrepository.cpp

        # Enrollment System
        ui/enrollment/enrollmentsystem.h
        ui/enrollment/enrollmentsystem.cpp
        ui/enrollment/enrollmentdialog.h
        ui/enrollment/enrollmentdialog.cpp
        modules/enrollment/enrollment.h
        modules/enrollment/enrollmentrepository.h
        modules/enrollment/enrollmentrepository.cpp

        # Attendance System
        ui/attendance/attendancesystem.h
        ui/attendance/attendancesystem.cpp
        ui/attendance/attendancedialog.h
        ui/attendance/attendancedialog.cpp
        modules/attendance/attendance.h
        modules/attendance/attendancerepository.h
        modules/attendance/attendancerepository.cpp

        # Facility System
        ui/facility/facilitysystem.h
        ui/facility/facilitysystem.cpp
        ui/facility/buildingdialog.h
        ui/facility/buildingdialog.cpp
        ui/facility/roomdialog.h
        ui/facility/roomdialog.cpp
        modules/facility/building.h
        modules/facility/room.h
        modules/facility/facilityrepository.h
        modules/facility/facilityrepository.cpp

        # Calendar System
        ui/calendar/calendarsystem.h
        ui/calendar/calendarsystem.cpp

        # Grades System
        ui/grades/gradessystem.h
        ui/grades/gradessystem.cpp

        # Reports System
        ui/reports/reportssystem.h
        ui/reports/reportssystem.cpp

        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(university-sis
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET university-sis APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(university-sis SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(university-sis
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(university-sis PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.university-sis)
endif()
set_target_properties(university-sis PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS university-sis
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(university-sis)
endif()
