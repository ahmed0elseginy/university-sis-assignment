#include "academicsystem.h"
#include "coursedialog.h"
#include "sectiondialog.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QStandardItem>

AcademicSystem::AcademicSystem(QWidget *parent) : QWidget(parent)
{
    setupUi();
    loadCourses();
    loadSections();
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
    // m_searchBar->setStyleSheet("..."); // Use global theme
    headerLayout->addWidget(m_searchBar);
    
    mainLayout->addLayout(headerLayout);

    // TABS
    m_tabs = new QTabWidget(this);
    // Styles handled by ThemeManager

    // --- Tab 1: Courses ---
    QWidget *courseTab = new QWidget();
    QVBoxLayout *courseLayout = new QVBoxLayout(courseTab);
    
    auto courseToolbar = new QHBoxLayout();
    auto btnAddCourse = new QPushButton("Add New Course");
    btnAddCourse->setCursor(Qt::PointingHandCursor);
    btnAddCourse->setProperty("type", "primary");
    courseToolbar->addWidget(btnAddCourse);
    courseToolbar->addStretch();
    courseLayout->addLayout(courseToolbar);

    m_courseView = new QTableView();
    m_courseModel = new QStandardItemModel(this);
    m_courseModel->setColumnCount(5); 
    m_courseModel->setHorizontalHeaderLabels({"ID", "Course Name", "Year", "Credits", "Actions"});
    m_courseView->setModel(m_courseModel);
    styleTable(m_courseView);
    courseLayout->addWidget(m_courseView);
    
    connect(btnAddCourse, &QPushButton::clicked, this, &AcademicSystem::onAddCourse);
    
    // --- Tab 2: Sections ---
    QWidget *sectionTab = new QWidget();
    QVBoxLayout *sectionLayout = new QVBoxLayout(sectionTab);
    
    auto sectionToolbar = new QHBoxLayout();
    auto btnAddSection = new QPushButton("Create Section");
    btnAddSection->setCursor(Qt::PointingHandCursor);
    btnAddSection->setProperty("type", "success");
    sectionToolbar->addWidget(btnAddSection);
    sectionToolbar->addStretch();
    sectionLayout->addLayout(sectionToolbar);
    
    m_sectionView = new QTableView();
    m_sectionModel = new QStandardItemModel(this);
    m_sectionModel->setColumnCount(4);
    m_sectionModel->setHorizontalHeaderLabels({"Section ID", "Course ID", "Capacity", "Actions"});
    m_sectionView->setModel(m_sectionModel);
    styleTable(m_sectionView);
    sectionLayout->addWidget(m_sectionView);
    
    connect(btnAddSection, &QPushButton::clicked, this, &AcademicSystem::onAddSection);
    
    m_tabs->addTab(courseTab, "Courses Catalog");
    m_tabs->addTab(sectionTab, "Class Sections");
    
    mainLayout->addWidget(m_tabs);

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
        editBtn->setCursor(Qt::PointingHandCursor);
        editBtn->setProperty("type", "info");
        
        QPushButton* deleteBtn = new QPushButton("Delete");
        deleteBtn->setCursor(Qt::PointingHandCursor);
        deleteBtn->setProperty("type", "danger");

        layout->addWidget(editBtn);
        layout->addWidget(deleteBtn);
        layout->addStretch();
        
        int courseId = c.id;
        connect(editBtn, &QPushButton::clicked, this, [this, courseId]() { editCourse(courseId); });
        connect(deleteBtn, &QPushButton::clicked, this, [this, courseId]() { deleteCourse(courseId); });

        m_courseView->setIndexWidget(m_courseModel->index(m_courseModel->rowCount() - 1, 4), actionWidget);
    }
}

void AcademicSystem::loadSections()
{
    m_sectionModel->removeRows(0, m_sectionModel->rowCount());
    auto sections = m_sectionRepo.getAllSections();
    
    for (const auto &s : sections) {
        QList<QStandardItem*> row;
        row << new QStandardItem(QString::number(s.id));
        row << new QStandardItem(QString::number(s.courseId));
        row << new QStandardItem(QString::number(s.maxStudents));
        row << new QStandardItem(""); // Actions
        
        m_sectionModel->appendRow(row);
        
        QWidget* actionWidget = new QWidget();
        QHBoxLayout* layout = new QHBoxLayout(actionWidget);
        layout->setContentsMargins(4, 4, 4, 4);
        
        QPushButton* deleteBtn = new QPushButton("Delete");
        deleteBtn->setCursor(Qt::PointingHandCursor);
        deleteBtn->setProperty("type", "danger");
        layout->addWidget(deleteBtn);
        layout->addStretch();
        
        int sectionId = s.id;
        connect(deleteBtn, &QPushButton::clicked, this, [this, sectionId]() { deleteSection(sectionId); });
        
        m_sectionView->setIndexWidget(m_sectionModel->index(m_sectionModel->rowCount() - 1, 3), actionWidget);
    }
}

void AcademicSystem::refreshData()
{
    loadCourses();
    loadSections();
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

void AcademicSystem::onAddSection()
{
    SectionDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        if (m_sectionRepo.addSection(dialog.getSection())) {
            refreshData();
            QMessageBox::information(this, "Success", "Section created.");
        } else {
            QMessageBox::critical(this, "Error", "Failed to create section. Ensure Course ID exists.");
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

void AcademicSystem::deleteSection(int id)
{
    if (QMessageBox::Yes == QMessageBox::question(this, "Confirm", "Delete this section?")) {
        if (m_sectionRepo.deleteSection(id)) refreshData();
    }
}

void AcademicSystem::viewCourse(int id) {} // Not used in new UI

void AcademicSystem::onSearch(const QString &text)
{
    // Search both models
    auto search = [&](QTableView* view, QStandardItemModel* model) {
         for (int i = 0; i < model->rowCount(); ++i) {
            bool match = false;
            for (int j = 0; j < model->columnCount(); ++j) {
                if (model->item(i, j)->text().contains(text, Qt::CaseInsensitive)) {
                    match = true;
                    break;
                }
            }
            view->setRowHidden(i, !match);
        }
    };
    
    search(m_courseView, m_courseModel);
    search(m_sectionView, m_sectionModel);
}
