#ifndef STUDENTPORTAL_H
#define STUDENTPORTAL_H

#include <QWidget>
#include <QTableView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QStandardItemModel>
#include "../modules/student/studentrepository.h"

class StudentPortal : public QWidget
{
    Q_OBJECT
public:
    explicit StudentPortal(QWidget *parent = nullptr);
    void setUserContext(const QString& role, int userId);       
    void refreshData();

private slots:
    void onAddStudent();
    void onSearch(const QString &text);

private:
    void setupUi();
    void loadStudents();
    
    // Actions
    void viewStudent(int id);
    void editStudent(int id);
    void deleteStudent(int id);

    // UI Helpers
    void styleTable();

    QTableView *m_view;
    QStandardItemModel *m_model;
    StudentRepository m_repo;
    
    QPushButton *m_btnAdd;
    QLineEdit *m_searchBar;

    QString m_currentUserRole;
    int m_currentUserId = -1;
};

#endif // STUDENTPORTAL_H
