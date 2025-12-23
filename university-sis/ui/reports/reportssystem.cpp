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
    m_exportBtn = new QPushButton("Export to File", this);
    buttonLayout->addWidget(m_generateBtn);
    buttonLayout->addWidget(m_exportBtn);
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
    
    // Connections
    connect(m_reportTypeCombo, &QComboBox::currentIndexChanged, this, &ReportsSystem::onReportTypeChanged);
    connect(m_generateBtn, &QPushButton::clicked, this, &ReportsSystem::onGenerateReport);
    connect(m_exportBtn, &QPushButton::clicked, this, &ReportsSystem::onExportReport);
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
    headers << "Student ID" << "Name" << "Email" << "Major" << "Enrollment Date" << "Courses Enrolled";
    m_reportModel->setHorizontalHeaderLabels(headers);
    
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT s.id, s.first_name, s.last_name, s.email, s.major, s.enrollment_date, "
                 "COUNT(e.id) as course_count "
                 "FROM students s "
                 "LEFT JOIN enrollments e ON s.id = e.student_id "
                 "WHERE s.enrollment_date BETWEEN :start_date AND :end_date "
                 "GROUP BY s.id "
                 "ORDER BY s.id");
    query.bindValue(":start_date", m_startDateEdit->date().toString("yyyy-MM-dd"));
    query.bindValue(":end_date", m_endDateEdit->date().toString("yyyy-MM-dd"));
    
    int totalStudents = 0;
    if (query.exec()) {
        while (query.next()) {
            int row = m_reportModel->rowCount();
            m_reportModel->insertRow(row);
            m_reportModel->setItem(row, 0, new QStandardItem(query.value("id").toString()));
            m_reportModel->setItem(row, 1, new QStandardItem(query.value("first_name").toString() + " " + query.value("last_name").toString()));
            m_reportModel->setItem(row, 2, new QStandardItem(query.value("email").toString()));
            m_reportModel->setItem(row, 3, new QStandardItem(query.value("major").toString()));
            m_reportModel->setItem(row, 4, new QStandardItem(query.value("enrollment_date").toDate().toString("MMM dd, yyyy")));
            m_reportModel->setItem(row, 5, new QStandardItem(query.value("course_count").toString()));
            totalStudents++;
        }
    }
    
    m_reportText->setPlainText(QString("Student Enrollment Report\n"
                                       "Period: %1 to %2\n"
                                       "Total Students: %3\n"
                                       "Generated: %4")
                               .arg(m_startDateEdit->date().toString("MMM dd, yyyy"))
                               .arg(m_endDateEdit->date().toString("MMM dd, yyyy"))
                               .arg(totalStudents)
                               .arg(QDate::currentDate().toString("MMM dd, yyyy")));
}

void ReportsSystem::generateCourseReport()
{
    m_reportModel->clear();
    QStringList headers;
    headers << "Course Code" << "Course Name" << "Credits" << "Students Enrolled" << "Average Grade";
    m_reportModel->setHorizontalHeaderLabels(headers);
    
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT c.course_code, c.course_name, c.credits, "
                 "COUNT(DISTINCT e.student_id) as enrolled, "
                 "AVG(CASE WHEN g.max_score > 0 THEN (g.score / g.max_score) * 100 ELSE NULL END) as avg_grade "
                 "FROM courses c "
                 "LEFT JOIN enrollments e ON c.id = e.course_id "
                 "LEFT JOIN grades g ON e.id = g.enrollment_id "
                 "GROUP BY c.id "
                 "ORDER BY c.course_code");
    
    int totalCourses = 0;
    if (query.exec()) {
        while (query.next()) {
            int row = m_reportModel->rowCount();
            m_reportModel->insertRow(row);
            m_reportModel->setItem(row, 0, new QStandardItem(query.value("course_code").toString()));
            m_reportModel->setItem(row, 1, new QStandardItem(query.value("course_name").toString()));
            m_reportModel->setItem(row, 2, new QStandardItem(query.value("credits").toString()));
            m_reportModel->setItem(row, 3, new QStandardItem(query.value("enrolled").toString()));
            double avgGrade = query.value("avg_grade").toDouble();
            m_reportModel->setItem(row, 4, new QStandardItem(avgGrade > 0 ? QString::number(avgGrade, 'f', 1) + "%" : "N/A"));
            totalCourses++;
        }
    }
    
    m_reportText->setPlainText(QString("Course Statistics Report\n"
                                       "Total Courses: %1\n"
                                       "Generated: %2")
                               .arg(totalCourses)
                               .arg(QDate::currentDate().toString("MMM dd, yyyy")));
}

void ReportsSystem::generateAttendanceReport()
{
    m_reportModel->clear();
    QStringList headers;
    headers << "Date" << "Student" << "Course" << "Status" << "Remarks";
    m_reportModel->setHorizontalHeaderLabels(headers);
    
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT a.date, s.first_name, s.last_name, c.course_code, a.status, a.remarks "
                 "FROM attendance a "
                 "JOIN students s ON a.student_id = s.id "
                 "JOIN courses c ON a.course_id = c.id "
                 "WHERE a.date BETWEEN :start_date AND :end_date "
                 "ORDER BY a.date DESC");
    query.bindValue(":start_date", m_startDateEdit->date().toString("yyyy-MM-dd"));
    query.bindValue(":end_date", m_endDateEdit->date().toString("yyyy-MM-dd"));
    
    int totalRecords = 0;
    int presentCount = 0;
    if (query.exec()) {
        while (query.next()) {
            int row = m_reportModel->rowCount();
            m_reportModel->insertRow(row);
            m_reportModel->setItem(row, 0, new QStandardItem(query.value("date").toDate().toString("MMM dd, yyyy")));
            m_reportModel->setItem(row, 1, new QStandardItem(query.value("first_name").toString() + " " + query.value("last_name").toString()));
            m_reportModel->setItem(row, 2, new QStandardItem(query.value("course_code").toString()));
            QString status = query.value("status").toString();
            m_reportModel->setItem(row, 3, new QStandardItem(status));
            m_reportModel->setItem(row, 4, new QStandardItem(query.value("remarks").toString()));
            totalRecords++;
            if (status == "Present") presentCount++;
        }
    }
    
    double attendanceRate = totalRecords > 0 ? (presentCount * 100.0 / totalRecords) : 0;
    m_reportText->setPlainText(QString("Attendance Summary Report\n"
                                       "Period: %1 to %2\n"
                                       "Total Records: %3\n"
                                       "Present: %4\n"
                                       "Attendance Rate: %5%\n"
                                       "Generated: %6")
                               .arg(m_startDateEdit->date().toString("MMM dd, yyyy"))
                               .arg(m_endDateEdit->date().toString("MMM dd, yyyy"))
                               .arg(totalRecords)
                               .arg(presentCount)
                               .arg(QString::number(attendanceRate, 'f', 1))
                               .arg(QDate::currentDate().toString("MMM dd, yyyy")));
}

void ReportsSystem::generateFinancialReport()
{
    m_reportModel->clear();
    QStringList headers;
    headers << "Student" << "Payment Date" << "Amount" << "Status" << "Description";
    m_reportModel->setHorizontalHeaderLabels(headers);
    
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT s.first_name, s.last_name, p.payment_date, p.amount, p.status, p.description "
                 "FROM payments p "
                 "JOIN students s ON p.student_id = s.id "
                 "WHERE p.payment_date BETWEEN :start_date AND :end_date "
                 "ORDER BY p.payment_date DESC");
    query.bindValue(":start_date", m_startDateEdit->date().toString("yyyy-MM-dd"));
    query.bindValue(":end_date", m_endDateEdit->date().toString("yyyy-MM-dd"));
    
    int totalPayments = 0;
    double totalAmount = 0;
    if (query.exec()) {
        while (query.next()) {
            int row = m_reportModel->rowCount();
            m_reportModel->insertRow(row);
            m_reportModel->setItem(row, 0, new QStandardItem(query.value("first_name").toString() + " " + query.value("last_name").toString()));
            m_reportModel->setItem(row, 1, new QStandardItem(query.value("payment_date").toDate().toString("MMM dd, yyyy")));
            double amount = query.value("amount").toDouble();
            m_reportModel->setItem(row, 2, new QStandardItem("$" + QString::number(amount, 'f', 2)));
            m_reportModel->setItem(row, 3, new QStandardItem(query.value("status").toString()));
            m_reportModel->setItem(row, 4, new QStandardItem(query.value("description").toString()));
            totalPayments++;
            totalAmount += amount;
        }
    }
    
    m_reportText->setPlainText(QString("Financial Summary Report\n"
                                       "Period: %1 to %2\n"
                                       "Total Payments: %3\n"
                                       "Total Amount: $%4\n"
                                       "Generated: %5")
                               .arg(m_startDateEdit->date().toString("MMM dd, yyyy"))
                               .arg(m_endDateEdit->date().toString("MMM dd, yyyy"))
                               .arg(totalPayments)
                               .arg(QString::number(totalAmount, 'f', 2))
                               .arg(QDate::currentDate().toString("MMM dd, yyyy")));
}

void ReportsSystem::onExportReport()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Export Report", "", "CSV Files (*.csv);;Text Files (*.txt)");
    if (fileName.isEmpty()) return;
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Failed to open file for writing.");
        return;
    }
    
    QTextStream out(&file);
    
    // Write header
    for (int col = 0; col < m_reportModel->columnCount(); ++col) {
        if (col > 0) out << ",";
        out << m_reportModel->headerData(col, Qt::Horizontal).toString();
    }
    out << "\n";
    
    // Write data
    for (int row = 0; row < m_reportModel->rowCount(); ++row) {
        for (int col = 0; col < m_reportModel->columnCount(); ++col) {
            if (col > 0) out << ",";
            QStandardItem *item = m_reportModel->item(row, col);
            if (item) {
                out << "\"" << item->text().replace("\"", "\"\"") << "\"";
            }
        }
        out << "\n";
    }
    
    // Write summary
    out << "\n" << m_reportText->toPlainText();
    
    file.close();
    QMessageBox::information(this, "Success", "Report exported successfully.");
}

