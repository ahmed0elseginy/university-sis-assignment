#ifndef ATTENDANCESYSTEM_H
#define ATTENDANCESYSTEM_H

#include <QWidget>
#include <QTableView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include "../../modules/attendance/attendancerepository.h"

class AttendanceSystem : public QWidget
{
    Q_OBJECT

public:
    explicit AttendanceSystem(QWidget *parent = nullptr);

private slots:
    void onAddAttendance();
    void deleteAttendance(int id);
    void refreshData();
    void applyFilters();

private:
    void setupUi();
    void loadAttendance();
    void styleTable();
    void loadCourses();

    QTableView *m_view;
    QStandardItemModel *m_model;
    QPushButton *m_btnAdd;
    
    // Filter controls
    QLineEdit *m_filterStudentName;
    QComboBox *m_filterCourse;
    QComboBox *m_filterStatus;
    QDateEdit *m_filterDateFrom;
    QDateEdit *m_filterDateTo;
    QPushButton *m_btnClearFilters;
    
    AttendanceRepository m_repo;
};

#endif // ATTENDANCESYSTEM_H
