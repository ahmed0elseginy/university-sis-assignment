#include "enrollmentdialog.h"

EnrollmentDialog::EnrollmentDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Enroll Student");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setMinimumWidth(350);
    
    setupUi();
}

void EnrollmentDialog::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // Header
    QLabel *titleLabel = new QLabel("New Enrollment");
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50; margin-bottom: 10px;");
    mainLayout->addWidget(titleLabel);

    // Form
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(15);
    formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_studentIdEdit = new QSpinBox();
    m_studentIdEdit->setRange(1, 999999);
    m_studentIdEdit->setStyleSheet("QSpinBox { padding: 8px; border: 1px solid #bdc3c7; border-radius: 4px; }");
    
    m_sectionIdEdit = new QSpinBox();
    m_sectionIdEdit->setRange(1, 999999);
    m_sectionIdEdit->setStyleSheet("QSpinBox { padding: 8px; border: 1px solid #bdc3c7; border-radius: 4px; }");

    formLayout->addRow(new QLabel("Student ID:"), m_studentIdEdit);
    formLayout->addRow(new QLabel("Section ID:"), m_sectionIdEdit);

    mainLayout->addLayout(formLayout);

    // Buttons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();

    m_cancelBtn = new QPushButton("Cancel");
    m_cancelBtn->setCursor(Qt::PointingHandCursor);
    m_cancelBtn->setStyleSheet("QPushButton { background-color: #95a5a6; color: white; border: none; padding: 10px 20px; border-radius: 4px; font-weight: bold; } "
                               "QPushButton:hover { background-color: #7f8c8d; }");
    
    m_saveBtn = new QPushButton("Enroll");
    m_saveBtn->setCursor(Qt::PointingHandCursor);
    m_saveBtn->setStyleSheet("QPushButton { background-color: #3498db; color: white; border: none; padding: 10px 20px; border-radius: 4px; font-weight: bold; } "
                             "QPushButton:hover { background-color: #2980b9; }");

    btnLayout->addWidget(m_cancelBtn);
    btnLayout->addWidget(m_saveBtn);

    mainLayout->addLayout(btnLayout);

    connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_saveBtn, &QPushButton::clicked, this, &EnrollmentDialog::onSave);
}

void EnrollmentDialog::onSave()
{
    m_enrollment.studentId = m_studentIdEdit->value();
    m_enrollment.sectionId = m_sectionIdEdit->value();
    accept();
}

Enrollment EnrollmentDialog::getEnrollment() const
{
    return m_enrollment;
}
