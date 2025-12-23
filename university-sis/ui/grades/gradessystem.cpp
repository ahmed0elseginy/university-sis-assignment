#include "gradessystem.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QStringList>
#include <QInputDialog>
#include "../../database/databasemanager.h"

GradesSystem::GradesSystem(QWidget *parent)
    : BaseSystemWidget("Grades & Transcripts", parent)
    , m_userId(0)
{
    setupUi();
}

void GradesSystem::setUserContext(const QString& role, int userId)
{
    m_role = role;
    m_userId = userId;
    loadCourses();
    // Reload student combo if admin/faculty
    if (m_role != "Student" && m_studentCombo) {
        m_studentCombo->clear();
        QSqlQuery studentQuery(DatabaseManager::instance().getDatabase());
        studentQuery.exec("SELECT student_id, name FROM students ORDER BY name");
        while (studentQuery.next()) {
            QString text = studentQuery.value("name").toString() + " (ID: " + studentQuery.value("student_id").toString() + ")";
            m_studentCombo->addItem(text, studentQuery.value("student_id").toInt());
        }
    }
}

void GradesSystem::setupUi()
{
    auto mainLayout = qobject_cast<QVBoxLayout*>(layout());
    if (!mainLayout) return;
    
    // Header Section
    auto headerLayout = new QHBoxLayout();
    auto title = new QLabel("Grades & Transcripts");
    title->setStyleSheet("font-size: 24px; font-weight: bold;");
    headerLayout->addWidget(title);
    headerLayout->addStretch();
    mainLayout->addLayout(headerLayout);
    
    // Course Selection
    auto courseGroup = new QGroupBox("Select Course");
    auto courseLayout = new QHBoxLayout(courseGroup);
    courseLayout->addWidget(new QLabel("Course:"));
    m_courseCombo = new QComboBox(this);
    m_courseCombo->addItem("All Courses");
    connect(m_courseCombo, &QComboBox::currentIndexChanged, this, &GradesSystem::onCourseChanged);
    courseLayout->addWidget(m_courseCombo);
    courseLayout->addStretch();
    
    m_gpaLabel = new QLabel("GPA: --");
    m_gpaLabel->setStyleSheet("font-size: 16px; font-weight: 600; color: #007AFF;");
    courseLayout->addWidget(m_gpaLabel);
    
    mainLayout->addWidget(courseGroup);
    
    // Grades Table
    auto tableGroup = new QGroupBox("Grades");
    auto tableLayout = new QVBoxLayout(tableGroup);
    
    m_gradesTable = new QTableView(this);
    m_gradesModel = new QStandardItemModel(0, 7, this);
    QStringList headers;
    if (m_role == "Student") {
        headers << "Course" << "Component" << "Type" << "Grade" << "Max" << "Percentage" << "Date";
    } else {
        headers << "Student" << "Course" << "A1" << "A2" << "Final Exam" << "Total" << "Actions";
    }
    m_gradesModel->setHorizontalHeaderLabels(headers);
    m_gradesTable->setModel(m_gradesModel);
    m_gradesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_gradesTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_gradesTable->horizontalHeader()->setStretchLastSection(true);
    m_gradesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_gradesTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_gradesTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_gradesTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_gradesTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_gradesTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    m_gradesTable->verticalHeader()->setVisible(false);
    m_gradesTable->setAlternatingRowColors(true);
    m_gradesTable->setShowGrid(false);
    m_gradesTable->setSortingEnabled(true);
    tableLayout->addWidget(m_gradesTable);
    
    mainLayout->addWidget(tableGroup, 2);
    
    // Add Grade Form (simplified)
    auto formGroup = new QGroupBox("Add/Update Grade");
    auto formLayout = new QHBoxLayout(formGroup);
    formLayout->setSpacing(10);
    
    // Student selection (for admin/faculty only)
    if (m_role != "Student") {
        formLayout->addWidget(new QLabel("Student:"));
        m_studentCombo = new QComboBox(this);
        QSqlQuery studentQuery(DatabaseManager::instance().getDatabase());
        studentQuery.exec("SELECT student_id, name FROM students ORDER BY name");
        while (studentQuery.next()) {
            QString text = studentQuery.value("name").toString() + " (ID: " + studentQuery.value("student_id").toString() + ")";
            m_studentCombo->addItem(text, studentQuery.value("student_id").toInt());
        }
        formLayout->addWidget(m_studentCombo);
    } else {
        m_studentCombo = nullptr;
    }
    
    // Course selection
    formLayout->addWidget(new QLabel("Course:"));
    QComboBox *courseFormCombo = new QComboBox(this);
    courseFormCombo->setObjectName("courseFormCombo");
    QSqlQuery courseQuery(DatabaseManager::instance().getDatabase());
    courseQuery.exec("SELECT course_id, name FROM courses ORDER BY name");
    while (courseQuery.next()) {
        QString text = courseQuery.value("name").toString();
        courseFormCombo->addItem(text, courseQuery.value("course_id").toInt());
    }
    formLayout->addWidget(courseFormCombo);
    
    // Assignment type
    formLayout->addWidget(new QLabel("Type:"));
    m_assignmentTypeCombo = new QComboBox(this);
    m_assignmentTypeCombo->addItems({"Assignment 1", "Assignment 2", "Final Exam"});
    formLayout->addWidget(m_assignmentTypeCombo);
    
    // Score (simplified - just percentage)
    formLayout->addWidget(new QLabel("Score (%):"));
    m_scoreSpin = new QDoubleSpinBox(this);
    m_scoreSpin->setRange(0, 100);
    m_scoreSpin->setDecimals(1);
    m_scoreSpin->setSuffix("%");
    m_scoreSpin->setValue(85);
    formLayout->addWidget(m_scoreSpin);
    
    // Max score (hidden, always 100 for percentage)
    m_maxScoreSpin = new QDoubleSpinBox(this);
    m_maxScoreSpin->setVisible(false);
    m_maxScoreSpin->setValue(100);
    
    // Buttons
    m_addBtn = new QPushButton("Save Grade", this);
    m_addBtn->setProperty("type", "primary");
    m_deleteBtn = new QPushButton("Delete", this);
    m_deleteBtn->setProperty("type", "danger");
    formLayout->addWidget(m_addBtn);
    formLayout->addWidget(m_deleteBtn);
    formLayout->addStretch();
    
    mainLayout->addWidget(formGroup);
    
    // Connections
    connect(m_addBtn, &QPushButton::clicked, this, &GradesSystem::onAddGrade);
    connect(m_deleteBtn, &QPushButton::clicked, this, &GradesSystem::onDeleteGrade);
}

void GradesSystem::loadCourses()
{
    m_courseCombo->clear();
    m_courseCombo->addItem("All Courses", -1);
    
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    if (m_role == "Student") {
        query.prepare("SELECT DISTINCT c.course_id, c.name as course_name "
                     "FROM courses c "
                     "JOIN student_section ss ON c.course_id IN (SELECT course_id FROM sections WHERE section_id = ss.section_id) "
                     "WHERE ss.student_id = :student_id "
                     "ORDER BY c.name");
        query.bindValue(":student_id", m_userId);
    } else {
        query.prepare("SELECT course_id, name as course_name FROM courses ORDER BY name");
    }
    
    if (query.exec()) {
        while (query.next()) {
            QString text = query.value("course_name").toString();
            m_courseCombo->addItem(text, query.value("course_id").toInt());
        }
    }
    
    if (m_courseCombo->count() > 1) {
        onCourseChanged(0);
    }
}

void GradesSystem::onCourseChanged(int index)
{
    if (index < 0) return;
    
    int courseId = m_courseCombo->itemData(index).toInt();
    if (courseId == -1) {
        // Show all courses
        refreshGrades();
    } else {
        loadGrades(courseId);
    }
    calculateGPA();
}

void GradesSystem::loadGrades(int courseId)
{
    m_gradesModel->removeRows(0, m_gradesModel->rowCount());
    
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    QString baseQuery = "SELECT g.student_id, s.name as student_name, c.course_id, c.name as course_name, "
                        "g.a1, g.a2, g.final_exam, g.total "
                        "FROM grades g "
                        "LEFT JOIN students s ON g.student_id = s.student_id "
                        "LEFT JOIN courses c ON g.course_id = c.course_id "
                        "WHERE 1=1";
    
    if (m_role == "Student") {
        baseQuery += " AND g.student_id = :student_id";
    }
    
    if (courseId != -1) {
        baseQuery += " AND g.course_id = :course_id";
    }
    
    baseQuery += " ORDER BY c.name, s.name";
    
    query.prepare(baseQuery);
    
    if (m_role == "Student") {
        query.bindValue(":student_id", m_userId);
    }
    if (courseId != -1) {
        query.bindValue(":course_id", courseId);
    }
    
    if (query.exec()) {
        while (query.next()) {
            int row = m_gradesModel->rowCount();
            m_gradesModel->insertRow(row);
            
            QString courseName = query.value("course_name").toString();
            QString studentName = m_role == "Student" ? "" : query.value("student_name").toString();
            QString a1 = query.value("a1").toString();
            QString a2 = query.value("a2").toString();
            QString finalExam = query.value("final_exam").toString();
            QString total = query.value("total").toString();
            
            // Adjust columns based on role
            if (m_role == "Student") {
                m_gradesModel->setItem(row, 0, new QStandardItem(courseName));
                m_gradesModel->setItem(row, 1, new QStandardItem("Assignment 1"));
                m_gradesModel->setItem(row, 2, new QStandardItem("Assignment"));
                m_gradesModel->setItem(row, 3, new QStandardItem(a1.isEmpty() ? "N/A" : a1));
                m_gradesModel->setItem(row, 4, new QStandardItem("--"));
                m_gradesModel->setItem(row, 5, new QStandardItem("--"));
                m_gradesModel->setItem(row, 6, new QStandardItem(""));
                
                // Store student_id and course_id for reference
                m_gradesModel->item(row, 0)->setData(query.value("student_id").toInt(), Qt::UserRole);
                m_gradesModel->item(row, 0)->setData(query.value("course_id").toInt(), Qt::UserRole + 1);
                
                // Add row for Assignment 2
                row = m_gradesModel->rowCount();
                m_gradesModel->insertRow(row);
                m_gradesModel->setItem(row, 0, new QStandardItem(courseName));
                m_gradesModel->setItem(row, 1, new QStandardItem("Assignment 2"));
                m_gradesModel->setItem(row, 2, new QStandardItem("Assignment"));
                m_gradesModel->setItem(row, 3, new QStandardItem(a2.isEmpty() ? "N/A" : a2));
                m_gradesModel->setItem(row, 4, new QStandardItem("--"));
                m_gradesModel->setItem(row, 5, new QStandardItem("--"));
                m_gradesModel->setItem(row, 6, new QStandardItem(""));
                
                // Add row for Final Exam
                row = m_gradesModel->rowCount();
                m_gradesModel->insertRow(row);
                m_gradesModel->setItem(row, 0, new QStandardItem(courseName));
                m_gradesModel->setItem(row, 1, new QStandardItem("Final Exam"));
                m_gradesModel->setItem(row, 2, new QStandardItem("Exam"));
                m_gradesModel->setItem(row, 3, new QStandardItem(finalExam.isEmpty() ? "N/A" : finalExam));
                m_gradesModel->setItem(row, 4, new QStandardItem("--"));
                m_gradesModel->setItem(row, 5, new QStandardItem("--"));
                m_gradesModel->setItem(row, 6, new QStandardItem(""));
                
                // Add row for Total
                row = m_gradesModel->rowCount();
                m_gradesModel->insertRow(row);
                auto totalItem = new QStandardItem(total.isEmpty() ? "N/A" : total);
                totalItem->setForeground(QBrush(QColor("#007AFF")));
                QFont font = totalItem->font();
                font.setBold(true);
                totalItem->setFont(font);
                m_gradesModel->setItem(row, 0, new QStandardItem(courseName));
                m_gradesModel->setItem(row, 1, new QStandardItem("Total Grade"));
                m_gradesModel->setItem(row, 2, new QStandardItem("Total"));
                m_gradesModel->setItem(row, 3, totalItem);
                m_gradesModel->setItem(row, 4, new QStandardItem("--"));
                m_gradesModel->setItem(row, 5, new QStandardItem("--"));
                m_gradesModel->setItem(row, 6, new QStandardItem(""));
            } else {
                // Admin/Faculty view - show student name
                m_gradesModel->setItem(row, 0, new QStandardItem(studentName));
                m_gradesModel->setItem(row, 1, new QStandardItem(courseName));
                m_gradesModel->setItem(row, 2, new QStandardItem("A1: " + (a1.isEmpty() ? "N/A" : a1)));
                m_gradesModel->setItem(row, 3, new QStandardItem("A2: " + (a2.isEmpty() ? "N/A" : a2)));
                m_gradesModel->setItem(row, 4, new QStandardItem("Final: " + (finalExam.isEmpty() ? "N/A" : finalExam)));
                m_gradesModel->setItem(row, 5, new QStandardItem("Total: " + (total.isEmpty() ? "N/A" : total)));
                m_gradesModel->setItem(row, 6, new QStandardItem(""));
                
                // Store student_id and course_id for reference
                m_gradesModel->item(row, 0)->setData(query.value("student_id").toInt(), Qt::UserRole);
                m_gradesModel->item(row, 0)->setData(query.value("course_id").toInt(), Qt::UserRole + 1);
            }
        }
    }
}

void GradesSystem::onAddGrade()
{
    // Get course combo from form - find it by object name
    QComboBox *courseFormCombo = findChild<QComboBox*>("courseFormCombo");
    
    if (!courseFormCombo || courseFormCombo->currentIndex() < 0) {
        QMessageBox::warning(this, "Validation", "Please select a course.");
        return;
    }
    
    int courseId = courseFormCombo->currentData().toInt();
    int studentId = m_userId;
    
    // If admin/faculty, get student from combo
    if (m_role != "Student") {
        if (!m_studentCombo || m_studentCombo->currentIndex() < 0) {
            QMessageBox::warning(this, "Validation", "Please select a student.");
            return;
        }
        studentId = m_studentCombo->currentData().toInt();
    }
    
    // Check if student is enrolled in course
    QSqlQuery checkQuery(DatabaseManager::instance().getDatabase());
    checkQuery.prepare("SELECT COUNT(*) FROM student_section ss "
                      "JOIN sections s ON ss.section_id = s.section_id "
                      "WHERE ss.student_id = :student_id AND s.course_id = :course_id");
    checkQuery.bindValue(":student_id", studentId);
    checkQuery.bindValue(":course_id", courseId);
    
    if (!checkQuery.exec() || !checkQuery.next() || checkQuery.value(0).toInt() == 0) {
        QMessageBox::warning(this, "Enrollment Error", 
            "The selected student is not enrolled in this course.");
        return;
    }
    
    // Map assignment type to grade field
    QString assignmentType = m_assignmentTypeCombo->currentText();
    QString gradeField;
    if (assignmentType == "Assignment 1" || assignmentType == "A1") {
        gradeField = "a1";
    } else if (assignmentType == "Assignment 2" || assignmentType == "A2") {
        gradeField = "a2";
    } else if (assignmentType == "Final Exam" || assignmentType == "Final") {
        gradeField = "final_exam";
    } else {
        QMessageBox::warning(this, "Validation", 
            "Please select Assignment 1, Assignment 2, or Final Exam.");
        return;
    }
    
    // Convert percentage to letter grade (score is already percentage)
    double percentage = m_scoreSpin->value();
    
    QString letterGrade;
    if (percentage >= 97) letterGrade = "A+";
    else if (percentage >= 93) letterGrade = "A";
    else if (percentage >= 90) letterGrade = "A-";
    else if (percentage >= 87) letterGrade = "B+";
    else if (percentage >= 83) letterGrade = "B";
    else if (percentage >= 80) letterGrade = "B-";
    else if (percentage >= 77) letterGrade = "C+";
    else if (percentage >= 73) letterGrade = "C";
    else if (percentage >= 70) letterGrade = "C-";
    else if (percentage >= 67) letterGrade = "D+";
    else if (percentage >= 63) letterGrade = "D";
    else if (percentage >= 60) letterGrade = "D-";
    else letterGrade = "F";
    
    // Check if grade record exists
    QSqlQuery checkGradeQuery(DatabaseManager::instance().getDatabase());
    checkGradeQuery.prepare("SELECT student_id, course_id FROM grades WHERE student_id = :student_id AND course_id = :course_id");
    checkGradeQuery.bindValue(":student_id", studentId);
    checkGradeQuery.bindValue(":course_id", courseId);
    
    bool recordExists = (checkGradeQuery.exec() && checkGradeQuery.next());
    
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    if (recordExists) {
        // Update existing record
        QString updateSql;
        if (gradeField == "a1") {
            updateSql = "UPDATE grades SET a1 = :grade WHERE student_id = :student_id AND course_id = :course_id";
        } else if (gradeField == "a2") {
            updateSql = "UPDATE grades SET a2 = :grade WHERE student_id = :student_id AND course_id = :course_id";
        } else if (gradeField == "final_exam") {
            updateSql = "UPDATE grades SET final_exam = :grade WHERE student_id = :student_id AND course_id = :course_id";
        } else {
            QMessageBox::warning(this, "Error", "Invalid grade field.");
            return;
        }
        query.prepare(updateSql);
        query.bindValue(":grade", letterGrade);
        query.bindValue(":student_id", studentId);
        query.bindValue(":course_id", courseId);
    } else {
        // Insert new record
        query.prepare("INSERT INTO grades (student_id, course_id, a1, a2, final_exam, total) "
                     "VALUES (:student_id, :course_id, :a1, :a2, :final_exam, :total)");
        query.bindValue(":student_id", studentId);
        query.bindValue(":course_id", courseId);
        query.bindValue(":a1", gradeField == "a1" ? letterGrade : "");
        query.bindValue(":a2", gradeField == "a2" ? letterGrade : "");
        query.bindValue(":final_exam", gradeField == "final_exam" ? letterGrade : "");
        query.bindValue(":total", ""); // Will be calculated
    }
    
    if (query.exec()) {
        // Calculate and update total grade
        QSqlQuery calcQuery(DatabaseManager::instance().getDatabase());
        calcQuery.prepare("SELECT a1, a2, final_exam FROM grades WHERE student_id = :student_id AND course_id = :course_id");
        calcQuery.bindValue(":student_id", studentId);
        calcQuery.bindValue(":course_id", courseId);
        
        if (calcQuery.exec() && calcQuery.next()) {
            QString a1 = calcQuery.value("a1").toString();
            QString a2 = calcQuery.value("a2").toString();
            QString finalExam = calcQuery.value("final_exam").toString();
            
            // Calculate total (weighted average)
            QString totalGrade = calculateTotalGrade(a1, a2, finalExam);
            
            QSqlQuery updateTotalQuery(DatabaseManager::instance().getDatabase());
            updateTotalQuery.prepare("UPDATE grades SET total = :total WHERE student_id = :student_id AND course_id = :course_id");
            updateTotalQuery.bindValue(":total", totalGrade);
            updateTotalQuery.bindValue(":student_id", studentId);
            updateTotalQuery.bindValue(":course_id", courseId);
            updateTotalQuery.exec();
        }
        
        QMessageBox::information(this, "Success", 
            QString("Grade '%1' (%2%) has been %3 successfully.")
            .arg(letterGrade).arg(QString::number(percentage, 'f', 1))
            .arg(recordExists ? "updated" : "added"));
        m_scoreSpin->setValue(85);  // Reset to default
        refreshGrades();
        calculateGPA();
    } else {
        QMessageBox::critical(this, "Database Error", 
            QString("Failed to add grade.\n\nError: %1").arg(query.lastError().text()));
    }
}

QString GradesSystem::calculateTotalGrade(const QString& a1, const QString& a2, const QString& finalExam)
{
    // Convert letter grades to numeric values for calculation
    auto gradeToNum = [](const QString& grade) -> double {
        if (grade == "A+") return 4.0;
        if (grade == "A") return 4.0;
        if (grade == "A-") return 3.7;
        if (grade == "B+") return 3.3;
        if (grade == "B") return 3.0;
        if (grade == "B-") return 2.7;
        if (grade == "C+") return 2.3;
        if (grade == "C") return 2.0;
        if (grade == "C-") return 1.7;
        if (grade == "D+") return 1.3;
        if (grade == "D") return 1.0;
        if (grade == "D-") return 0.7;
        if (grade == "F") return 0.0;
        return -1; // Invalid
    };
    
    int count = 0;
    double sum = 0;
    
    if (!a1.isEmpty()) {
        double val = gradeToNum(a1);
        if (val >= 0) { sum += val * 0.3; count++; }
    }
    if (!a2.isEmpty()) {
        double val = gradeToNum(a2);
        if (val >= 0) { sum += val * 0.3; count++; }
    }
    if (!finalExam.isEmpty()) {
        double val = gradeToNum(finalExam);
        if (val >= 0) { sum += val * 0.4; count++; }
    }
    
    if (count == 0) return "";
    
    double avg = sum; // Already weighted
    
    // Convert back to letter grade
    if (avg >= 3.85) return "A";
    if (avg >= 3.5) return "A-";
    if (avg >= 3.15) return "B+";
    if (avg >= 2.85) return "B";
    if (avg >= 2.5) return "B-";
    if (avg >= 2.15) return "C+";
    if (avg >= 1.85) return "C";
    if (avg >= 1.5) return "C-";
    if (avg >= 1.15) return "D+";
    if (avg >= 0.85) return "D";
    if (avg >= 0.5) return "D-";
    return "F";
}

void GradesSystem::onDeleteGrade()
{
    auto selection = m_gradesTable->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, "No Selection", 
            "Please select a grade record from the table to delete.");
        return;
    }
    
    int row = selection.first().row();
    QStandardItem* item = m_gradesModel->item(row, 0);
    if (!item) return;
    
    int studentId = item->data(Qt::UserRole).toInt();
    int courseId = item->data(Qt::UserRole + 1).toInt();
    
    if (studentId == 0 || courseId == 0) {
        QMessageBox::warning(this, "Error", "Could not identify the grade record to delete.");
        return;
    }
    
    // Get course and student names for confirmation
    QSqlQuery nameQuery(DatabaseManager::instance().getDatabase());
    nameQuery.prepare("SELECT c.name as course_name, s.name as student_name "
                     "FROM courses c, students s "
                     "WHERE c.course_id = :course_id AND s.student_id = :student_id");
    nameQuery.bindValue(":course_id", courseId);
    nameQuery.bindValue(":student_id", studentId);
    
    QString courseName = "Unknown";
    QString studentName = "Unknown";
    if (nameQuery.exec() && nameQuery.next()) {
        courseName = nameQuery.value("course_name").toString();
        studentName = nameQuery.value("student_name").toString();
    }
    
    int ret = QMessageBox::question(this, "Confirm Delete", 
        QString("Are you sure you want to delete all grades for:\n\nStudent: %1\nCourse: %2\n\nThis action cannot be undone.")
        .arg(studentName).arg(courseName),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    
    if (ret != QMessageBox::Yes) {
        return;
    }
    
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("DELETE FROM grades WHERE student_id = :student_id AND course_id = :course_id");
    query.bindValue(":student_id", studentId);
    query.bindValue(":course_id", courseId);
    
    if (query.exec()) {
        QMessageBox::information(this, "Success", 
            QString("Grades for %1 in %2 have been deleted successfully.")
            .arg(studentName).arg(courseName));
        refreshGrades();
        calculateGPA();
    } else {
        QMessageBox::critical(this, "Database Error", 
            QString("Failed to delete grade.\n\nError: %1").arg(query.lastError().text()));
    }
}

void GradesSystem::refreshGrades()
{
    int currentIndex = m_courseCombo->currentIndex();
    if (currentIndex >= 0) {
        int courseId = m_courseCombo->itemData(currentIndex).toInt();
        loadGrades(courseId);
    }
}

void GradesSystem::calculateGPA()
{
    if (m_role != "Student") {
        m_gpaLabel->setText("GPA: N/A (Admin View)");
        return;
    }
    
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT total FROM grades WHERE student_id = :student_id AND total != '' AND total IS NOT NULL");
    query.bindValue(":student_id", m_userId);
    
    // Convert letter grades to GPA points
    auto gradeToGPA = [](const QString& grade) -> double {
        if (grade == "A+" || grade == "A") return 4.0;
        if (grade == "A-") return 3.7;
        if (grade == "B+") return 3.3;
        if (grade == "B") return 3.0;
        if (grade == "B-") return 2.7;
        if (grade == "C+") return 2.3;
        if (grade == "C") return 2.0;
        if (grade == "C-") return 1.7;
        if (grade == "D+") return 1.3;
        if (grade == "D") return 1.0;
        if (grade == "D-") return 0.7;
        if (grade == "F") return 0.0;
        return -1;
    };
    
    double sumGPA = 0;
    int count = 0;
    
    if (query.exec()) {
        while (query.next()) {
            QString total = query.value("total").toString();
            double gpa = gradeToGPA(total);
            if (gpa >= 0) {
                sumGPA += gpa;
                count++;
            }
        }
    }
    
    if (count > 0) {
        double avgGPA = sumGPA / count;
        m_gpaLabel->setText(QString("GPA: %1").arg(avgGPA, 0, 'f', 2));
    } else {
        m_gpaLabel->setText("GPA: --");
    }
}

