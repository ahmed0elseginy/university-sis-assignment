#include "studentportal.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QDate>
#include <QDateTime>
#include <QScrollBar>
#include <QFrame>
#include <QSplitter>
#include <QGraphicsDropShadowEffect>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QColor>
#include <QStringList>

StudentPortal::StudentPortal(QWidget *parent) : QWidget(parent)
{
    setupUi();
    loadStudentData();
}

void StudentPortal::setUserContext(const QString& role, int userId)
{
    m_currentUserRole = role;
    m_currentUserId = userId;
    loadStudentData();
}

void StudentPortal::setupUi()
{
    // Main layout
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Create scroll area for the dashboard
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setStyleSheet(
        "QScrollArea { background: #F5F5F7; border: none; }"
        "QScrollBar:vertical { background: #E5E5EA; width: 10px; border-radius: 5px; }"
        "QScrollBar::handle:vertical { background: #C7C7CC; border-radius: 5px; min-height: 20px; }"
        "QScrollBar::handle:vertical:hover { background: #AEAEB2; }"
    );
    
    m_contentWidget = new QWidget();
    m_mainLayout = new QVBoxLayout(m_contentWidget);
    m_mainLayout->setContentsMargins(30, 30, 30, 30);
    m_mainLayout->setSpacing(25);
    
    m_scrollArea->setWidget(m_contentWidget);
    mainLayout->addWidget(m_scrollArea);
    
    // Header
    auto headerWidget = new QWidget();
    headerWidget->setStyleSheet("background: transparent;");
    auto headerLayout = new QVBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 10);
    
    auto welcomeLabel = new QLabel("Welcome to Your Student Portal");
    welcomeLabel->setStyleSheet("font-size: 32px; font-weight: 700; color: #1D1D1F; background: transparent;");
    headerLayout->addWidget(welcomeLabel);
    
    auto subtitleLabel = new QLabel("View your academic information, grades, attendance, and more");
    subtitleLabel->setStyleSheet("font-size: 16px; color: #8E8E93; font-weight: 400; background: transparent; margin-top: 5px;");
    headerLayout->addWidget(subtitleLabel);
    
    // Refresh button
    auto refreshBtn = new QPushButton("🔄 Refresh");
    refreshBtn->setCursor(Qt::PointingHandCursor);
    refreshBtn->setFixedSize(120, 40);
    refreshBtn->setStyleSheet(
        "QPushButton { "
        "   background-color: #007AFF; "
        "   color: white; "
        "   border: none; "
        "   border-radius: 8px; "
        "   font-weight: 600; "
        "   font-size: 14px; "
        "}"
        "QPushButton:hover { background-color: #0051D5; }"
        "QPushButton:pressed { background-color: #0040AA; }"
    );
    connect(refreshBtn, &QPushButton::clicked, this, &StudentPortal::onRefresh);
    
    auto headerRow = new QHBoxLayout();
    headerRow->addWidget(headerWidget);
    headerRow->addStretch();
    headerRow->addWidget(refreshBtn);
    
    m_mainLayout->addLayout(headerRow);
}

void StudentPortal::loadStudentData()
{
    // Clear existing content (except header)
    QLayoutItem* item;
    while ((item = m_mainLayout->takeAt(1)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    
    if (m_currentUserId == -1) {
        auto noDataLabel = new QLabel("Please log in to view your student portal.");
        noDataLabel->setStyleSheet("font-size: 18px; color: #8E8E93; padding: 40px;");
        noDataLabel->setAlignment(Qt::AlignCenter);
        m_mainLayout->addWidget(noDataLabel);
        return;
    }
    
    // Create dashboard cards
    createProfileCard();
    createQuickStatsCard();
    
    // Two column layout for main content
    auto contentRow = new QHBoxLayout();
    contentRow->setSpacing(20);
    
    auto leftColumn = new QVBoxLayout();
    leftColumn->setSpacing(20);
    
    auto rightColumn = new QVBoxLayout();
    rightColumn->setSpacing(20);
    
    // Left column - Create cards with tables
    createCoursesCard();
    leftColumn->addWidget(m_coursesTable->parentWidget());
    
    createGradesCard();
    leftColumn->addWidget(m_gradesTable->parentWidget());
    
    // Right column
    createAttendanceCard();
    rightColumn->addWidget(m_attendanceTable->parentWidget());
    
    createFinancialCard();
    rightColumn->addWidget(m_paymentsTable->parentWidget());
    
    rightColumn->addWidget(createUpcomingEventsCard());
    
    contentRow->addLayout(leftColumn, 2);
    contentRow->addLayout(rightColumn, 1);
    
    m_mainLayout->addLayout(contentRow);
    
    m_mainLayout->addStretch();
}

QWidget* StudentPortal::createCard(const QString& title, QWidget* content)
{
    auto card = new QWidget();
    card->setStyleSheet(
        "QWidget { "
        "   background: white; "
        "   border-radius: 12px; "
        "   padding: 20px; "
        "}"
    );
    
    auto layout = new QVBoxLayout(card);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);
    
    auto titleLabel = new QLabel(title);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: 700; color: #1D1D1F; background: transparent;");
    layout->addWidget(titleLabel);
    
    if (content) {
        layout->addWidget(content);
    }
    
    return card;
}

void StudentPortal::createProfileCard()
{
    auto studentInfo = getStudentInfo();
    
    auto profileCard = new QWidget();
    profileCard->setStyleSheet(
        "QWidget { "
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #007AFF, stop:1 #0051D5); "
        "   border-radius: 16px; "
        "   padding: 30px; "
        "}"
        "QLabel { background: transparent; color: white; }"
    );
    
    auto layout = new QHBoxLayout(profileCard);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(30);
    
    // Left side - Profile info
    auto infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(12);
    
    m_profileName = new QLabel(studentInfo.name.isEmpty() ? "Student" : studentInfo.name);
    m_profileName->setStyleSheet("font-size: 28px; font-weight: 700; color: white; background: transparent;");
    infoLayout->addWidget(m_profileName);
    
    auto idLabel = new QLabel(QString("Student ID: %1").arg(studentInfo.id > 0 ? QString::number(studentInfo.id) : "N/A"));
    idLabel->setStyleSheet("font-size: 16px; color: rgba(255,255,255,0.9); background: transparent;");
    infoLayout->addWidget(idLabel);
    
    auto deptLabel = new QLabel(QString("Department: %1").arg(studentInfo.department.isEmpty() ? "N/A" : studentInfo.department));
    deptLabel->setStyleSheet("font-size: 16px; color: rgba(255,255,255,0.9); background: transparent;");
    infoLayout->addWidget(deptLabel);
    
    auto yearLabel = new QLabel(QString("Year: %1").arg(studentInfo.year > 0 ? QString::number(studentInfo.year) : "N/A"));
    yearLabel->setStyleSheet("font-size: 16px; color: rgba(255,255,255,0.9); background: transparent;");
    infoLayout->addWidget(yearLabel);
    
    layout->addLayout(infoLayout);
    layout->addStretch();
    
    // Right side - GPA
    auto gpaLayout = new QVBoxLayout();
    gpaLayout->setAlignment(Qt::AlignRight | Qt::AlignTop);
    
    auto gpaTitle = new QLabel("Current GPA");
    gpaTitle->setStyleSheet("font-size: 14px; color: rgba(255,255,255,0.8); background: transparent;");
    gpaTitle->setAlignment(Qt::AlignRight);
    gpaLayout->addWidget(gpaTitle);
    
    double gpa = calculateGPA();
    m_gpaLabel = new QLabel(QString::number(gpa, 'f', 2));
    m_gpaLabel->setStyleSheet("font-size: 48px; font-weight: 700; color: white; background: transparent;");
    m_gpaLabel->setAlignment(Qt::AlignRight);
    gpaLayout->addWidget(m_gpaLabel);
    
    if (gpa == 0.0) {
        m_gpaLabel->setText("--");
        auto noGradesLabel = new QLabel("No grades available");
        noGradesLabel->setStyleSheet("font-size: 12px; color: rgba(255,255,255,0.7); background: transparent;");
        noGradesLabel->setAlignment(Qt::AlignRight);
        gpaLayout->addWidget(noGradesLabel);
    }
    
    layout->addLayout(gpaLayout);
    
    m_mainLayout->addWidget(profileCard);
}

void StudentPortal::createQuickStatsCard()
{
    auto statsCard = new QWidget();
    statsCard->setStyleSheet("QWidget { background: white; border-radius: 12px; padding: 20px; }");
    
    auto layout = new QHBoxLayout(statsCard);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(20);
    
    auto courses = getStudentCourses();
    auto attendance = getStudentAttendance();
    auto payments = getStudentPayments();
    
    int totalCourses = courses.size();
    int pendingPayments = 0;
    double totalPending = 0.0;
    for (const auto& p : payments) {
        if (p.status == "Pending" || p.status == "Overdue") {
            pendingPayments++;
            totalPending += p.amount;
        }
    }
    
    double avgAttendance = 0.0;
    if (!attendance.isEmpty()) {
        double sum = 0.0;
        for (const auto& a : attendance) {
            sum += a.attendanceRate;
        }
        avgAttendance = sum / attendance.size();
    }
    
    layout->addWidget(createStatLabel("Enrolled Courses", QString::number(totalCourses), "#007AFF"));
    layout->addWidget(createStatLabel("Avg. Attendance", QString::number(avgAttendance, 'f', 1) + "%", "#34C759"));
    layout->addWidget(createStatLabel("Pending Payments", QString::number(pendingPayments), pendingPayments > 0 ? "#FF3B30" : "#34C759"));
    if (pendingPayments > 0) {
        layout->addWidget(createStatLabel("Amount Due", "$" + QString::number(totalPending, 'f', 2), "#FF3B30"));
    }
    
    m_mainLayout->addWidget(statsCard);
}

QWidget* StudentPortal::createStatLabel(const QString& label, const QString& value, const QString& color)
{
    auto widget = new QWidget();
    widget->setStyleSheet("QWidget { background: transparent; }");
    auto layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);
    
    auto valueLabel = new QLabel(value);
    valueLabel->setStyleSheet(QString("font-size: 24px; font-weight: 700; color: %1; background: transparent;").arg(color));
    valueLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(valueLabel);
    
    auto labelLabel = new QLabel(label);
    labelLabel->setStyleSheet("font-size: 13px; color: #8E8E93; background: transparent;");
    labelLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(labelLabel);
    
    return widget;
}

void StudentPortal::createCoursesCard()
{
    auto courses = getStudentCourses();
    
    m_coursesTable = new QTableWidget();
    m_coursesTable->setColumnCount(4);
    QStringList courseHeaders;
    courseHeaders << "Course ID" << "Course Name" << "Section" << "Hours";
    m_coursesTable->setHorizontalHeaderLabels(courseHeaders);
    m_coursesTable->setRowCount(courses.size());
    
    for (int i = 0; i < courses.size(); ++i) {
        const auto& course = courses[i];
        m_coursesTable->setItem(i, 0, new QTableWidgetItem(QString::number(course.courseId)));
        m_coursesTable->setItem(i, 1, new QTableWidgetItem(course.courseName));
        m_coursesTable->setItem(i, 2, new QTableWidgetItem(QString::number(course.sectionId)));
        m_coursesTable->setItem(i, 3, new QTableWidgetItem(QString::number(course.hours)));
    }
    
    m_coursesTable->horizontalHeader()->setStretchLastSection(true);
    m_coursesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_coursesTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_coursesTable->verticalHeader()->setVisible(false);
    m_coursesTable->setAlternatingRowColors(true);
    m_coursesTable->setShowGrid(false);
    m_coursesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_coursesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_coursesTable->setStyleSheet(
        "QTableWidget { border: none; background: white; }"
        "QTableWidget::item { padding: 10px; border-bottom: 1px solid #E5E5EA; }"
        "QHeaderView::section { background: #F5F5F7; padding: 12px; font-weight: 600; border: none; }"
    );
    
    if (courses.isEmpty()) {
        m_coursesTable->setRowCount(1);
        auto item = new QTableWidgetItem("No courses enrolled");
        item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
        m_coursesTable->setItem(0, 0, item);
        // Fill all columns with the same item
        const int numCols = 4;
        for (int col = 1; col < numCols; ++col) {
            auto emptyItem = new QTableWidgetItem("");
            emptyItem->setFlags(emptyItem->flags() & ~Qt::ItemIsEnabled);
            m_coursesTable->setItem(0, col, emptyItem);
        }
        const int rowSpan = 1;
        const int colSpan = 4;
        m_coursesTable->setSpan(0, 0, rowSpan, colSpan);
    }
    
    auto card = createCard("My Courses", m_coursesTable);
    m_coursesTable->setParent(card);
}

void StudentPortal::createGradesCard()
{
    auto grades = getStudentGrades();
    
    m_gradesTable = new QTableWidget();
    m_gradesTable->setColumnCount(5);
    QStringList gradeHeaders;
    gradeHeaders << "Course" << "Assignment 1" << "Assignment 2" << "Final Exam" << "Total";
    m_gradesTable->setHorizontalHeaderLabels(gradeHeaders);
    m_gradesTable->setRowCount(grades.size());
    
    for (int i = 0; i < grades.size(); ++i) {
        const auto& grade = grades[i];
        m_gradesTable->setItem(i, 0, new QTableWidgetItem(grade.courseName));
        m_gradesTable->setItem(i, 1, new QTableWidgetItem(grade.a1.isEmpty() ? "--" : grade.a1));
        m_gradesTable->setItem(i, 2, new QTableWidgetItem(grade.a2.isEmpty() ? "--" : grade.a2));
        m_gradesTable->setItem(i, 3, new QTableWidgetItem(grade.finalExam.isEmpty() ? "--" : grade.finalExam));
        
        auto totalItem = new QTableWidgetItem(grade.total.isEmpty() ? "--" : grade.total);
        if (!grade.total.isEmpty()) {
            bool ok;
            double totalValue = grade.total.toDouble(&ok);
            if (ok) {
                QString color = totalValue >= 90 ? "#34C759" : (totalValue >= 70 ? "#FF9500" : "#FF3B30");
                totalItem->setForeground(QColor(color));
            }
        }
        m_gradesTable->setItem(i, 4, totalItem);
    }
    
    m_gradesTable->horizontalHeader()->setStretchLastSection(true);
    m_gradesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_gradesTable->verticalHeader()->setVisible(false);
    m_gradesTable->setAlternatingRowColors(true);
    m_gradesTable->setShowGrid(false);
    m_gradesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_gradesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_gradesTable->setStyleSheet(
        "QTableWidget { border: none; background: white; }"
        "QTableWidget::item { padding: 10px; border-bottom: 1px solid #E5E5EA; }"
        "QHeaderView::section { background: #F5F5F7; padding: 12px; font-weight: 600; border: none; }"
    );
    
    if (grades.isEmpty()) {
        m_gradesTable->setRowCount(1);
        auto item = new QTableWidgetItem("No grades available");
        item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
        m_gradesTable->setItem(0, 0, item);
        // Fill all columns with empty items for spanning
        const int numCols = 5;
        for (int col = 1; col < numCols; ++col) {
            auto emptyItem = new QTableWidgetItem("");
            emptyItem->setFlags(emptyItem->flags() & ~Qt::ItemIsEnabled);
            m_gradesTable->setItem(0, col, emptyItem);
        }
        const int rowSpan = 1;
        const int colSpan = 5;
        m_gradesTable->setSpan(0, 0, rowSpan, colSpan);
    }
    
    auto card = createCard("My Grades", m_gradesTable);
    m_gradesTable->setParent(card);
}

void StudentPortal::createAttendanceCard()
{
    auto attendance = getStudentAttendance();
    
    m_attendanceTable = new QTableWidget();
    m_attendanceTable->setColumnCount(5);
    QStringList attendanceHeaders;
    attendanceHeaders << "Course" << "Present" << "Absent" << "Late" << "Rate";
    m_attendanceTable->setHorizontalHeaderLabels(attendanceHeaders);
    m_attendanceTable->setRowCount(attendance.size());
    
    for (int i = 0; i < attendance.size(); ++i) {
        const auto& att = attendance[i];
        m_attendanceTable->setItem(i, 0, new QTableWidgetItem(att.courseName));
        m_attendanceTable->setItem(i, 1, new QTableWidgetItem(QString::number(att.present)));
        m_attendanceTable->setItem(i, 2, new QTableWidgetItem(QString::number(att.absent)));
        m_attendanceTable->setItem(i, 3, new QTableWidgetItem(QString::number(att.late)));
        
        auto rateItem = new QTableWidgetItem(QString::number(att.attendanceRate, 'f', 1) + "%");
        QString color = att.attendanceRate >= 80 ? "#34C759" : (att.attendanceRate >= 60 ? "#FF9500" : "#FF3B30");
        rateItem->setForeground(QColor(color));
        m_attendanceTable->setItem(i, 4, rateItem);
    }
    
    m_attendanceTable->horizontalHeader()->setStretchLastSection(true);
    m_attendanceTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_attendanceTable->verticalHeader()->setVisible(false);
    m_attendanceTable->setAlternatingRowColors(true);
    m_attendanceTable->setShowGrid(false);
    m_attendanceTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_attendanceTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_attendanceTable->setStyleSheet(
        "QTableWidget { border: none; background: white; }"
        "QTableWidget::item { padding: 10px; border-bottom: 1px solid #E5E5EA; }"
        "QHeaderView::section { background: #F5F5F7; padding: 12px; font-weight: 600; border: none; }"
    );
    
    if (attendance.isEmpty()) {
        m_attendanceTable->setRowCount(1);
        auto item = new QTableWidgetItem("No attendance records");
        item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
        m_attendanceTable->setItem(0, 0, item);
        // Fill all columns with empty items for spanning
        const int numCols = 5;
        for (int col = 1; col < numCols; ++col) {
            auto emptyItem = new QTableWidgetItem("");
            emptyItem->setFlags(emptyItem->flags() & ~Qt::ItemIsEnabled);
            m_attendanceTable->setItem(0, col, emptyItem);
        }
        const int rowSpan = 1;
        const int colSpan = 5;
        m_attendanceTable->setSpan(0, 0, rowSpan, colSpan);
    }
    
    auto card = createCard("Attendance", m_attendanceTable);
    m_attendanceTable->setParent(card);
}

void StudentPortal::createFinancialCard()
{
    auto payments = getStudentPayments();
    
    m_paymentsTable = new QTableWidget();
    m_paymentsTable->setColumnCount(4);
    QStringList paymentHeaders;
    paymentHeaders << "Description" << "Amount" << "Status" << "Date";
    m_paymentsTable->setHorizontalHeaderLabels(paymentHeaders);
    m_paymentsTable->setRowCount(payments.size());
    
    for (int i = 0; i < payments.size(); ++i) {
        const auto& payment = payments[i];
        m_paymentsTable->setItem(i, 0, new QTableWidgetItem(payment.description));
        m_paymentsTable->setItem(i, 1, new QTableWidgetItem("$" + QString::number(payment.amount, 'f', 2)));
        
        auto statusItem = new QTableWidgetItem(payment.status);
        QString color = payment.status == "Paid" ? "#34C759" : (payment.status == "Pending" ? "#FF9500" : "#FF3B30");
        statusItem->setForeground(QColor(color));
        m_paymentsTable->setItem(i, 2, statusItem);
        
        m_paymentsTable->setItem(i, 3, new QTableWidgetItem(payment.date.toString("MMM dd, yyyy")));
    }
    
    m_paymentsTable->horizontalHeader()->setStretchLastSection(true);
    m_paymentsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_paymentsTable->verticalHeader()->setVisible(false);
    m_paymentsTable->setAlternatingRowColors(true);
    m_paymentsTable->setShowGrid(false);
    m_paymentsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_paymentsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_paymentsTable->setStyleSheet(
        "QTableWidget { border: none; background: white; }"
        "QTableWidget::item { padding: 10px; border-bottom: 1px solid #E5E5EA; }"
        "QHeaderView::section { background: #F5F5F7; padding: 12px; font-weight: 600; border: none; }"
    );
    
    if (payments.isEmpty()) {
        m_paymentsTable->setRowCount(1);
        auto item = new QTableWidgetItem("No payment records");
        item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
        m_paymentsTable->setItem(0, 0, item);
        // Fill all columns with empty items for spanning
        const int numCols = 4;
        for (int col = 1; col < numCols; ++col) {
            auto emptyItem = new QTableWidgetItem("");
            emptyItem->setFlags(emptyItem->flags() & ~Qt::ItemIsEnabled);
            m_paymentsTable->setItem(0, col, emptyItem);
        }
        const int rowSpan = 1;
        const int colSpan = 4;
        m_paymentsTable->setSpan(0, 0, rowSpan, colSpan);
    }
    
    auto card = createCard("Financial Status", m_paymentsTable);
    m_paymentsTable->setParent(card);
}

QWidget* StudentPortal::createUpcomingEventsCard()
{
    int eventCount = getUpcomingEventsCount();
    
    auto eventsWidget = new QWidget();
    auto layout = new QVBoxLayout(eventsWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);
    
    if (eventCount > 0) {
        auto countLabel = new QLabel(QString("You have %1 upcoming event(s)").arg(eventCount));
        countLabel->setStyleSheet("font-size: 16px; color: #1D1D1F; background: transparent;");
        layout->addWidget(countLabel);
        
        auto viewBtn = new QPushButton("View Calendar");
        viewBtn->setCursor(Qt::PointingHandCursor);
        viewBtn->setStyleSheet(
            "QPushButton { "
            "   background-color: #007AFF; "
            "   color: white; "
            "   border: none; "
            "   border-radius: 8px; "
            "   padding: 10px; "
            "   font-weight: 600; "
            "}"
            "QPushButton:hover { background-color: #0051D5; }"
        );
        layout->addWidget(viewBtn);
    } else {
        auto noEventsLabel = new QLabel("No upcoming events");
        noEventsLabel->setStyleSheet("font-size: 16px; color: #8E8E93; background: transparent;");
        noEventsLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(noEventsLabel);
    }
    
    return createCard("Upcoming Events", eventsWidget);
}

// Data fetching methods
StudentInfo StudentPortal::getStudentInfo()
{
    StudentInfo info;
    auto studentOpt = m_repo.getStudentById(m_currentUserId);
    if (studentOpt) {
        info.id = studentOpt->id;
        info.name = studentOpt->name;
        info.year = studentOpt->year;
        info.department = studentOpt->department;
        info.sectionId = studentOpt->sectionId;
        info.username = studentOpt->username;
    }
    return info;
}

QList<CourseInfo> StudentPortal::getStudentCourses()
{
    QList<CourseInfo> courses;
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    
    query.prepare(
        "SELECT DISTINCT c.course_id, c.name, ss.section_id, c.hours "
        "FROM student_section ss "
        "JOIN sections s ON ss.section_id = s.section_id "
        "JOIN courses c ON s.course_id = c.course_id "
        "WHERE ss.student_id = :studentId"
    );
    query.bindValue(":studentId", m_currentUserId);
    
    if (query.exec()) {
        while (query.next()) {
            CourseInfo course;
            course.courseId = query.value(0).toInt();
            course.courseName = query.value(1).toString();
            course.sectionId = query.value(2).toInt();
            course.hours = query.value(3).toInt();
            courses.append(course);
        }
    }
    
    return courses;
}

QList<GradeInfo> StudentPortal::getStudentGrades()
{
    QList<GradeInfo> grades;
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    
    query.prepare(
        "SELECT g.course_id, c.name, g.a1, g.a2, g.final_exam, g.total "
        "FROM grades g "
        "JOIN courses c ON g.course_id = c.course_id "
        "WHERE g.student_id = :studentId"
    );
    query.bindValue(":studentId", m_currentUserId);
    
    if (query.exec()) {
        while (query.next()) {
            GradeInfo grade;
            grade.courseId = query.value(0).toInt();
            grade.courseName = query.value(1).toString();
            grade.a1 = query.value(2).toString();
            grade.a2 = query.value(3).toString();
            grade.finalExam = query.value(4).toString();
            grade.total = query.value(5).toString();
            grades.append(grade);
        }
    }
    
    return grades;
}

QList<AttendanceInfo> StudentPortal::getStudentAttendance()
{
    QList<AttendanceInfo> attendance;
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    
    query.prepare(
        "SELECT a.course_id, c.name, "
        "SUM(CASE WHEN a.status = 'Present' THEN 1 ELSE 0 END) as present, "
        "SUM(CASE WHEN a.status = 'Absent' THEN 1 ELSE 0 END) as absent, "
        "SUM(CASE WHEN a.status = 'Late' THEN 1 ELSE 0 END) as late "
        "FROM attendance a "
        "JOIN courses c ON a.course_id = c.course_id "
        "WHERE a.student_id = :studentId "
        "GROUP BY a.course_id, c.name"
    );
    query.bindValue(":studentId", m_currentUserId);
    
    if (query.exec()) {
        while (query.next()) {
            AttendanceInfo att;
            att.courseId = query.value(0).toInt();
            att.courseName = query.value(1).toString();
            att.present = query.value(2).toInt();
            att.absent = query.value(3).toInt();
            att.late = query.value(4).toInt();
            
            int total = att.present + att.absent + att.late;
            att.attendanceRate = total > 0 ? (att.present * 100.0 / total) : 0.0;
            
            attendance.append(att);
        }
    }
    
    return attendance;
}

QList<PaymentInfo> StudentPortal::getStudentPayments()
{
    QList<PaymentInfo> payments;
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    
    query.prepare(
        "SELECT payment_id, amount, description, status, date "
        "FROM payments "
        "WHERE student_id = :studentId "
        "ORDER BY date DESC"
    );
    query.bindValue(":studentId", m_currentUserId);
    
    if (query.exec()) {
        while (query.next()) {
            PaymentInfo payment;
            payment.paymentId = query.value(0).toInt();
            payment.amount = query.value(1).toDouble();
            payment.description = query.value(2).toString();
            payment.status = query.value(3).toString();
            payment.date = query.value(4).toDate();
            payments.append(payment);
        }
    }
    
    return payments;
}

double StudentPortal::calculateGPA()
{
    auto grades = getStudentGrades();
    if (grades.isEmpty()) {
        return 0.0;
    }
    
    double totalPoints = 0.0;
    int count = 0;
    
    for (const auto& grade : grades) {
        if (!grade.total.isEmpty()) {
            bool ok;
            double value = grade.total.toDouble(&ok);
            if (ok && value >= 0 && value <= 100) {
                // Convert percentage to GPA (4.0 scale)
                double gpa;
                if (value >= 90) gpa = 4.0;
                else if (value >= 80) gpa = 3.0;
                else if (value >= 70) gpa = 2.0;
                else if (value >= 60) gpa = 1.0;
                else gpa = 0.0;
                
                totalPoints += gpa;
                count++;
            }
        }
    }
    
    return count > 0 ? totalPoints / count : 0.0;
}

int StudentPortal::getUpcomingEventsCount()
{
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    
    QDate today = QDate::currentDate();
    query.prepare(
        "SELECT COUNT(*) FROM calendar_events "
        "WHERE date >= :today"
    );
    query.bindValue(":today", today);
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    
    return 0;
}

void StudentPortal::refreshData()
{
    loadStudentData();
}

void StudentPortal::onRefresh()
{
    loadStudentData();
}

void StudentPortal::applyCardStyle(QWidget* card)
{
    card->setStyleSheet(
        "QWidget { "
        "   background: white; "
        "   border-radius: 12px; "
        "   padding: 20px; "
        "}"
    );
}
