#include "attendancesystem.h"
#include "attendancedialog.h"
#include "../../modules/academic/courserepository.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QStandardItem>
#include <QBrush>
#include <QColor>
#include <QFormLayout>
#include <QGroupBox>

AttendanceSystem::AttendanceSystem(QWidget *parent) : QWidget(parent)
{
    setupUi();
    loadCourses();
    loadAttendance();
}

void AttendanceSystem::setupUi()
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // Header
    auto headerLayout = new QHBoxLayout();
    auto title = new QLabel("Attendance Monitoring");
    title->setStyleSheet("font-size: 24px; font-weight: bold;");
    headerLayout->addWidget(title);
    headerLayout->addStretch();
    mainLayout->addLayout(headerLayout);

    // Toolbar
    auto toolbarLayout = new QHBoxLayout();
    m_btnAdd = new QPushButton("Record Attendance");
    m_btnAdd->setCursor(Qt::PointingHandCursor);
    m_btnAdd->setStyleSheet("QPushButton { background-color: #34495e; color: white; padding: 8px 16px; border: none; border-radius: 4px; font-size: 14px; font-weight: 600; } QPushButton:hover { opacity: 0.9; }");
    toolbarLayout->addWidget(m_btnAdd);
    toolbarLayout->addStretch();
    mainLayout->addLayout(toolbarLayout);

    // Filter Section
    auto filterGroup = new QGroupBox("Filters");
    auto filterLayout = new QHBoxLayout(filterGroup);
    filterLayout->setSpacing(10);
    
    // Student Name Filter
    filterLayout->addWidget(new QLabel("Student Name:"));
    m_filterStudentName = new QLineEdit();
    m_filterStudentName->setPlaceholderText("Search by student name...");
    m_filterStudentName->setFixedWidth(200);
    filterLayout->addWidget(m_filterStudentName);
    
    // Course Filter
    filterLayout->addWidget(new QLabel("Course:"));
    m_filterCourse = new QComboBox();
    m_filterCourse->addItem("All Courses", -1);
    m_filterCourse->setFixedWidth(200);
    filterLayout->addWidget(m_filterCourse);
    
    // Status Filter
    filterLayout->addWidget(new QLabel("Status:"));
    m_filterStatus = new QComboBox();
    m_filterStatus->addItem("All Statuses", "");
    m_filterStatus->addItem("Present", "Present");
    m_filterStatus->addItem("Absent", "Absent");
    m_filterStatus->addItem("Late", "Late");
    m_filterStatus->setFixedWidth(120);
    filterLayout->addWidget(m_filterStatus);
    
    // Date From
    filterLayout->addWidget(new QLabel("From:"));
    m_filterDateFrom = new QDateEdit();
    m_filterDateFrom->setCalendarPopup(true);
    m_filterDateFrom->setDate(QDate(2020, 1, 1)); // Set to early date to show all by default
    m_filterDateFrom->setFixedWidth(120);
    filterLayout->addWidget(m_filterDateFrom);
    
    // Date To
    filterLayout->addWidget(new QLabel("To:"));
    m_filterDateTo = new QDateEdit();
    m_filterDateTo->setCalendarPopup(true);
    m_filterDateTo->setDate(QDate(2099, 12, 31)); // Set to future date to show all by default
    m_filterDateTo->setFixedWidth(120);
    filterLayout->addWidget(m_filterDateTo);
    
    // Filter Button
    auto btnFilter = new QPushButton("Apply Filters");
    btnFilter->setCursor(Qt::PointingHandCursor);
    btnFilter->setStyleSheet("QPushButton { background-color: #3498db; color: white; padding: 6px 12px; border: none; border-radius: 4px; } QPushButton:hover { opacity: 0.9; }");
    filterLayout->addWidget(btnFilter);
    
    // Clear Filters Button
    m_btnClearFilters = new QPushButton("Clear");
    m_btnClearFilters->setCursor(Qt::PointingHandCursor);
    m_btnClearFilters->setStyleSheet("QPushButton { background-color: #95a5a6; color: white; padding: 6px 12px; border: none; border-radius: 4px; } QPushButton:hover { opacity: 0.9; }");
    filterLayout->addWidget(m_btnClearFilters);
    
    filterLayout->addStretch();
    mainLayout->addWidget(filterGroup);

    // Table
    m_view = new QTableView(this);
    m_model = new QStandardItemModel(this);
    m_model->setColumnCount(7);
    m_model->setHorizontalHeaderLabels({"ID", "Student Name", "Student ID", "Course Name", "Status", "Date", "Section ID"});
    
    m_view->setModel(m_model);
    styleTable();
    
    mainLayout->addWidget(m_view);

    connect(m_btnAdd, &QPushButton::clicked, this, &AttendanceSystem::onAddAttendance);
    connect(btnFilter, &QPushButton::clicked, this, &AttendanceSystem::applyFilters);
    connect(m_btnClearFilters, &QPushButton::clicked, this, &AttendanceSystem::applyFilters);
    connect(m_filterStudentName, &QLineEdit::returnPressed, this, &AttendanceSystem::applyFilters);
}

void AttendanceSystem::styleTable()
{
    m_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_view->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_view->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_view->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);

    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setAlternatingRowColors(true);
    m_view->verticalHeader()->setVisible(false);
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view->setShowGrid(false);
    m_view->setStyleSheet("QTableView::item { padding: 5px; }");
}

void AttendanceSystem::loadCourses()
{
    CourseRepository courseRepo;
    auto courses = courseRepo.getAllCourses();
    
    for (const auto &course : courses) {
        m_filterCourse->addItem(course.name, course.id);
    }
}

void AttendanceSystem::loadAttendance()
{
    m_model->removeRows(0, m_model->rowCount());
    auto list = m_repo.getAttendanceWithNames();
    
    for (const auto &a : list) {
        QList<QStandardItem*> row;
        row << new QStandardItem(QString::number(a.id));
        row << new QStandardItem(a.studentName.isEmpty() ? "N/A" : a.studentName);
        row << new QStandardItem(QString::number(a.studentId));
        row << new QStandardItem(a.courseName.isEmpty() ? "N/A" : a.courseName);
        
        auto statusItem = new QStandardItem(a.status);
        if (a.status == "Present") statusItem->setForeground(QBrush(QColor("#27ae60")));
        else if (a.status == "Absent") statusItem->setForeground(QBrush(QColor("#c0392b")));
        else statusItem->setForeground(QBrush(QColor("#f39c12")));
        
        row << statusItem;
        row << new QStandardItem(a.date.toString("yyyy-MM-dd"));
        row << new QStandardItem(QString::number(a.sectionId));
        
        m_model->appendRow(row);
    }
}

void AttendanceSystem::applyFilters()
{
    // Check if clear button was clicked
    QPushButton* senderBtn = qobject_cast<QPushButton*>(sender());
    if (senderBtn == m_btnClearFilters) {
        m_filterStudentName->clear();
        m_filterCourse->setCurrentIndex(0);
        m_filterStatus->setCurrentIndex(0);
        m_filterDateFrom->setDate(QDate(2020, 1, 1));
        m_filterDateTo->setDate(QDate(2099, 12, 31));
        // After clearing, reload all data
        loadAttendance();
        return;
    }
    
    // Get filter values
    QString studentName = m_filterStudentName->text().trimmed();
    int courseId = m_filterCourse->currentData().toInt();
    QString status = m_filterStatus->currentData().toString();
    QDate dateFrom = m_filterDateFrom->date();
    QDate dateTo = m_filterDateTo->date();
    
    // Check if date filters are set to default (all dates) - don't filter by date if so
    bool datesAreDefault = (dateFrom == QDate(2020, 1, 1) && dateTo == QDate(2099, 12, 31));
    QDate dateFromFilter = datesAreDefault ? QDate() : dateFrom;
    QDate dateToFilter = datesAreDefault ? QDate() : dateTo;
    
    // If all filters are empty/default, just load all attendance
    if (studentName.isEmpty() && courseId == -1 && status.isEmpty() && datesAreDefault) {
        loadAttendance();
        return;
    }
    
    // Apply filters
    m_model->removeRows(0, m_model->rowCount());
    auto list = m_repo.getFilteredAttendance(
        studentName.isEmpty() ? QString() : studentName,
        courseId,
        status,
        dateFromFilter,
        dateToFilter
    );
    
    for (const auto &a : list) {
        QList<QStandardItem*> row;
        row << new QStandardItem(QString::number(a.id));
        row << new QStandardItem(a.studentName.isEmpty() ? "N/A" : a.studentName);
        row << new QStandardItem(QString::number(a.studentId));
        row << new QStandardItem(a.courseName.isEmpty() ? "N/A" : a.courseName);
        
        auto statusItem = new QStandardItem(a.status);
        if (a.status == "Present") statusItem->setForeground(QBrush(QColor("#27ae60")));
        else if (a.status == "Absent") statusItem->setForeground(QBrush(QColor("#c0392b")));
        else statusItem->setForeground(QBrush(QColor("#f39c12")));
        
        row << statusItem;
        row << new QStandardItem(a.date.toString("yyyy-MM-dd"));
        row << new QStandardItem(QString::number(a.sectionId));
        
        m_model->appendRow(row);
    }
}

void AttendanceSystem::onAddAttendance()
{
    AttendanceDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        std::vector<Attendance> records = dialog.getAttendanceRecords();
        if (!records.empty()) {
            if (m_repo.addMultipleAttendance(records)) {
                refreshData();
                QMessageBox::information(this, "Success", 
                    QString("Successfully recorded attendance for %1 student(s).").arg(records.size()));
            } else {
                QMessageBox::critical(this, "Error", "Failed to record attendance. Please check the data.");
            }
        } else {
            QMessageBox::warning(this, "Warning", "No attendance records to save.");
        }
    }
}

void AttendanceSystem::deleteAttendance(int id)
{
    // Not exposed in UI for now, but method exists
}

void AttendanceSystem::refreshData()
{
    loadAttendance();
}
