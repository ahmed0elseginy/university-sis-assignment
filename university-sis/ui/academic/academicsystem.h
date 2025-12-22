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
#include "../../modules/academic/sectionrepository.h"

class AcademicSystem : public QWidget
{
    Q_OBJECT

public:
    explicit AcademicSystem(QWidget *parent = nullptr);

private slots:
    void onAddCourse();
    void onAddSection(); // New Slot
    void onSearch(const QString &text);
    void viewCourse(int id);
    void editCourse(int id);
    void deleteCourse(int id);
    void deleteSection(int id); // New Slot

private:
    void setupUi();
    void loadCourses();
    void loadSections(); // New Logic
    void styleTable(QTableView* view); // Helper
    void refreshData();

    QTabWidget *m_tabs;
    QTableView *m_courseView;
    QTableView *m_sectionView;
    
    QStandardItemModel *m_courseModel;
    QStandardItemModel *m_sectionModel;
    
    QLineEdit *m_searchBar;
    
    CourseRepository m_courseRepo;
    SectionRepository m_sectionRepo;
};

#endif // ACADEMICSYSTEM_H
