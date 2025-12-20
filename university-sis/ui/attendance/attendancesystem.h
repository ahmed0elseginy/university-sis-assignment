#ifndef ATTENDANCESYSTEM_H
#define ATTENDANCESYSTEM_H

#include <QWidget>
#include <QTableView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "../../modules/attendance/attendancerepository.h"

class AttendanceSystem : public QWidget
{
    Q_OBJECT

public:
    explicit AttendanceSystem(QWidget *parent = nullptr);

private slots:
    void onAddAttendance();
    void editAttendance(int id);
    void deleteAttendance(int id);
    void refreshData();

private:
    void setupUi();
    void loadAttendance();
    void styleTable();

    QTableView *m_view;
    QStandardItemModel *m_model;
    QPushButton *m_btnAdd;
    
    AttendanceRepository m_repo;
};

#endif // ATTENDANCESYSTEM_H
