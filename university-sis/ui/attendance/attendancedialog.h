#ifndef ATTENDANCEDIALOG_H
#define ATTENDANCEDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <QTableView>
#include <QStandardItemModel>
#include "../../modules/attendance/attendance.h"
#include "../../modules/academic/sectionrepository.h"
#include "../../modules/academic/courserepository.h"
#include "../../modules/enrollment/enrollmentrepository.h"
#include "../../modules/student/studentrepository.h"

class AttendanceDialog : public QDialog {
    Q_OBJECT
public:
    explicit AttendanceDialog(QWidget *parent = nullptr);
    std::vector<Attendance> getAttendanceRecords() const;

private slots:
    void onSectionChanged(int index);
    void onLoadStudents();
    void onSave();

private:
    void setupUi();
    void loadSections();
    void loadStudentsForSection(int sectionId);
    
    QComboBox *m_sectionEdit;
    QDateEdit *m_dateEdit;
    QLabel *m_courseLabel;
    QTableView *m_studentsTable;
    QStandardItemModel *m_studentsModel;
    
    QPushButton *m_loadBtn;
    QPushButton *m_saveBtn;
    QPushButton *m_cancelBtn;

    int m_currentSectionId = 0;
    int m_currentCourseId = 0;
    QDate m_selectedDate;
    
    SectionRepository m_sectionRepo;
    CourseRepository m_courseRepo;
    EnrollmentRepository m_enrollmentRepo;
    StudentRepository m_studentRepo;
};

#endif // ATTENDANCEDIALOG_H
