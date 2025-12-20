#include "coursedialog.h"

CourseDialog::CourseDialog(QWidget *parent, const Course* course)
    : QDialog(parent), m_isEdit(course != nullptr)
{
    if (course) {
        m_course = *course;
    } else {
        m_course = Course();
    }
    
    setWindowTitle(m_isEdit ? "Edit Course" : "Add New Course");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setMinimumWidth(400);
    
    setupUi();
}

void CourseDialog::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // Header
    QLabel *titleLabel = new QLabel(m_isEdit ? "Update Course" : "Register Course");
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50; margin-bottom: 10px;");
    mainLayout->addWidget(titleLabel);

    // Form
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(15);
    formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_nameEdit = new QLineEdit();
    m_nameEdit->setPlaceholderText("Course Name");
    m_nameEdit->setStyleSheet("QLineEdit { padding: 8px; border: 1px solid #bdc3c7; border-radius: 4px; } "
                              "QLineEdit:focus { border: 2px solid #3498db; }");

    m_yearEdit = new QSpinBox();
    m_yearEdit->setRange(1, 6);
    m_yearEdit->setStyleSheet("QSpinBox { padding: 8px; border: 1px solid #bdc3c7; border-radius: 4px; }");
    
    m_hoursEdit = new QSpinBox();
    m_hoursEdit->setRange(1, 10);
    m_hoursEdit->setStyleSheet("QSpinBox { padding: 8px; border: 1px solid #bdc3c7; border-radius: 4px; }");

    // Pre-fill if edit
    if (m_isEdit) {
        m_nameEdit->setText(m_course.name);
        m_yearEdit->setValue(m_course.year);
        m_hoursEdit->setValue(m_course.hours);
    }

    formLayout->addRow(new QLabel("Course Name:"), m_nameEdit);
    formLayout->addRow(new QLabel("Year:"), m_yearEdit);
    formLayout->addRow(new QLabel("Credit Hours:"), m_hoursEdit);

    mainLayout->addLayout(formLayout);

    // Buttons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();

    m_cancelBtn = new QPushButton("Cancel");
    m_cancelBtn->setCursor(Qt::PointingHandCursor);
    m_cancelBtn->setStyleSheet("QPushButton { background-color: #95a5a6; color: white; border: none; padding: 10px 20px; border-radius: 4px; font-weight: bold; } "
                               "QPushButton:hover { background-color: #7f8c8d; }");
    
    m_saveBtn = new QPushButton(m_isEdit ? "Save" : "Add");
    m_saveBtn->setCursor(Qt::PointingHandCursor);
    m_saveBtn->setStyleSheet("QPushButton { background-color: #3498db; color: white; border: none; padding: 10px 20px; border-radius: 4px; font-weight: bold; } "
                             "QPushButton:hover { background-color: #2980b9; }");

    btnLayout->addWidget(m_cancelBtn);
    btnLayout->addWidget(m_saveBtn);

    mainLayout->addLayout(btnLayout);

    connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_saveBtn, &QPushButton::clicked, this, &CourseDialog::onSave);
}

void CourseDialog::onSave()
{
    if (m_nameEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Course Name cannot be empty.");
        return;
    }

    m_course.name = m_nameEdit->text();
    m_course.year = m_yearEdit->value();
    m_course.hours = m_hoursEdit->value();

    accept();
}

Course CourseDialog::getCourse() const
{
    return m_course;
}
