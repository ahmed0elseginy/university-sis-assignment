#include "attendancedialog.h"

AttendanceDialog::AttendanceDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Record Attendance");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setMinimumWidth(350);
    
    setupUi();
}

void AttendanceDialog::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // Header
    QLabel *titleLabel = new QLabel("New Attendance Record");
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50; margin-bottom: 10px;");
    mainLayout->addWidget(titleLabel);

    // Form
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(15);
    formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_studentIdEdit = new QSpinBox();
    m_studentIdEdit->setRange(1, 999999);
    m_studentIdEdit->setStyleSheet("QSpinBox { padding: 8px; border: 1px solid #bdc3c7; border-radius: 4px; }");
    
    m_courseIdEdit = new QSpinBox();
    m_courseIdEdit->setRange(1, 999999);
    m_courseIdEdit->setStyleSheet("QSpinBox { padding: 8px; border: 1px solid #bdc3c7; border-radius: 4px; }");
    
    m_statusEdit = new QComboBox();
    m_statusEdit->addItems({"Present", "Absent", "Late", "Excused"});
    m_statusEdit->setStyleSheet("QComboBox { padding: 8px; border: 1px solid #bdc3c7; border-radius: 4px; }");
    
    m_dateEdit = new QDateEdit();
    m_dateEdit->setCalendarPopup(true);
    m_dateEdit->setDate(QDate::currentDate());
    m_dateEdit->setStyleSheet("QDateEdit { padding: 8px; border: 1px solid #bdc3c7; border-radius: 4px; }");

    formLayout->addRow(new QLabel("Student ID:"), m_studentIdEdit);
    formLayout->addRow(new QLabel("Course ID:"), m_courseIdEdit);
    formLayout->addRow(new QLabel("Status:"), m_statusEdit);
    formLayout->addRow(new QLabel("Date:"), m_dateEdit);

    mainLayout->addLayout(formLayout);

    // Buttons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();

    m_cancelBtn = new QPushButton("Cancel");
    m_cancelBtn->setCursor(Qt::PointingHandCursor);
    m_cancelBtn->setStyleSheet("QPushButton { background-color: #95a5a6; color: white; border: none; padding: 10px 20px; border-radius: 4px; font-weight: bold; } "
                               "QPushButton:hover { background-color: #7f8c8d; }");
    
    m_saveBtn = new QPushButton("Save");
    m_saveBtn->setCursor(Qt::PointingHandCursor);
    m_saveBtn->setStyleSheet("QPushButton { background-color: #3498db; color: white; border: none; padding: 10px 20px; border-radius: 4px; font-weight: bold; } "
                             "QPushButton:hover { background-color: #2980b9; }");

    btnLayout->addWidget(m_cancelBtn);
    btnLayout->addWidget(m_saveBtn);

    mainLayout->addLayout(btnLayout);

    connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_saveBtn, &QPushButton::clicked, this, &AttendanceDialog::onSave);
}

void AttendanceDialog::onSave()
{
    m_attendance.studentId = m_studentIdEdit->value();
    m_attendance.courseId = m_courseIdEdit->value();
    m_attendance.status = m_statusEdit->currentText();
    m_attendance.date = m_dateEdit->date();
    accept();
}

Attendance AttendanceDialog::getAttendance() const
{
    return m_attendance;
}
