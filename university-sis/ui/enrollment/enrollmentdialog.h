#ifndef ENROLLMENTDIALOG_H
#define ENROLLMENTDIALOG_H

#include <QDialog>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include "../../modules/enrollment/enrollment.h"

class EnrollmentDialog : public QDialog {
    Q_OBJECT
public:
    explicit EnrollmentDialog(QWidget *parent = nullptr);
    Enrollment getEnrollment() const;

private slots:
    void onSave();

private:
    void setupUi();
    
    QSpinBox *m_studentIdEdit;
    QSpinBox *m_sectionIdEdit;
    
    QPushButton *m_saveBtn;
    QPushButton *m_cancelBtn;

    Enrollment m_enrollment;
};

#endif // ENROLLMENTDIALOG_H
