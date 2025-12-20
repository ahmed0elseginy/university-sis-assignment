#ifndef ATTENDANCEDIALOG_H
#define ATTENDANCEDIALOG_H

#include <QDialog>
#include <QSpinBox>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include "../../modules/attendance/attendance.h"

class AttendanceDialog : public QDialog {
    Q_OBJECT
public:
    explicit AttendanceDialog(QWidget *parent = nullptr, const Attendance* attendance = nullptr);
    Attendance getAttendance() const;

private slots:
    void onSave();

private:
    void setupUi();
    
    QSpinBox *m_studentIdEdit;
    QSpinBox *m_courseIdEdit;
    QComboBox *m_statusEdit;
    QDateEdit *m_dateEdit;
    
    QPushButton *m_saveBtn;
    QPushButton *m_cancelBtn;

    Attendance m_attendance;
};

#endif // ATTENDANCEDIALOG_H
