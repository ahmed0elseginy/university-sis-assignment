#include "enrollmentsystem.h"
#include "enrollmentdialog.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QStandardItem>

EnrollmentSystem::EnrollmentSystem(QWidget *parent) : QWidget(parent)
{
    setupUi();
    loadEnrollments();
}

void EnrollmentSystem::setUserContext(const QString& role, int userId)
{
    m_currentUserRole = role;
    m_currentUserId = userId;
    
    if (m_btnAdd) {
        bool canEdit = (role != "Student"); 
        // Assuming students cannot enroll themselves directly in this admin tool, 
        // or if they can, we might enable it. Based on "can't update any thing", disable.
        m_btnAdd->setVisible(canEdit);
    }
    
    loadEnrollments();
}

void EnrollmentSystem::setupUi()
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // Header
    auto headerLayout = new QHBoxLayout();
    auto title = new QLabel("Enrollment & Registration");
    title->setStyleSheet("font-size: 24px; font-weight: bold;");
    headerLayout->addWidget(title);
    headerLayout->addStretch();
    mainLayout->addLayout(headerLayout);

    // Toolbar
    auto toolbarLayout = new QHBoxLayout();
    m_btnAdd = new QPushButton("Enroll Student");
    m_btnAdd->setCursor(Qt::PointingHandCursor);
    m_btnAdd->setStyleSheet("QPushButton { background-color: #e67e22; color: white; padding: 8px 16px; border: none; border-radius: 4px; font-size: 14px; font-weight: 600; } QPushButton:hover { opacity: 0.9; }");
    toolbarLayout->addWidget(m_btnAdd);
    toolbarLayout->addStretch();
    mainLayout->addLayout(toolbarLayout);

    // Table
    m_view = new QTableView(this);
    m_model = new QStandardItemModel(this);
    m_model->setColumnCount(3);
    m_model->setHorizontalHeaderLabels({"Student ID", "Section ID", "Actions"});
    m_view->setModel(m_model);
    styleTable();
    mainLayout->addWidget(m_view);

    connect(m_btnAdd, &QPushButton::clicked, this, &EnrollmentSystem::onEnroll);
}

void EnrollmentSystem::styleTable()
{
    m_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setAlternatingRowColors(true);
    m_view->verticalHeader()->setVisible(false);
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view->setShowGrid(false);
    m_view->setStyleSheet("QTableView::item { padding: 5px; }");
}

void EnrollmentSystem::loadEnrollments()
{
    m_model->removeRows(0, m_model->rowCount());
    auto list = m_repo.getAllEnrollments();
    
    for (const auto &e : list) {
        // Filter: If Student, matching ID only. 
        if (m_currentUserRole == "Student" && e.studentId != m_currentUserId) {
            continue;
        }

        QList<QStandardItem*> row;
        row << new QStandardItem(QString::number(e.studentId));
        row << new QStandardItem(QString::number(e.sectionId));
        row << new QStandardItem("");
        
        m_model->appendRow(row);
        
        // Unenroll Button (Hide if Student)
        QWidget* actionWidget = new QWidget();
        QHBoxLayout* layout = new QHBoxLayout(actionWidget);
        layout->setContentsMargins(4, 4, 4, 4);
        
        if (m_currentUserRole != "Student") {
            QPushButton* deleteBtn = new QPushButton("Unenroll");
            deleteBtn->setCursor(Qt::PointingHandCursor);
            deleteBtn->setStyleSheet("QPushButton { background-color: #FFEBEE; color: #C62828; border: none; border-radius: 6px; padding: 6px 12px; font-weight: 600; font-size: 12px; } QPushButton:hover { background-color: #FFCDD2; }");
            layout->addWidget(deleteBtn);
            
            int sid = e.studentId;
            int secid = e.sectionId;
            connect(deleteBtn, &QPushButton::clicked, this, [this, sid, secid]() { deleteEnrollment(sid, secid); });
        }
        
        layout->addStretch();
        
        m_view->setIndexWidget(m_model->index(m_model->rowCount() - 1, 2), actionWidget);
    }
}

void EnrollmentSystem::onEnroll()
{
    EnrollmentDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        Enrollment e = dialog.getEnrollment();
        if (m_repo.enrollStudent(e)) {
            refreshData();
            QMessageBox::information(this, "Success", "Student enrolled successfully.");
        } else {
            QMessageBox::critical(this, "Error", "Failed to enroll student. Check IDs.");
        }
    }
}

void EnrollmentSystem::deleteEnrollment(int studentId, int sectionId)
{
    if (QMessageBox::Yes == QMessageBox::question(this, "Confirm", "Unenroll this student?")) {
        if (m_repo.unenrollStudent(studentId, sectionId)) {
            refreshData();
        } else {
            QMessageBox::critical(this, "Error", "Failed to unenroll.");
        }
    }
}

void EnrollmentSystem::refreshData()
{
    loadEnrollments();
}
