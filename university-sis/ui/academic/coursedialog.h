#ifndef COURSEDIALOG_H
#define COURSEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include "../../modules/academic/course.h"

class CourseDialog : public QDialog {
    Q_OBJECT
public:
    explicit CourseDialog(QWidget *parent = nullptr, const Course* course = nullptr);
    Course getCourse() const;

private slots:
    void onSave();

private:
    void setupUi();
    
    QLineEdit *m_nameEdit;
    QSpinBox *m_yearEdit;
    QSpinBox *m_hoursEdit;
    
    QPushButton *m_saveBtn;
    QPushButton *m_cancelBtn;

    Course m_course;
    bool m_isEdit;
};

#endif // COURSEDIALOG_H
