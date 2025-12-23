#ifndef STUDENTPORTAL_H
#define STUDENTPORTAL_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QGroupBox>
#include <QProgressBar>
#include <QDate>
#include "../modules/student/studentrepository.h"
#include "../database/databasemanager.h"
#include <QSqlQuery>

// Forward declarations
struct StudentInfo {
    int id;
    QString name;
    int year;
    QString department;
    int sectionId;
    QString username;
};

struct CourseInfo {
    int courseId;
    QString courseName;
    int sectionId;
    int hours;
    QString grade;
};

struct GradeInfo {
    int courseId;
    QString courseName;
    QString a1;
    QString a2;
    QString finalExam;
    QString total;
};

struct AttendanceInfo {
    int courseId;
    QString courseName;
    int present;
    int absent;
    int late;
    double attendanceRate;
};

struct PaymentInfo {
    int paymentId;
    double amount;
    QString description;
    QString status;
    QDate date;
};

class StudentPortal : public QWidget
{
    Q_OBJECT
public:
    explicit StudentPortal(QWidget *parent = nullptr);
    void setUserContext(const QString& role, int userId);       
    void refreshData();

signals:
    void dataChanged();

private slots:
    void onRefresh();

private:
    void setupUi();
    void loadStudentData();
    void createProfileCard();
    void createCoursesCard();
    void createGradesCard();
    void createAttendanceCard();
    void createFinancialCard();
    void createQuickStatsCard();
    QWidget* createUpcomingEventsCard();
    
    // Data fetching methods
    StudentInfo getStudentInfo();
    QList<CourseInfo> getStudentCourses();
    QList<GradeInfo> getStudentGrades();
    QList<AttendanceInfo> getStudentAttendance();
    QList<PaymentInfo> getStudentPayments();
    double calculateGPA();
    int getUpcomingEventsCount();
    
    // UI Helpers
    QWidget* createCard(const QString& title, QWidget* content);
    QWidget* createStatLabel(const QString& label, const QString& value, const QString& color = "#007AFF");
    void applyCardStyle(QWidget* card);

    QScrollArea *m_scrollArea;
    QWidget *m_contentWidget;
    QVBoxLayout *m_mainLayout;
    StudentRepository m_repo;
    
    QString m_currentUserRole;
    int m_currentUserId = -1;
    
    // UI Components
    QLabel *m_profileName;
    QLabel *m_profileId;
    QLabel *m_profileDept;
    QLabel *m_profileYear;
    QLabel *m_gpaLabel;
    QTableWidget *m_coursesTable;
    QTableWidget *m_gradesTable;
    QTableWidget *m_attendanceTable;
    QTableWidget *m_paymentsTable;
};

#endif // STUDENTPORTAL_H
