#include "sectiondialog.h"

SectionDialog::SectionDialog(QWidget *parent, const Section* section)
    : QDialog(parent)
{
    QString title = section ? "Edit Section" : "Create Section";
    setWindowTitle(title);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setMinimumWidth(300);
    setupUi();
    
    if (section) {
        m_section = *section;
        // Find index for courseId
        int idx = m_courseCombo->findData(section->courseId);
        if(idx >= 0) m_courseCombo->setCurrentIndex(idx);
        
        m_maxEdit->setValue(section->maxStudents);
        m_saveBtn->setText("Update");
    }
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

    m_courseCombo = new QComboBox();
    m_courseCombo->setStyleSheet("QComboBox { padding: 8px; border: 1px solid #bdc3c7; border-radius: 4px; }");
    
    // Load Courses
    auto courses = m_courseRepo.getAllCourses();
    for(const auto& c : courses) {
        m_courseCombo->addItem(c.name + " (" + QString::number(c.year) + ")", c.id);
    }
    
    m_maxEdit = new QSpinBox();
    m_maxEdit->setRange(1, 1000);
    m_maxEdit->setValue(30);
    m_maxEdit->setStyleSheet("QSpinBox { padding: 8px; border: 1px solid #bdc3c7; border-radius: 4px; }");

    formLayout->addRow(new QLabel("Select Course:"), m_courseCombo);
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
    m_section.courseId = m_courseCombo->currentData().toInt();
    m_section.maxStudents = m_maxEdit->value();
    accept();
}

Section SectionDialog::getSection() const
{
    return m_section;
}
