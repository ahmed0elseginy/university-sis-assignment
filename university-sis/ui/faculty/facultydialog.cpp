#include "facultydialog.h"

FacultyDialog::FacultyDialog(QWidget *parent, const Faculty* faculty)
    : QDialog(parent), m_isEdit(faculty != nullptr)
{
    if (faculty) {
        m_faculty = *faculty;
    } else {
        m_faculty = Faculty();
    }
    
    setWindowTitle(m_isEdit ? "Edit Faculty" : "Add Faculty Member");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setMinimumWidth(400);
    
    setupUi();
}

void FacultyDialog::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // Header
    QLabel *titleLabel = new QLabel(m_isEdit ? "Update Details" : "New Faculty Member");
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50; margin-bottom: 10px;");
    mainLayout->addWidget(titleLabel);

    // Form
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(15);
    formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_nameEdit = new QLineEdit();
    m_nameEdit->setPlaceholderText("Full Name");
    m_nameEdit->setStyleSheet("QLineEdit { padding: 8px; border: 1px solid #bdc3c7; border-radius: 4px; }");
    
    m_emailEdit = new QLineEdit();
    m_emailEdit->setPlaceholderText("Email Address");
    m_emailEdit->setStyleSheet("QLineEdit { padding: 8px; border: 1px solid #bdc3c7; border-radius: 4px; }");
    
    m_deptEdit = new QLineEdit();
    m_deptEdit->setPlaceholderText("Department (e.g. CS, EE)");
    m_deptEdit->setStyleSheet("QLineEdit { padding: 8px; border: 1px solid #bdc3c7; border-radius: 4px; }");
    
    m_posEdit = new QComboBox();
    m_posEdit->addItems({"Professor", "Associate Professor", "Assistant Professor", "Lecturer", "Teaching Assistant"});
    m_posEdit->setStyleSheet("QComboBox { padding: 8px; border: 1px solid #bdc3c7; border-radius: 4px; }");

    // Pre-fill
    if (m_isEdit) {
        m_nameEdit->setText(m_faculty.name);
        m_emailEdit->setText(m_faculty.email);
        m_deptEdit->setText(m_faculty.department);
        m_posEdit->setCurrentText(m_faculty.position);
    }

    formLayout->addRow(new QLabel("Name:"), m_nameEdit);
    formLayout->addRow(new QLabel("Email:"), m_emailEdit);
    formLayout->addRow(new QLabel("Department:"), m_deptEdit);
    formLayout->addRow(new QLabel("Position:"), m_posEdit);

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
    connect(m_saveBtn, &QPushButton::clicked, this, &FacultyDialog::onSave);
}

void FacultyDialog::onSave()
{
    if (m_nameEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Name cannot be empty.");
        return;
    }

    m_faculty.name = m_nameEdit->text();
    m_faculty.email = m_emailEdit->text();
    m_faculty.department = m_deptEdit->text();
    m_faculty.position = m_posEdit->currentText();

    accept();
}

Faculty FacultyDialog::getFaculty() const
{
    return m_faculty;
}
