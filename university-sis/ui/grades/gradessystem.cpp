#include "gradessystem.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QStringList>
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
}

void GradesSystem::setupUi()
{
    auto mainLayout = qobject_cast<QVBoxLayout*>(layout());
    if (!mainLayout) return;
    
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
    headers << "Course" << "Assignment" << "Type" << "Score" << "Max" << "Percentage" << "Date";
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
    
    // Add Grade Form
    auto formGroup = new QGroupBox("Add Grade");
    auto formLayout = new QVBoxLayout(formGroup);
    formLayout->setSpacing(10);
    
    auto courseFormLayout = new QHBoxLayout();
    courseFormLayout->addWidget(new QLabel("Course:"));
    QComboBox *courseFormCombo = new QComboBox(this);
    courseFormCombo->setObjectName("courseFormCombo");
    courseFormLayout->addWidget(courseFormCombo);
    formLayout->addLayout(courseFormLayout);
    
    // Load courses for form
    QSqlQuery courseQuery(DatabaseManager::instance().getDatabase());
    courseQuery.exec("SELECT id, course_code, course_name FROM courses ORDER BY course_code");
    while (courseQuery.next()) {
        QString text = courseQuery.value("course_code").toString() + " - " + courseQuery.value("course_name").toString();
        courseFormCombo->addItem(text, courseQuery.value("id").toInt());
    }
    
    auto typeLayout = new QHBoxLayout();
    typeLayout->addWidget(new QLabel("Type:"));
    m_assignmentTypeCombo = new QComboBox(this);
    m_assignmentTypeCombo->addItems({"Exam", "Quiz", "Assignment", "Project", "Lab", "Participation", "Final"});
    typeLayout->addWidget(m_assignmentTypeCombo);
    formLayout->addLayout(typeLayout);
    
    auto nameLayout = new QHBoxLayout();
    nameLayout->addWidget(new QLabel("Assignment:"));
    m_assignmentNameEdit = new QLineEdit(this);
    m_assignmentNameEdit->setPlaceholderText("Assignment name");
    nameLayout->addWidget(m_assignmentNameEdit);
    formLayout->addLayout(nameLayout);
    
    auto scoreLayout = new QHBoxLayout();
    scoreLayout->addWidget(new QLabel("Score:"));
    m_scoreSpin = new QDoubleSpinBox(this);
    m_scoreSpin->setRange(0, 1000);
    m_scoreSpin->setDecimals(2);
    scoreLayout->addWidget(m_scoreSpin);
    scoreLayout->addWidget(new QLabel("Max:"));
    m_maxScoreSpin = new QDoubleSpinBox(this);
    m_maxScoreSpin->setRange(0, 1000);
    m_maxScoreSpin->setValue(100);
    m_maxScoreSpin->setDecimals(2);
    scoreLayout->addWidget(m_maxScoreSpin);
    formLayout->addLayout(scoreLayout);
    
    auto dateLayout = new QHBoxLayout();
    dateLayout->addWidget(new QLabel("Date:"));
    m_gradeDateEdit = new QDateEdit(this);
    m_gradeDateEdit->setDate(QDate::currentDate());
    m_gradeDateEdit->setCalendarPopup(true);
    dateLayout->addWidget(m_gradeDateEdit);
    dateLayout->addStretch();
    formLayout->addLayout(dateLayout);
    
    auto buttonLayout = new QHBoxLayout();
    m_addBtn = new QPushButton("Add Grade", this);
    m_addBtn->setProperty("type", "primary");
    m_deleteBtn = new QPushButton("Delete Selected", this);
    m_deleteBtn->setProperty("type", "danger");
    buttonLayout->addWidget(m_addBtn);
    buttonLayout->addWidget(m_deleteBtn);
    buttonLayout->addStretch();
    formLayout->addLayout(buttonLayout);
    
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
        query.prepare("SELECT DISTINCT c.id, c.course_code, c.course_name "
                     "FROM courses c "
                     "JOIN enrollments e ON c.id = e.course_id "
                     "WHERE e.student_id = :student_id "
                     "ORDER BY c.course_code");
        query.bindValue(":student_id", m_userId);
    } else {
        query.prepare("SELECT id, course_code, course_name FROM courses ORDER BY course_code");
    }
    
    if (query.exec()) {
        while (query.next()) {
            QString text = query.value("course_code").toString() + " - " + query.value("course_name").toString();
            m_courseCombo->addItem(text, query.value("id").toInt());
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
    if (courseId == -1) {
        if (m_role == "Student") {
            query.prepare("SELECT g.id, c.course_code, g.assignment_name, g.assignment_type, "
                         "g.score, g.max_score, g.grade_date "
                         "FROM grades g "
                         "JOIN enrollments e ON g.enrollment_id = e.id "
                         "JOIN courses c ON e.course_id = c.id "
                         "WHERE e.student_id = :student_id "
                         "ORDER BY g.grade_date DESC");
            query.bindValue(":student_id", m_userId);
        } else {
            query.prepare("SELECT g.id, c.course_code, g.assignment_name, g.assignment_type, "
                         "g.score, g.max_score, g.grade_date "
                         "FROM grades g "
                         "JOIN enrollments e ON g.enrollment_id = e.id "
                         "JOIN courses c ON e.course_id = c.id "
                         "ORDER BY g.grade_date DESC");
        }
    } else {
        if (m_role == "Student") {
            query.prepare("SELECT g.id, c.course_code, g.assignment_name, g.assignment_type, "
                         "g.score, g.max_score, g.grade_date "
                         "FROM grades g "
                         "JOIN enrollments e ON g.enrollment_id = e.id "
                         "JOIN courses c ON e.course_id = c.id "
                         "WHERE e.course_id = :course_id AND e.student_id = :student_id "
                         "ORDER BY g.grade_date DESC");
            query.bindValue(":course_id", courseId);
            query.bindValue(":student_id", m_userId);
        } else {
            query.prepare("SELECT g.id, c.course_code, g.assignment_name, g.assignment_type, "
                         "g.score, g.max_score, g.grade_date "
                         "FROM grades g "
                         "JOIN enrollments e ON g.enrollment_id = e.id "
                         "JOIN courses c ON e.course_id = c.id "
                         "WHERE e.course_id = :course_id "
                         "ORDER BY g.grade_date DESC");
            query.bindValue(":course_id", courseId);
        }
    }
    
    if (query.exec()) {
        while (query.next()) {
            int row = m_gradesModel->rowCount();
            m_gradesModel->insertRow(row);
            
            double score = query.value("score").toDouble();
            double maxScore = query.value("max_score").toDouble();
            double percentage = maxScore > 0 ? (score / maxScore) * 100 : 0;
            
            m_gradesModel->setItem(row, 0, new QStandardItem(query.value("course_code").toString()));
            m_gradesModel->setItem(row, 1, new QStandardItem(query.value("assignment_name").toString()));
            m_gradesModel->setItem(row, 2, new QStandardItem(query.value("assignment_type").toString()));
            m_gradesModel->setItem(row, 3, new QStandardItem(QString::number(score, 'f', 2)));
            m_gradesModel->setItem(row, 4, new QStandardItem(QString::number(maxScore, 'f', 2)));
            m_gradesModel->setItem(row, 5, new QStandardItem(QString::number(percentage, 'f', 1) + "%"));
            m_gradesModel->setItem(row, 6, new QStandardItem(query.value("grade_date").toDate().toString("MMM dd, yyyy")));
            
            m_gradesModel->item(row, 6)->setData(query.value("id").toInt(), Qt::UserRole);
        }
    }
}

void GradesSystem::onAddGrade()
{
    if (m_assignmentNameEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Validation", "Please enter an assignment name.");
        return;
    }
    
    // Get course combo from form - find it by object name
    QComboBox *courseFormCombo = findChild<QComboBox*>("courseFormCombo");
    
    if (!courseFormCombo || courseFormCombo->currentIndex() < 0) {
        QMessageBox::warning(this, "Validation", "Please select a course.");
        return;
    }
    
    int courseId = courseFormCombo->currentData().toInt();
    
    // Find enrollment
    QSqlQuery enrollQuery(DatabaseManager::instance().getDatabase());
    enrollQuery.prepare("SELECT id FROM enrollments WHERE course_id = :course_id AND student_id = :student_id LIMIT 1");
    enrollQuery.bindValue(":course_id", courseId);
    enrollQuery.bindValue(":student_id", m_userId);
    
    if (!enrollQuery.exec() || !enrollQuery.next()) {
        QMessageBox::warning(this, "Enrollment Error", 
            "The selected student is not enrolled in this course.\n\nPlease verify the student's enrollment status.");
        return;
    }
    
    int enrollmentId = enrollQuery.value("id").toInt();
    
    // Validation
    if (m_assignmentNameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", 
            "Please enter an assignment name.");
        m_assignmentNameEdit->setFocus();
        return;
    }
    
    if (m_scoreSpin->value() < 0) {
        QMessageBox::warning(this, "Validation Error", 
            "Score cannot be negative.");
        m_scoreSpin->setFocus();
        return;
    }
    
    if (m_maxScoreSpin->value() <= 0) {
        QMessageBox::warning(this, "Validation Error", 
            "Maximum score must be greater than zero.");
        m_maxScoreSpin->setFocus();
        return;
    }
    
    if (m_scoreSpin->value() > m_maxScoreSpin->value()) {
        QMessageBox::warning(this, "Validation Error", 
            "Score cannot be greater than maximum score.");
        m_scoreSpin->setFocus();
        return;
    }
    
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("INSERT INTO grades (enrollment_id, assignment_type, assignment_name, score, max_score, grade_date) "
                  "VALUES (:enrollment_id, :type, :name, :score, :max_score, :date)");
    query.bindValue(":enrollment_id", enrollmentId);
    query.bindValue(":type", m_assignmentTypeCombo->currentText());
    query.bindValue(":name", m_assignmentNameEdit->text().trimmed());
    query.bindValue(":score", m_scoreSpin->value());
    query.bindValue(":max_score", m_maxScoreSpin->value());
    query.bindValue(":date", m_gradeDateEdit->date().toString("yyyy-MM-dd"));
    
    if (query.exec()) {
        QMessageBox::information(this, "Success", 
            QString("Grade for '%1' has been added successfully.").arg(m_assignmentNameEdit->text()));
        m_assignmentNameEdit->clear();
        m_scoreSpin->setValue(0);
        m_maxScoreSpin->setValue(100);
        refreshGrades();
        calculateGPA();
    } else {
        QMessageBox::critical(this, "Database Error", 
            QString("Failed to add grade.\n\nError: %1").arg(query.lastError().text()));
    }
}

void GradesSystem::onDeleteGrade()
{
    auto selection = m_gradesTable->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, "No Selection", 
            "Please select a grade from the table to delete.");
        return;
    }
    
    int row = selection.first().row();
    QString assignmentName = m_gradesModel->item(row, 1)->text();
    int gradeId = m_gradesModel->item(row, 6)->data(Qt::UserRole).toInt();
    
    int ret = QMessageBox::question(this, "Confirm Delete", 
        QString("Are you sure you want to delete the grade for '%1'?\n\nThis action cannot be undone.").arg(assignmentName),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    
    if (ret != QMessageBox::Yes) {
        return;
    }
    
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("DELETE FROM grades WHERE id = :id");
    query.bindValue(":id", gradeId);
    
    if (query.exec()) {
        QMessageBox::information(this, "Success", 
            QString("Grade for '%1' has been deleted successfully.").arg(assignmentName));
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
    query.prepare("SELECT AVG((g.score / g.max_score) * 4.0) as gpa "
                 "FROM grades g "
                 "JOIN enrollments e ON g.enrollment_id = e.id "
                 "WHERE e.student_id = :student_id");
    query.bindValue(":student_id", m_userId);
    
    if (query.exec() && query.next()) {
        double gpa = query.value("gpa").toDouble();
        m_gpaLabel->setText(QString("GPA: %1").arg(gpa, 0, 'f', 2));
    } else {
        m_gpaLabel->setText("GPA: --");
    }
}

