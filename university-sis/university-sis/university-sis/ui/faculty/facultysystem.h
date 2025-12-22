#ifndef FACULTYSYSTEM_H
#define FACULTYSYSTEM_H

#include <QWidget>
#include <QTableView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include "../../modules/faculty/facultyrepository.h"

class FacultySystem : public QWidget
{
    Q_OBJECT

public:
    explicit FacultySystem(QWidget *parent = nullptr);

private slots:
    void onAddFaculty();
    void onSearch(const QString &text);
    void editFaculty(int id);
    void deleteFaculty(int id);

private:
    void setupUi();
    void loadFaculty();
    void styleTable();
    void refreshData();

    QTableView *m_view;
    QStandardItemModel *m_model;
    QLineEdit *m_searchBar;
    QPushButton *m_btnAdd;
    
    FacultyRepository m_repo;
};

#endif // FACULTYSYSTEM_H
