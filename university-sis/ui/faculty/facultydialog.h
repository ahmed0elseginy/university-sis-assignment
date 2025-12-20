#ifndef FACULTYDIALOG_H
#define FACULTYDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include "../../modules/faculty/faculty.h"

class FacultyDialog : public QDialog {
    Q_OBJECT
public:
    explicit FacultyDialog(QWidget *parent = nullptr, const Faculty* faculty = nullptr);
    Faculty getFaculty() const;

private slots:
    void onSave();

private:
    void setupUi();
    
    QLineEdit *m_nameEdit;
    QLineEdit *m_emailEdit;
    QLineEdit *m_deptEdit;
    QComboBox *m_posEdit;
    
    QPushButton *m_saveBtn;
    QPushButton *m_cancelBtn;

    Faculty m_faculty;
    bool m_isEdit;
};

#endif // FACULTYDIALOG_H
