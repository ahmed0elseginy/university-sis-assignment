#ifndef GRADESSYSTEM_H
#define GRADESSYSTEM_H

#include <QWidget>
#include <QTableView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QLabel>
#include "../basesystemwidget.h"

class GradesSystem : public BaseSystemWidget
{
    Q_OBJECT

public:
    explicit GradesSystem(QWidget *parent = nullptr);
    void setUserContext(const QString& role, int userId);

private slots:
    void onCourseChanged(int index);
    void onAddGrade();
    void onDeleteGrade();
    void refreshGrades();
    void calculateGPA();

private:
    void setupUi();
    void loadCourses();
    void loadGrades(int courseId);
    QString calculateTotalGrade(const QString& a1, const QString& a2, const QString& finalExam);
    
    QString m_role;
    int m_userId;
    QTableView *m_gradesTable;
    QStandardItemModel *m_gradesModel;
    QComboBox *m_courseCombo;
    QComboBox *m_assignmentTypeCombo;
    QComboBox *m_studentCombo;  // For admin/faculty to select student
    QDoubleSpinBox *m_scoreSpin;
    QDoubleSpinBox *m_maxScoreSpin;  // Hidden, always 100
    QPushButton *m_addBtn;
    QPushButton *m_deleteBtn;
    QLabel *m_gpaLabel;
};

#endif // GRADESSYSTEM_H

