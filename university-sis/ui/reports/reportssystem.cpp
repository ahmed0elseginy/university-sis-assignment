#include "reportssystem.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QFileDialog>
#include <QTextStream>
#include <QStringList>
#include "../../database/databasemanager.h"

ReportsSystem::ReportsSystem(QWidget *parent)
    : BaseSystemWidget("Reports & Analytics", parent)
{
    setupUi();
}

void ReportsSystem::setupUi()
{
    auto mainLayout = qobject_cast<QVBoxLayout*>(layout());
    if (!mainLayout) return;
    
    // Report Controls
    auto controlsGroup = new QGroupBox("Report Options");
    auto controlsLayout = new QVBoxLayout(controlsGroup);
    
    auto typeLayout = new QHBoxLayout();
    typeLayout->addWidget(new QLabel("Report Type:"));
    m_reportTypeCombo = new QComboBox(this);
    m_reportTypeCombo->addItems({"Student Enrollment", "Course Statistics", "Attendance Summary", "Financial Summary"});
    typeLayout->addWidget(m_reportTypeCombo);
    typeLayout->addStretch();
    controlsLayout->addLayout(typeLayout);
    
    auto dateLayout = new QHBoxLayout();
    dateLayout->addWidget(new QLabel("Start Date:"));
    m_startDateEdit = new QDateEdit(this);
    m_startDateEdit->setDate(QDate::currentDate().addMonths(-1));
    m_startDateEdit->setCalendarPopup(true);
    m_startDateEdit->setDisplayFormat("MMM dd, yyyy");
    dateLayout->addWidget(m_startDateEdit);
    dateLayout->addWidget(new QLabel("End Date:"));
    m_endDateEdit = new QDateEdit(this);
    m_endDateEdit->setDate(QDate::currentDate());
    m_endDateEdit->setCalendarPopup(true);
    m_endDateEdit->setDisplayFormat("MMM dd, yyyy");
    dateLayout->addWidget(m_endDateEdit);
    dateLayout->addStretch();
    controlsLayout->addLayout(dateLayout);
    
    auto buttonLayout = new QHBoxLayout();
    m_generateBtn = new QPushButton("Generate Report", this);
    m_generateBtn->setProperty("type", "primary");
    m_exportBtn = new QPushButton("Export to CSV", this);
    m_printBtn = new QPushButton("Print", this);
    buttonLayout->addWidget(m_generateBtn);
    buttonLayout->addWidget(m_exportBtn);
    buttonLayout->addWidget(m_printBtn);
    buttonLayout->addStretch();
    controlsLayout->addLayout(buttonLayout);
    
    mainLayout->addWidget(controlsGroup);
    
    // Report Display
    auto displayGroup = new QGroupBox("Report Results");
    auto displayLayout = new QVBoxLayout(displayGroup);
    
    m_reportTable = new QTableView(this);
    m_reportModel = new QStandardItemModel(0, 0, this);
    m_reportTable->setModel(m_reportModel);
    m_reportTable->horizontalHeader()->setStretchLastSection(true);
    m_reportTable->verticalHeader()->setVisible(false);
    m_reportTable->setAlternatingRowColors(true);
    displayLayout->addWidget(m_reportTable, 2);
    
    m_reportText = new QTextEdit(this);
    m_reportText->setReadOnly(true);
    m_reportText->setMaximumHeight(150);
    displayLayout->addWidget(m_reportText, 1);
    
    mainLayout->addWidget(displayGroup, 1);
    
    // Improve table display
    m_reportTable->setSortingEnabled(true);
    m_reportTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    m_reportTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_reportTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    // Connections
    connect(m_reportTypeCombo, &QComboBox::currentIndexChanged, this, &ReportsSystem::onReportTypeChanged);
    connect(m_generateBtn, &QPushButton::clicked, this, &ReportsSystem::onGenerateReport);
    connect(m_exportBtn, &QPushButton::clicked, this, &ReportsSystem::onExportReport);
    connect(m_printBtn, &QPushButton::clicked, this, [this]() {
        if (m_reportModel->rowCount() > 0) {
            QMessageBox::information(this, "Print", 
                "Print functionality: Please use Export to CSV and print from your preferred application.");
        } else {
            QMessageBox::information(this, "Print", "Please generate a report first.");
        }
    });
}

void ReportsSystem::onReportTypeChanged(int index)
{
    Q_UNUSED(index);
    m_reportModel->clear();
    m_reportText->clear();
}

void ReportsSystem::onGenerateReport()
{
    int reportType = m_reportTypeCombo->currentIndex();
    
    switch (reportType) {
        case 0: generateStudentReport(); break;
        case 1: generateCourseReport(); break;
        case 2: generateAttendanceReport(); break;
        case 3: generateFinancialReport(); break;
    }
}

void ReportsSystem::generateStudentReport()
{
    m_reportModel->clear();
    QStringList headers;
    headers << "Student ID" << "Name" << "Year" << "Department" << "Section ID" << "Courses Enrolled";
    m_reportModel->setHorizontalHeaderLabels(headers);
    
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT s.student_id, s.name, s.year, s.department, s.section_id, "
                 "COUNT(DISTINCT ss.section_id) as course_count "
                 "FROM students s "
                 "LEFT JOIN student_section ss ON s.student_id = ss.student_id "
                 "GROUP BY s.student_id "
                 "ORDER BY s.name");
    
    int totalStudents = 0;
    int totalEnrollments = 0;
    if (query.exec()) {
        while (query.next()) {
            int row = m_reportModel->rowCount();
            m_reportModel->insertRow(row);
            m_reportModel->setItem(row, 0, new QStandardItem(query.value("student_id").toString()));
            m_reportModel->setItem(row, 1, new QStandardItem(query.value("name").toString()));
            m_reportModel->setItem(row, 2, new QStandardItem(query.value("year").toString()));
            m_reportModel->setItem(row, 3, new QStandardItem(query.value("department").toString()));
            m_reportModel->setItem(row, 4, new QStandardItem(query.value("section_id").toString()));
            int courseCount = query.value("course_count").toInt();
            m_reportModel->setItem(row, 5, new QStandardItem(QString::number(courseCount)));
            totalStudents++;
            totalEnrollments += courseCount;
        }
    }
    
    double avgCourses = totalStudents > 0 ? (totalEnrollments * 1.0 / totalStudents) : 0;
    m_reportText->setPlainText(QString("Student Enrollment Report\n"
                                       "Total Students: %1\n"
                                       "Total Course Enrollments: %2\n"
                                       "Average Courses per Student: %3\n"
                                       "Generated: %4")
                               .arg(totalStudents)
                               .arg(totalEnrollments)
                               .arg(QString::number(avgCourses, 'f', 1))
                               .arg(QDate::currentDate().toString("MMM dd, yyyy")));
}

void ReportsSystem::generateCourseReport()
{
    m_reportModel->clear();
    QStringList headers;
    headers << "Course ID" << "Course Name" << "Year" << "Hours" << "Sections" << "Students Enrolled" << "Avg Grade";
    m_reportModel->setHorizontalHeaderLabels(headers);
    
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT c.course_id, c.name as course_name, c.year, c.hours, "
                 "COUNT(DISTINCT sec.section_id) as section_count, "
                 "COUNT(DISTINCT ss.student_id) as enrolled, "
                 "AVG(CASE WHEN g.final_exam = 'A' THEN 95 "
                 "          WHEN g.final_exam = 'A-' THEN 90 "
                 "          WHEN g.final_exam = 'B+' THEN 87 "
                 "          WHEN g.final_exam = 'B' THEN 83 "
                 "          WHEN g.final_exam = 'B-' THEN 80 "
                 "          WHEN g.final_exam = 'C+' THEN 77 "
                 "          WHEN g.final_exam = 'C' THEN 73 "
                 "          WHEN g.final_exam = 'C-' THEN 70 "
                 "          WHEN g.final_exam = 'D+' THEN 67 "
                 "          WHEN g.final_exam = 'D' THEN 63 "
                 "          WHEN g.final_exam = 'D-' THEN 60 "
                 "          WHEN g.final_exam = 'F' THEN 50 "
                 "          ELSE NULL END) as avg_grade "
                 "FROM courses c "
                 "LEFT JOIN sections sec ON c.course_id = sec.course_id "
                 "LEFT JOIN student_section ss ON sec.section_id = ss.section_id "
                 "LEFT JOIN grades g ON (ss.student_id = g.student_id AND c.course_id = g.course_id) "
                 "GROUP BY c.course_id "
                 "ORDER BY c.year, c.name");
    
    int totalCourses = 0;
    int totalEnrollments = 0;
    if (query.exec()) {
        while (query.next()) {
            int row = m_reportModel->rowCount();
            m_reportModel->insertRow(row);
            m_reportModel->setItem(row, 0, new QStandardItem(query.value("course_id").toString()));
            m_reportModel->setItem(row, 1, new QStandardItem(query.value("course_name").toString()));
            m_reportModel->setItem(row, 2, new QStandardItem(query.value("year").toString()));
            m_reportModel->setItem(row, 3, new QStandardItem(query.value("hours").toString()));
            m_reportModel->setItem(row, 4, new QStandardItem(query.value("section_count").toString()));
            int enrolled = query.value("enrolled").toInt();
            m_reportModel->setItem(row, 5, new QStandardItem(QString::number(enrolled)));
            double avgGrade = query.value("avg_grade").toDouble();
            auto gradeItem = new QStandardItem(avgGrade > 0 ? QString::number(avgGrade, 'f', 1) : "N/A");
            if (avgGrade >= 90) gradeItem->setForeground(QBrush(QColor("#27ae60")));
            else if (avgGrade >= 70) gradeItem->setForeground(QBrush(QColor("#f39c12")));
            else if (avgGrade > 0) gradeItem->setForeground(QBrush(QColor("#c0392b")));
            m_reportModel->setItem(row, 6, gradeItem);
            totalCourses++;
            totalEnrollments += enrolled;
        }
    }
    
    m_reportText->setPlainText(QString("Course Statistics Report\n"
                                       "Total Courses: %1\n"
                                       "Total Enrollments: %2\n"
                                       "Average Enrollments per Course: %3\n"
                                       "Generated: %4")
                               .arg(totalCourses)
                               .arg(totalEnrollments)
                               .arg(totalCourses > 0 ? QString::number(totalEnrollments * 1.0 / totalCourses, 'f', 1) : "0")
                               .arg(QDate::currentDate().toString("MMM dd, yyyy")));
}

void ReportsSystem::generateAttendanceReport()
{
    m_reportModel->clear();
    QStringList headers;
    headers << "Date" << "Student Name" << "Student ID" << "Course Name" << "Status";
    m_reportModel->setHorizontalHeaderLabels(headers);
    
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT a.date, s.name as student_name, s.student_id, c.name as course_name, a.status "
                 "FROM attendance a "
                 "LEFT JOIN students s ON a.student_id = s.student_id "
                 "LEFT JOIN courses c ON a.course_id = c.course_id "
                 "WHERE a.date BETWEEN :start_date AND :end_date "
                 "ORDER BY a.date DESC, s.name");
    query.bindValue(":start_date", m_startDateEdit->date().toString("yyyy-MM-dd"));
    query.bindValue(":end_date", m_endDateEdit->date().toString("yyyy-MM-dd"));
    
    int totalRecords = 0;
    int presentCount = 0;
    int absentCount = 0;
    int lateCount = 0;
    if (query.exec()) {
        while (query.next()) {
            int row = m_reportModel->rowCount();
            m_reportModel->insertRow(row);
            m_reportModel->setItem(row, 0, new QStandardItem(query.value("date").toDate().toString("MMM dd, yyyy")));
            m_reportModel->setItem(row, 1, new QStandardItem(query.value("student_name").toString()));
            m_reportModel->setItem(row, 2, new QStandardItem(query.value("student_id").toString()));
            m_reportModel->setItem(row, 3, new QStandardItem(query.value("course_name").toString()));
            QString status = query.value("status").toString();
            auto statusItem = new QStandardItem(status);
            if (status == "Present") {
                statusItem->setForeground(QBrush(QColor("#27ae60")));
                presentCount++;
            } else if (status == "Absent") {
                statusItem->setForeground(QBrush(QColor("#c0392b")));
                absentCount++;
            } else if (status == "Late") {
                statusItem->setForeground(QBrush(QColor("#f39c12")));
                lateCount++;
            }
            m_reportModel->setItem(row, 4, statusItem);
            totalRecords++;
        }
    }
    
    double attendanceRate = totalRecords > 0 ? (presentCount * 100.0 / totalRecords) : 0;
    m_reportText->setPlainText(QString("Attendance Summary Report\n"
                                       "Period: %1 to %2\n"
                                       "Total Records: %3\n"
                                       "Present: %4\n"
                                       "Absent: %5\n"
                                       "Late: %6\n"
                                       "Attendance Rate: %7%\n"
                                       "Generated: %8")
                               .arg(m_startDateEdit->date().toString("MMM dd, yyyy"))
                               .arg(m_endDateEdit->date().toString("MMM dd, yyyy"))
                               .arg(totalRecords)
                               .arg(presentCount)
                               .arg(absentCount)
                               .arg(lateCount)
                               .arg(QString::number(attendanceRate, 'f', 1))
                               .arg(QDate::currentDate().toString("MMM dd, yyyy")));
}

void ReportsSystem::generateFinancialReport()
{
    m_reportModel->clear();
    QStringList headers;
    headers << "Payment ID" << "Student Name" << "Student ID" << "Date" << "Amount" << "Status" << "Description";
    m_reportModel->setHorizontalHeaderLabels(headers);
    
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT p.payment_id, s.name as student_name, s.student_id, p.date, p.amount, p.status, p.description "
                 "FROM payments p "
                 "LEFT JOIN students s ON p.student_id = s.student_id "
                 "WHERE p.date BETWEEN :start_date AND :end_date "
                 "ORDER BY p.date DESC");
    query.bindValue(":start_date", m_startDateEdit->date().toString("yyyy-MM-dd"));
    query.bindValue(":end_date", m_endDateEdit->date().toString("yyyy-MM-dd"));
    
    int totalPayments = 0;
    double totalAmount = 0;
    double paidAmount = 0;
    double pendingAmount = 0;
    double overdueAmount = 0;
    int paidCount = 0;
    int pendingCount = 0;
    int overdueCount = 0;
    
    if (query.exec()) {
        while (query.next()) {
            int row = m_reportModel->rowCount();
            m_reportModel->insertRow(row);
            m_reportModel->setItem(row, 0, new QStandardItem(query.value("payment_id").toString()));
            m_reportModel->setItem(row, 1, new QStandardItem(query.value("student_name").toString()));
            m_reportModel->setItem(row, 2, new QStandardItem(query.value("student_id").toString()));
            m_reportModel->setItem(row, 3, new QStandardItem(query.value("date").toDate().toString("MMM dd, yyyy")));
            double amount = query.value("amount").toDouble();
            auto amountItem = new QStandardItem(QString("$%1").arg(amount, 0, 'f', 2));
            amountItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            m_reportModel->setItem(row, 4, amountItem);
            QString status = query.value("status").toString();
            auto statusItem = new QStandardItem(status);
            if (status == "Paid") {
                statusItem->setForeground(QBrush(QColor("#27ae60")));
                paidAmount += amount;
                paidCount++;
            } else if (status == "Pending") {
                statusItem->setForeground(QBrush(QColor("#f39c12")));
                pendingAmount += amount;
                pendingCount++;
            } else if (status == "Overdue") {
                statusItem->setForeground(QBrush(QColor("#c0392b")));
                overdueAmount += amount;
                overdueCount++;
            }
            m_reportModel->setItem(row, 5, statusItem);
            m_reportModel->setItem(row, 6, new QStandardItem(query.value("description").toString()));
            totalPayments++;
            totalAmount += amount;
        }
    }
    
    m_reportText->setPlainText(QString("Financial Summary Report\n"
                                       "Period: %1 to %2\n"
                                       "Total Payments: %3\n"
                                       "Total Amount: $%4\n"
                                       "Paid: %5 ($%6)\n"
                                       "Pending: %7 ($%8)\n"
                                       "Overdue: %9 ($%10)\n"
                                       "Collection Rate: %11%\n"
                                       "Generated: %12")
                               .arg(m_startDateEdit->date().toString("MMM dd, yyyy"))
                               .arg(m_endDateEdit->date().toString("MMM dd, yyyy"))
                               .arg(totalPayments)
                               .arg(QString::number(totalAmount, 'f', 2))
                               .arg(paidCount)
                               .arg(QString::number(paidAmount, 'f', 2))
                               .arg(pendingCount)
                               .arg(QString::number(pendingAmount, 'f', 2))
                               .arg(overdueCount)
                               .arg(QString::number(overdueAmount, 'f', 2))
                               .arg(totalAmount > 0 ? QString::number((paidAmount / totalAmount) * 100, 'f', 1) : "0")
                               .arg(QDate::currentDate().toString("MMM dd, yyyy")));
}

void ReportsSystem::onExportReport()
{
    if (m_reportModel->rowCount() == 0) {
        QMessageBox::information(this, "Export", "Please generate a report first.");
        return;
    }
    
    QString fileName = QFileDialog::getSaveFileName(this, "Export Report", 
                                                    QString("Report_%1_%2.csv")
                                                    .arg(m_reportTypeCombo->currentText().replace(" ", "_"))
                                                    .arg(QDate::currentDate().toString("yyyyMMdd")), 
                                                    "CSV Files (*.csv);;Text Files (*.txt)");
    if (fileName.isEmpty()) return;
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Failed to open file for writing.");
        return;
    }
    
    QTextStream out(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    out.setEncoding(QStringConverter::Utf8);
#else
    out.setCodec("UTF-8");
#endif
    
    // Write summary header
    out << m_reportText->toPlainText() << "\n\n";
    
    // Write table header
    for (int col = 0; col < m_reportModel->columnCount(); ++col) {
        if (col > 0) out << ",";
        out << "\"" << m_reportModel->headerData(col, Qt::Horizontal).toString().replace("\"", "\"\"") << "\"";
    }
    out << "\n";
    
    // Write data
    for (int row = 0; row < m_reportModel->rowCount(); ++row) {
        for (int col = 0; col < m_reportModel->columnCount(); ++col) {
            if (col > 0) out << ",";
            QStandardItem *item = m_reportModel->item(row, col);
            if (item) {
                QString text = item->text().replace("\"", "\"\"");
                out << "\"" << text << "\"";
            } else {
                out << "\"\"";
            }
        }
        out << "\n";
    }
    
    file.close();
    QMessageBox::information(this, "Success", QString("Report exported successfully to:\n%1").arg(fileName));
}

