#include "sectionsystem.h"
#include "sectiondialog.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QStandardItem>

SectionSystem::SectionSystem(QWidget *parent) : QWidget(parent)
{
    setupUi();
    loadSections();
}

void SectionSystem::setupUi()
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // Header Area
    auto headerLayout = new QHBoxLayout();
    
    auto title = new QLabel("Class Sections Management");
    title->setStyleSheet("font-size: 24px; font-weight: bold;");
    headerLayout->addWidget(title);
    
    headerLayout->addStretch();
    
    m_searchBar = new QLineEdit();
    m_searchBar->setPlaceholderText("Search sections...");
    m_searchBar->setFixedWidth(300);
    m_searchBar->setStyleSheet("QLineEdit { padding: 8px 12px; border: 1px solid #bdc3c7; border-radius: 20px; } QLineEdit:focus { border-color: #3498db; }");
    headerLayout->addWidget(m_searchBar);
    
    mainLayout->addLayout(headerLayout);

    // Toolbar
    auto toolbarLayout = new QHBoxLayout();
    m_btnAdd = new QPushButton("Create Section");
    m_btnAdd->setCursor(Qt::PointingHandCursor);
    m_btnAdd->setStyleSheet("QPushButton { background-color: #27ae60; color: white; padding: 8px 16px; border: none; border-radius: 4px; font-size: 14px; font-weight: 600; } QPushButton:hover { opacity: 0.9; }");
    
    toolbarLayout->addWidget(m_btnAdd);
    toolbarLayout->addStretch();
    mainLayout->addLayout(toolbarLayout);

    // Table
    m_view = new QTableView(this);
    m_model = new QStandardItemModel(this);
    m_model->setColumnCount(4);
    m_model->setHorizontalHeaderLabels({"Section ID", "Course", "Max Students", "Actions"});
    
    m_view->setModel(m_model);
    styleTable();
    
    mainLayout->addWidget(m_view);

    connect(m_btnAdd, &QPushButton::clicked, this, &SectionSystem::onAddSection);
    connect(m_searchBar, &QLineEdit::textChanged, this, &SectionSystem::onSearch);
}

void SectionSystem::styleTable()
{
    m_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_view->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setAlternatingRowColors(true);
    m_view->verticalHeader()->setVisible(false);
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view->setShowGrid(false);
    m_view->setStyleSheet("QTableView::item { padding: 5px; }");
}

void SectionSystem::loadSections()
{
    m_model->removeRows(0, m_model->rowCount());
    auto sections = m_repo.getAllSections();
    
    // Optimize: Pre-fetch course names
    // For small apps, fetching all is fine.
    // Ideally we join in SQL, but let's just make a map here for simplicity
    std::vector<Course> courses = m_courseRepo.getAllCourses();
    QHash<int, QString> courseNames;
    for(const auto& c : courses) courseNames[c.id] = c.name;
    
    for (const auto &s : sections) {
        QList<QStandardItem*> row;
        row << new QStandardItem(QString::number(s.id));
        
        // Show Course Name
        QString courseName = courseNames.value(s.courseId, "Unknown Course (" + QString::number(s.courseId) + ")");
        row << new QStandardItem(courseName);
        
        row << new QStandardItem(QString::number(s.maxStudents));
        row << new QStandardItem(""); // Actions
        
        row[0]->setData(s.id, Qt::UserRole);
        
        m_model->appendRow(row);
        
        // Actions
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
        
        int secId = s.id;
        connect(editBtn, &QPushButton::clicked, this, [this, secId]() { editSection(secId); });
        connect(deleteBtn, &QPushButton::clicked, this, [this, secId]() { deleteSection(secId); });
        
        m_view->setIndexWidget(m_model->index(m_model->rowCount() - 1, 3), actionWidget);
    }
}

void SectionSystem::onAddSection()
{
    SectionDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        if (m_repo.addSection(dialog.getSection())) {
            refreshData();
            QMessageBox::information(this, "Success", "Section created.");
        } else {
            QMessageBox::critical(this, "Error", "Failed to create section.");
        }
    }
}

void SectionSystem::editSection(int id)
{
    auto s = m_repo.getSectionById(id);
    if (s) {
        SectionDialog dialog(this, &(*s));
        if (dialog.exec() == QDialog::Accepted) {
            Section updated = dialog.getSection();
            updated.id = id;
            if (m_repo.updateSection(updated)) {
                refreshData();
                QMessageBox::information(this, "Success", "Section updated.");
            }
        }
    }
}

void SectionSystem::deleteSection(int id)
{
    if (QMessageBox::Yes == QMessageBox::question(this, "Confirm", "Delete this section?")) {
        if (m_repo.deleteSection(id)) {
            refreshData();
        }
    }
}

void SectionSystem::onSearch(const QString &text)
{
    for (int i = 0; i < m_model->rowCount(); ++i) {
        bool match = false;
        for (int j = 0; j < m_model->columnCount(); ++j) {
            if (m_model->item(i, j)->text().contains(text, Qt::CaseInsensitive)) {
                match = true;
                break;
            }
        }
        m_view->setRowHidden(i, !match);
    }
}

void SectionSystem::refreshData()
{
    loadSections();
}
