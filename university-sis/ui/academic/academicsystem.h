#ifndef ACADEMICSYSTEM_H
#define ACADEMICSYSTEM_H

#include <QWidget>
#include <QTableView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTabWidget>
#include "../../modules/academic/courserepository.h"

class AcademicSystem : public QWidget
{
    Q_OBJECT

public:
    explicit AcademicSystem(QWidget *parent = nullptr);

private slots:
    void onAddCourse();
    void onSearch(const QString &text);
    void viewCourse(int id);
    void editCourse(int id);
    void deleteCourse(int id);

private:
    void setupUi();
    void loadCourses();
    void styleTable(QTableView* view); // Helper
    void refreshData();

    QTableView *m_courseView;
    QStandardItemModel *m_courseModel;
    
    QLineEdit *m_searchBar;
    
    CourseRepository m_courseRepo;
};

#endif // ACADEMICSYSTEM_H
