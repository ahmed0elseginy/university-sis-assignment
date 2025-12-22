#include "facultysystem.h"
#include "facultydialog.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QStandardItem>

FacultySystem::FacultySystem(QWidget *parent) : QWidget(parent)
{
    setupUi();
    loadFaculty();
}

void FacultySystem::setupUi()
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // Header Area
    auto headerLayout = new QHBoxLayout();
    
    auto title = new QLabel("Faculty & Staff Management");
    title->setStyleSheet("font-size: 24px; font-weight: bold;");
    headerLayout->addWidget(title);
    
    headerLayout->addStretch();
    
    m_searchBar = new QLineEdit();
    m_searchBar->setPlaceholderText("Search faculty...");
    m_searchBar->setFixedWidth(300);
    m_searchBar->setStyleSheet("QLineEdit { padding: 8px 12px; border: 1px solid #bdc3c7; border-radius: 20px; } QLineEdit:focus { border-color: #3498db; }");
    headerLayout->addWidget(m_searchBar);
    
    mainLayout->addLayout(headerLayout);

    // Toolbar
    auto toolbarLayout = new QHBoxLayout();
    
    m_btnAdd = new QPushButton("Add Faculty");
    m_btnAdd->setCursor(Qt::PointingHandCursor);
    m_btnAdd->setStyleSheet("QPushButton { background-color: #8e44ad; color: white; padding: 8px 16px; border: none; border-radius: 4px; font-size: 14px; font-weight: 600; } "
                            "QPushButton:hover { opacity: 0.9; }");

    toolbarLayout->addWidget(m_btnAdd);
    toolbarLayout->addStretch();
    
    mainLayout->addLayout(toolbarLayout);

    // Table
    m_view = new QTableView(this);
    m_model = new QStandardItemModel(this);
    m_model->setColumnCount(6); 
    // ID, Name, Email, Dept, Position, Actions
    m_model->setHorizontalHeaderLabels({"ID", "Name", "Email", "Department", "Position", "Actions"});
    
    m_view->setModel(m_model);
    styleTable();
    
    mainLayout->addWidget(m_view);

    // Connections
    connect(m_btnAdd, &QPushButton::clicked, this, &FacultySystem::onAddFaculty);
    connect(m_searchBar, &QLineEdit::textChanged, this, &FacultySystem::onSearch);
}

void FacultySystem::styleTable()
{
    m_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_view->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_view->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_view->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setAlternatingRowColors(true);
    m_view->verticalHeader()->setVisible(false);
    m_view->verticalHeader()->setDefaultSectionSize(50);
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view->setShowGrid(false);
    m_view->setStyleSheet("QTableView::item { padding: 5px; }"); 
}

void FacultySystem::loadFaculty()
{
    m_model->removeRows(0, m_model->rowCount());
    auto list = m_repo.getAllFaculty();
    
    for (const auto &f : list) {
        QList<QStandardItem*> row;
        row << new QStandardItem(QString::number(f.id));
        row << new QStandardItem(f.name);
        row << new QStandardItem(f.email);
        row << new QStandardItem(f.department);
        row << new QStandardItem(f.position);
        row << new QStandardItem(""); // Actions placeholder
        
        row[0]->setData(f.id, Qt::UserRole);
        
        m_model->appendRow(row);
        
        // Create Action Widget
        QWidget* actionWidget = new QWidget();
        QHBoxLayout* layout = new QHBoxLayout(actionWidget);
        layout->setContentsMargins(4, 4, 4, 4);
        layout->setSpacing(8);
        
        // EDIT
        QPushButton* editBtn = new QPushButton("Edit");
        editBtn->setCursor(Qt::PointingHandCursor);
        editBtn->setStyleSheet("QPushButton { background-color: #E3F2FD; color: #1565C0; border: none; border-radius: 6px; padding: 6px 12px; font-weight: 600; font-size: 12px; } QPushButton:hover { background-color: #BBDEFB; }");
        
        // DELETE
        QPushButton* deleteBtn = new QPushButton("Delete");
        deleteBtn->setCursor(Qt::PointingHandCursor);
        deleteBtn->setStyleSheet("QPushButton { background-color: #FFEBEE; color: #C62828; border: none; border-radius: 6px; padding: 6px 12px; font-weight: 600; font-size: 12px; } QPushButton:hover { background-color: #FFCDD2; }");

        layout->addWidget(editBtn);
        layout->addWidget(deleteBtn);
        layout->addStretch();
        
        int fId = f.id;
        connect(editBtn, &QPushButton::clicked, this, [this, fId]() { editFaculty(fId); });
        connect(deleteBtn, &QPushButton::clicked, this, [this, fId]() { deleteFaculty(fId); });

        m_view->setIndexWidget(m_model->index(m_model->rowCount() - 1, 5), actionWidget);
    }
}

void FacultySystem::refreshData()
{
    loadFaculty();
}

void FacultySystem::onAddFaculty()
{
    FacultyDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        Faculty f = dialog.getFaculty();
        if (m_repo.addFaculty(f)) {
            refreshData();
            QMessageBox::information(this, "Success", "Faculty added successfully.");
        } else {
            QMessageBox::critical(this, "Error", "Failed to add faculty.");
        }
    }
}

void FacultySystem::editFaculty(int id)
{
    auto opt = m_repo.getFacultyById(id);
    if (opt) {
        FacultyDialog dialog(this, &(*opt));
        if (dialog.exec() == QDialog::Accepted) {
            Faculty updated = dialog.getFaculty();
            updated.id = id;
            if (m_repo.updateFaculty(updated)) {
                refreshData();
                QMessageBox::information(this, "Success", "Faculty updated successfully.");
            } else {
                QMessageBox::critical(this, "Error", "Failed to update faculty.");
            }
        }
    }
}

void FacultySystem::deleteFaculty(int id)
{
    if (QMessageBox::Yes == QMessageBox::question(this, "Confirm Delete", "Are you sure you want to delete this faculty member?")) {
        if (m_repo.deleteFaculty(id)) {
            refreshData();
        } else {
            QMessageBox::critical(this, "Error", "Failed to delete faculty.");
        }
    }
}

void FacultySystem::onSearch(const QString &text)
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
