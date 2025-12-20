#ifndef SECTIONSYSTEM_H
#define SECTIONSYSTEM_H

#include <QWidget>
#include <QTableView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include "../../modules/academic/sectionrepository.h"
#include "../../modules/academic/courserepository.h"

class SectionSystem : public QWidget
{
    Q_OBJECT

public:
    explicit SectionSystem(QWidget *parent = nullptr);

private slots:
    void onAddSection();
    void onSearch(const QString &text);
    void editSection(int id);
    void deleteSection(int id);

private:
    void setupUi();
    void loadSections();
    void styleTable();
    void refreshData();

    QTableView *m_view;
    QStandardItemModel *m_model;
    QLineEdit *m_searchBar;
    QPushButton *m_btnAdd;
    
    SectionRepository m_repo;
    CourseRepository m_courseRepo;
};

#endif // SECTIONSYSTEM_H
