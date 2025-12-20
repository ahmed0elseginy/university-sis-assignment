#ifndef STUDENTDIALOG_H
#define STUDENTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include "../../modules/student/student.h"

/**
 * @brief Dialog for Adding or Editing a Student according to new schema.
 */
class StudentDialog : public QDialog {
    Q_OBJECT
public:
    explicit StudentDialog(QWidget *parent = nullptr, const Student* student = nullptr);
    Student getStudent() const;

private slots:
    void onSave();

private:
    void setupUi();
    
    QLineEdit *m_nameEdit;
    QSpinBox *m_yearEdit;
    QLineEdit *m_departmentEdit;
    QSpinBox *m_sectionEdit;
    QLineEdit *m_userEdit;
    QLineEdit *m_passEdit;
    
    QPushButton *m_saveBtn;
    QPushButton *m_cancelBtn;

    Student m_student;
    bool m_isEdit;
};

#endif // STUDENTDIALOG_H
