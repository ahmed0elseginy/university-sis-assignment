#include "sectiondialog.h"

SectionDialog::SectionDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Create Section");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setMinimumWidth(300);
    setupUi();
}

void SectionDialog::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    QLabel *titleLabel = new QLabel("New Course Section");
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50; margin-bottom: 10px;");
    mainLayout->addWidget(titleLabel);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(15);

    m_courseIdEdit = new QSpinBox();
    m_courseIdEdit->setRange(1, 999999);
    m_courseIdEdit->setStyleSheet("QSpinBox { padding: 8px; border: 1px solid #bdc3c7; border-radius: 4px; }");
    
    m_maxEdit = new QSpinBox();
    m_maxEdit->setRange(1, 1000);
    m_maxEdit->setValue(30);
    m_maxEdit->setStyleSheet("QSpinBox { padding: 8px; border: 1px solid #bdc3c7; border-radius: 4px; }");

    formLayout->addRow(new QLabel("Course ID:"), m_courseIdEdit);
    formLayout->addRow(new QLabel("Capacity:"), m_maxEdit);

    mainLayout->addLayout(formLayout);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    m_cancelBtn = new QPushButton("Cancel");
    m_cancelBtn->setStyleSheet("QPushButton { background-color: #95a5a6; color: white; border: none; padding: 10px 20px; border-radius: 4px; font-weight: bold; } QPushButton:hover { background-color: #7f8c8d; }");
    m_saveBtn = new QPushButton("Create");
    m_saveBtn->setStyleSheet("QPushButton { background-color: #3498db; color: white; border: none; padding: 10px 20px; border-radius: 4px; font-weight: bold; } QPushButton:hover { background-color: #2980b9; }");
    
    btnLayout->addWidget(m_cancelBtn);
    btnLayout->addWidget(m_saveBtn);
    mainLayout->addLayout(btnLayout);

    connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_saveBtn, &QPushButton::clicked, this, &SectionDialog::onSave);
}

void SectionDialog::onSave()
{
    m_section.courseId = m_courseIdEdit->value();
    m_section.maxStudents = m_maxEdit->value();
    accept();
}

Section SectionDialog::getSection() const
{
    return m_section;
}
