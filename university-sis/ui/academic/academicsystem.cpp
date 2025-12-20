#include "academicsystem.h"
#include "coursedialog.h"
#include "academicsystem.h"
#include "coursedialog.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QStandardItem>

AcademicSystem::AcademicSystem(QWidget *parent) : QWidget(parent)
{
    setupUi();
    loadCourses();
}

void AcademicSystem::setupUi()
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // Header Area
    auto headerLayout = new QHBoxLayout();
    
    auto title = new QLabel("Academic Management");
    title->setStyleSheet("font-size: 24px; font-weight: bold;");
    headerLayout->addWidget(title);
    
    headerLayout->addStretch();
    
    m_searchBar = new QLineEdit();
    m_searchBar->setPlaceholderText("Search...");
    m_searchBar->setFixedWidth(300);
    m_searchBar->setStyleSheet("QLineEdit { padding: 8px 12px; border: 1px solid #bdc3c7; border-radius: 20px; } QLineEdit:focus { border-color: #3498db; }");
    headerLayout->addWidget(m_searchBar);
    
    mainLayout->addLayout(headerLayout);

    // Toolbar
    auto courseToolbar = new QHBoxLayout();
    auto btnAddCourse = new QPushButton("Add New Course");
    btnAddCourse->setCursor(Qt::PointingHandCursor);
    btnAddCourse->setStyleSheet("QPushButton { background-color: #007AFF; color: white; padding: 6px 12px; border: none; border-radius: 4px; font-weight: 600; } QPushButton:hover { opacity: 0.9; }");
    courseToolbar->addWidget(btnAddCourse);
    courseToolbar->addStretch();
    mainLayout->addLayout(courseToolbar);

    m_courseView = new QTableView();
    m_courseModel = new QStandardItemModel(this);
    m_courseModel->setColumnCount(5); 
    m_courseModel->setHorizontalHeaderLabels({"ID", "Course Name", "Year", "Credits", "Actions"});
    m_courseView->setModel(m_courseModel);
    styleTable(m_courseView);
    mainLayout->addWidget(m_courseView);
    
    connect(btnAddCourse, &QPushButton::clicked, this, &AcademicSystem::onAddCourse);
    
    connect(m_searchBar, &QLineEdit::textChanged, this, &AcademicSystem::onSearch);
}

void AcademicSystem::styleTable(QTableView* view)
{
    view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    view->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setAlternatingRowColors(true);
    view->verticalHeader()->setVisible(false);
    view->verticalHeader()->setDefaultSectionSize(50);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view->setShowGrid(false);
    view->setStyleSheet("QTableView::item { padding: 5px; }"); 
}

void AcademicSystem::loadCourses()
{
    m_courseModel->removeRows(0, m_courseModel->rowCount());
    auto courses = m_courseRepo.getAllCourses();
    
    for (const auto &c : courses) {
        QList<QStandardItem*> row;
        row << new QStandardItem(QString::number(c.id));
        row << new QStandardItem(c.name);
        row << new QStandardItem(QString::number(c.year));
        row << new QStandardItem(QString::number(c.hours));
        row << new QStandardItem(""); // Actions
        
        row[0]->setData(c.id, Qt::UserRole);
        
        m_courseModel->appendRow(row);
        
        QWidget* actionWidget = new QWidget();
        QHBoxLayout* layout = new QHBoxLayout(actionWidget);
        layout->setContentsMargins(4, 4, 4, 4);
        
        QPushButton* editBtn = new QPushButton("Edit");
        editBtn->setStyleSheet("QPushButton { background-color: #E3F2FD; color: #1565C0; border: none; border-radius: 6px; padding: 4px 8px; font-weight: bold; }");
        
        QPushButton* deleteBtn = new QPushButton("Delete");
        deleteBtn->setStyleSheet("QPushButton { background-color: #FFEBEE; color: #C62828; border: none; border-radius: 6px; padding: 4px 8px; font-weight: bold; }");

        layout->addWidget(editBtn);
        layout->addWidget(deleteBtn);
        layout->addStretch();
        
        int courseId = c.id;
        connect(editBtn, &QPushButton::clicked, this, [this, courseId]() { editCourse(courseId); });
        connect(deleteBtn, &QPushButton::clicked, this, [this, courseId]() { deleteCourse(courseId); });

        m_courseView->setIndexWidget(m_courseModel->index(m_courseModel->rowCount() - 1, 4), actionWidget);
    }
}

// Methods for Sections removed.

void AcademicSystem::refreshData()
{
    loadCourses();
}

void AcademicSystem::onAddCourse()
{
    CourseDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        if (m_courseRepo.addCourse(dialog.getCourse())) {
            refreshData();
            QMessageBox::information(this, "Success", "Course added.");
        } else {
            QMessageBox::critical(this, "Error", "Failed to add course.");
        }
    }
}

void AcademicSystem::editCourse(int id)
{
    auto c = m_courseRepo.getCourseById(id);
    if (c) {
        CourseDialog dialog(this, &(*c));
        if (dialog.exec() == QDialog::Accepted) {
            Course updated = dialog.getCourse();
            updated.id = id;
            if (m_courseRepo.updateCourse(updated)) {
                refreshData();
            }
        }
    }
}

void AcademicSystem::deleteCourse(int id)
{
    if (QMessageBox::Yes == QMessageBox::question(this, "Confirm", "Delete this course? WARNING: This will delete associated sections.")) {
        if (m_courseRepo.deleteCourse(id)) refreshData();
    }
}

void AcademicSystem::viewCourse(int id) {} // Not used in new UI

void AcademicSystem::onSearch(const QString &text)
{
    for (int i = 0; i < m_courseModel->rowCount(); ++i) {
        bool match = false;
        for (int j = 0; j < m_courseModel->columnCount(); ++j) {
            if (m_courseModel->item(i, j)->text().contains(text, Qt::CaseInsensitive)) {
                match = true;
                break;
            }
        }
        m_courseView->setRowHidden(i, !match);
    }
}
