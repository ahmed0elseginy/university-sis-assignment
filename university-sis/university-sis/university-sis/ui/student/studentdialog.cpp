#include "studentdialog.h"
#include "../../modules/student/studentrepository.h"

StudentDialog::StudentDialog(QWidget *parent, const Student* student)
    : QDialog(parent), m_isEdit(student != nullptr)
{
    if (student) {
        m_student = *student;
    } else {
        m_student = Student();
    }
    
    setWindowTitle(m_isEdit ? "Edit Student" : "Add New Student");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setMinimumWidth(400);
    
    setupUi();
}

void StudentDialog::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // Header
    QLabel *titleLabel = new QLabel(m_isEdit ? "Update Student" : "Register Student");
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50; margin-bottom: 10px;");
    mainLayout->addWidget(titleLabel);

    // Form
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(15);
    formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    auto createInput = [](const QString &placeholder) {
        QLineEdit *edit = new QLineEdit();
        edit->setPlaceholderText(placeholder);
        // Minimal styling, let ThemeManager handle colors
        edit->setStyleSheet("QLineEdit { padding: 8px; border: 1px solid #bdc3c7; border-radius: 4px; } "
                            "QLineEdit:focus { border: 2px solid #3498db; }");
        return edit;
    };

    m_nameEdit = createInput("Full Name");
    m_departmentEdit = createInput("Department (e.g. CS, EE)");

    m_yearEdit = new QSpinBox();
    m_yearEdit->setRange(1, 6);
    m_yearEdit->setStyleSheet("QSpinBox { padding: 8px; border: 1px solid #bdc3c7; border-radius: 4px; }");
    
    m_sectionEdit = new QSpinBox();
    m_sectionEdit->setRange(0, 9999);
    m_sectionEdit->setStyleSheet("QSpinBox { padding: 8px; border: 1px solid #bdc3c7; border-radius: 4px; }");

    // Pre-fill if edit
    m_userEdit = createInput("Username");
    m_passEdit = createInput("Password");
    if (m_isEdit) m_passEdit->setPlaceholderText("Fill to change password");

    // Pre-fill if edit
    if (m_isEdit) {
        m_nameEdit->setText(m_student.name);
        m_yearEdit->setValue(m_student.year);
        m_departmentEdit->setText(m_student.department);
        m_sectionEdit->setValue(m_student.sectionId);
        m_userEdit->setText(m_student.username);
        // Password usually left blank on edit for security/UX unless changing
    }

    formLayout->addRow(new QLabel("Name:"), m_nameEdit);
    formLayout->addRow(new QLabel("Year:"), m_yearEdit);
    formLayout->addRow(new QLabel("Department:"), m_departmentEdit);
    formLayout->addRow(new QLabel("Department:"), m_departmentEdit);
    formLayout->addRow(new QLabel("Section ID:"), m_sectionEdit);
    formLayout->addRow(new QLabel("Username:"), m_userEdit);
    formLayout->addRow(new QLabel("Password:"), m_passEdit);

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
    connect(m_saveBtn, &QPushButton::clicked, this, &StudentDialog::onSave);

    // Styling (handled by ThemeManager)
    // setStyleSheet("QDialog { background-color: #ffffff; } QLabel { font-size: 14px; color: #34495e; }");
    // Ensure labels have correct font size
    QList<QLabel*> labels = findChildren<QLabel*>();
    for(auto label : labels) {
        QFont f = label->font();
        f.setPointSize(10); // Approx 14px
        label->setFont(f);
    }
}

void StudentDialog::onSave()
{
    if (m_nameEdit->text().isEmpty() || m_departmentEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Name and Department cannot be empty.");
        return;
    }

    m_student.name = m_nameEdit->text();
    m_student.year = m_yearEdit->value();
    m_student.department = m_departmentEdit->text();
    m_student.name = m_nameEdit->text();
    m_student.year = m_yearEdit->value();
    m_student.department = m_departmentEdit->text();
    m_student.sectionId = m_sectionEdit->value();
    m_student.username = m_userEdit->text();
    if (!m_passEdit->text().isEmpty()) {
         m_student.password = m_passEdit->text();
    }

    accept();
}

Student StudentDialog::getStudent() const
{
    return m_student;
}
