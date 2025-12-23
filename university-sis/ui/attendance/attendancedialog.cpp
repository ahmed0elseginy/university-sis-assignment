#include "attendancedialog.h"
#include "../../modules/academic/section.h"
#include "../../modules/academic/course.h"
#include "../../modules/student/student.h"
#include <QMap>
#include <QHeaderView>
#include <QStandardItem>
#include <QAbstractItemView>

AttendanceDialog::AttendanceDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Record Attendance");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setMinimumWidth(600);
    setMinimumHeight(500);
    
    setupUi();
    loadSections();
}

void AttendanceDialog::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // Header
    QLabel *titleLabel = new QLabel("Student Attendance Recording");
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50; margin-bottom: 10px;");
    mainLayout->addWidget(titleLabel);

    // Form for Section and Date selection
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(15);
    formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_sectionEdit = new QComboBox();
    m_sectionEdit->setStyleSheet("QComboBox { padding: 8px; border: 1px solid #bdc3c7; border-radius: 4px; }");
    
    m_courseLabel = new QLabel("(Select a section)");
    m_courseLabel->setStyleSheet("color: #7f8c8d; font-style: italic;");
    
    m_dateEdit = new QDateEdit();
    m_dateEdit->setCalendarPopup(true);
    m_dateEdit->setDate(QDate::currentDate());
    m_dateEdit->setStyleSheet("QDateEdit { padding: 8px; border: 1px solid #bdc3c7; border-radius: 4px; }");

    formLayout->addRow(new QLabel("Section:"), m_sectionEdit);
    formLayout->addRow(new QLabel("Course:"), m_courseLabel);
    formLayout->addRow(new QLabel("Date:"), m_dateEdit);

    mainLayout->addLayout(formLayout);
    
    // Load Students Button
    QHBoxLayout *buttonLayout1 = new QHBoxLayout();
    buttonLayout1->addStretch();
    m_loadBtn = new QPushButton("Load Students");
    m_loadBtn->setCursor(Qt::PointingHandCursor);
    m_loadBtn->setStyleSheet("QPushButton { background-color: #3498db; color: white; border: none; padding: 10px 20px; border-radius: 4px; font-weight: bold; } "
                             "QPushButton:hover { background-color: #2980b9; }");
    buttonLayout1->addWidget(m_loadBtn);
    mainLayout->addLayout(buttonLayout1);

    // Students Table
    m_studentsTable = new QTableView(this);
    m_studentsModel = new QStandardItemModel(this);
    m_studentsModel->setColumnCount(3);
    m_studentsModel->setHorizontalHeaderLabels({"ID", "Student Name", "Status"});
    
    m_studentsTable->setModel(m_studentsModel);
    m_studentsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_studentsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_studentsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_studentsTable->verticalHeader()->setVisible(false);
    m_studentsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_studentsTable->setAlternatingRowColors(true);
    m_studentsTable->setStyleSheet("QTableView::item { padding: 5px; }");
    
    mainLayout->addWidget(m_studentsTable);

    // Buttons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();

    m_cancelBtn = new QPushButton("Cancel");
    m_cancelBtn->setCursor(Qt::PointingHandCursor);
    m_cancelBtn->setStyleSheet("QPushButton { background-color: #95a5a6; color: white; border: none; padding: 10px 20px; border-radius: 4px; font-weight: bold; } "
                               "QPushButton:hover { background-color: #7f8c8d; }");
    
    m_saveBtn = new QPushButton("Save");
    m_saveBtn->setCursor(Qt::PointingHandCursor);
    m_saveBtn->setStyleSheet("QPushButton { background-color: #27ae60; color: white; border: none; padding: 10px 20px; border-radius: 4px; font-weight: bold; } "
                             "QPushButton:hover { background-color: #229954; }");
    m_saveBtn->setEnabled(false);

    btnLayout->addWidget(m_cancelBtn);
    btnLayout->addWidget(m_saveBtn);

    mainLayout->addLayout(btnLayout);

    connect(m_sectionEdit, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AttendanceDialog::onSectionChanged);
    connect(m_loadBtn, &QPushButton::clicked, this, &AttendanceDialog::onLoadStudents);
    connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_saveBtn, &QPushButton::clicked, this, &AttendanceDialog::onSave);
}

void AttendanceDialog::loadSections()
{
    m_sectionEdit->clear();
    auto sections = m_sectionRepo.getAllSections();
    auto courses = m_courseRepo.getAllCourses();
    
    // Create a map of course IDs to course names for quick lookup
    QMap<int, QString> courseMap;
    for (const auto& course : courses) {
        courseMap[course.id] = course.name;
    }
    
    // Populate section combo box with section info and course name
    for (const auto& section : sections) {
        QString courseName = courseMap.value(section.courseId, QString("Course %1").arg(section.courseId));
        QString displayText = QString("Section %1 - %2").arg(section.id).arg(courseName);
        m_sectionEdit->addItem(displayText, section.id);  // Store section ID as user data
        // Also store course_id in additional data
        m_sectionEdit->setItemData(m_sectionEdit->count() - 1, section.courseId, Qt::UserRole + 1);
    }
}

void AttendanceDialog::onSectionChanged(int index)
{
    if (index >= 0 && index < m_sectionEdit->count()) {
        m_currentSectionId = m_sectionEdit->itemData(index).toInt();
        m_currentCourseId = m_sectionEdit->itemData(index, Qt::UserRole + 1).toInt();
        
        auto courseOpt = m_courseRepo.getCourseById(m_currentCourseId);
        if (courseOpt) {
            m_courseLabel->setText(QString("%1 (ID: %2)").arg(courseOpt->name).arg(m_currentCourseId));
            m_courseLabel->setStyleSheet("color: #2c3e50; font-weight: 500;");
        } else {
            m_courseLabel->setText(QString("Course ID: %1").arg(m_currentCourseId));
            m_courseLabel->setStyleSheet("color: #7f8c8d;");
        }
    } else {
        m_currentSectionId = 0;
        m_currentCourseId = 0;
        m_courseLabel->setText("(Select a section)");
        m_courseLabel->setStyleSheet("color: #7f8c8d; font-style: italic;");
    }
    
    // Clear students table when section changes
    m_studentsModel->removeRows(0, m_studentsModel->rowCount());
    m_saveBtn->setEnabled(false);
}

void AttendanceDialog::onLoadStudents()
{
    if (m_currentSectionId == 0) {
        QMessageBox::warning(this, "Warning", "Please select a section first.");
        return;
    }
    
    m_selectedDate = m_dateEdit->date();
    loadStudentsForSection(m_currentSectionId);
}

void AttendanceDialog::loadStudentsForSection(int sectionId)
{
    m_studentsModel->removeRows(0, m_studentsModel->rowCount());
    
    // Get all student IDs enrolled in this section
    std::vector<int> studentIds = m_enrollmentRepo.getStudentIdsBySection(sectionId);
    
    if (studentIds.empty()) {
        QMessageBox::information(this, "Info", "No students enrolled in this section.");
        m_saveBtn->setEnabled(false);
        return;
    }
    
    // Load student details and create table rows
    for (int studentId : studentIds) {
        auto studentOpt = m_studentRepo.getStudentById(studentId);
        if (!studentOpt) continue;
        
        Student student = *studentOpt;
        
        // Create row items
        QList<QStandardItem*> row;
        QStandardItem *idItem = new QStandardItem(QString::number(student.id));
        idItem->setData(student.id, Qt::UserRole); // Store ID for later retrieval
        row << idItem;
        
        row << new QStandardItem(student.name);
        
        // Create combo box for status
        QStandardItem *statusItem = new QStandardItem("Present");
        statusItem->setData("Present", Qt::UserRole); // Store status value
        row << statusItem;
        
        m_studentsModel->appendRow(row);
        
        // Create combo box widget for status selection
        QComboBox *statusCombo = new QComboBox();
        statusCombo->addItems({"Present", "Absent", "Late", "Excused"});
        statusCombo->setCurrentText("Present");
        statusCombo->setStyleSheet("QComboBox { padding: 5px; border: 1px solid #bdc3c7; border-radius: 3px; }");
        
        // Store student ID in combo box property for retrieval
        statusCombo->setProperty("studentId", student.id);
        
        int rowIndex = m_studentsModel->rowCount() - 1;
        m_studentsTable->setIndexWidget(m_studentsModel->index(rowIndex, 2), statusCombo);
    }
    
    m_saveBtn->setEnabled(true);
}

void AttendanceDialog::onSave()
{
    if (m_currentSectionId == 0 || m_currentCourseId == 0) {
        QMessageBox::warning(this, "Validation Error", "Please select a section and load students first.");
        return;
    }
    
    m_selectedDate = m_dateEdit->date();
    accept();
}

std::vector<Attendance> AttendanceDialog::getAttendanceRecords() const
{
    std::vector<Attendance> records;
    
    if (m_currentSectionId == 0 || m_currentCourseId == 0) {
        return records;
    }
    
    QDate selectedDate = m_dateEdit->date(); // Use current date from date edit
    
    // Iterate through all rows in the table
    for (int row = 0; row < m_studentsModel->rowCount(); ++row) {
        // Get student ID from first column
        int studentId = m_studentsModel->item(row, 0)->data(Qt::UserRole).toInt();
        
        // Get status from combo box widget in third column
        QWidget *widget = m_studentsTable->indexWidget(m_studentsModel->index(row, 2));
        QComboBox *statusCombo = qobject_cast<QComboBox*>(widget);
        
        if (statusCombo && studentId > 0) {
            Attendance att;
            att.studentId = studentId;
            att.sectionId = m_currentSectionId;
            att.courseId = m_currentCourseId;
            att.date = selectedDate;
            att.status = statusCombo->currentText();
            
            records.push_back(att);
        }
    }
    
    return records;
}
