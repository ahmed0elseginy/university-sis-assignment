#ifndef ENROLLMENTSYSTEM_H
#define ENROLLMENTSYSTEM_H

#include <QWidget>
#include <QTableView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "../../modules/enrollment/enrollmentrepository.h"

class EnrollmentSystem : public QWidget
{
    Q_OBJECT

public:
    explicit EnrollmentSystem(QWidget *parent = nullptr);
    void setUserContext(const QString& role, int userId);

private slots:
    void onEnroll();
    void deleteEnrollment(int studentId, int sectionId);
    void refreshData();

private:
    void setupUi();
    void loadEnrollments();
    void styleTable();

    QTableView *m_view;
    QStandardItemModel *m_model;
    QPushButton *m_btnAdd;
    
    EnrollmentRepository m_repo;
    QString m_currentUserRole;
    int m_currentUserId = -1;
};

#endif // ENROLLMENTSYSTEM_H
