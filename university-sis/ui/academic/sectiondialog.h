#ifndef SECTIONDIALOG_H
#define SECTIONDIALOG_H

#include <QDialog>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include "../../modules/academic/section.h"

class SectionDialog : public QDialog {
    Q_OBJECT
public:
    explicit SectionDialog(QWidget *parent = nullptr, const Section* section = nullptr);
    Section getSection() const;

private slots:
    void onSave();

private:
    void setupUi();
    
    QComboBox *m_courseCombo;
    QSpinBox *m_maxEdit;
    QPushButton *m_saveBtn;
    QPushButton *m_cancelBtn;

    Section m_section;
    CourseRepository m_courseRepo;
};

#endif // SECTIONDIALOG_H
