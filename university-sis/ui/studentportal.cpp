#include "studentportal.h"
#include "student/studentdialog.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QStandardItem>

StudentPortal::StudentPortal(QWidget *parent) : QWidget(parent)
{
    setupUi();
    loadStudents();
}

void StudentPortal::setUserContext(const QString& role, int userId)
{
    m_currentUserRole = role;
    m_currentUserId = userId;
    
    // Hide Add button if not Admin/Faculty
    if (m_btnAdd) {
        bool canEdit = (role == "Administrator" || role == "Faculty Member");
        m_btnAdd->setVisible(canEdit);
    }
    
    loadStudents(); // Reload with filter
}

void StudentPortal::setupUi()
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // Header Area
    auto headerLayout = new QHBoxLayout();
    
    auto title = new QLabel("Student Management");
    title->setStyleSheet("font-size: 24px; font-weight: bold;");
    headerLayout->addWidget(title);
    
    headerLayout->addStretch();
    
    m_searchBar = new QLineEdit();
    m_searchBar->setPlaceholderText("Search students...");
    m_searchBar->setFixedWidth(300);
    m_searchBar->setStyleSheet("QLineEdit { padding: 8px 12px; border: 1px solid #bdc3c7; border-radius: 20px; } QLineEdit:focus { border-color: #3498db; }");
    headerLayout->addWidget(m_searchBar);
    
    mainLayout->addLayout(headerLayout);

    // Toolbar
    auto toolbarLayout = new QHBoxLayout();
    
    auto createBtn = [](const QString &text, const QString &color) {
        auto btn = new QPushButton(text);
        btn->setCursor(Qt::PointingHandCursor);
        // We only set background/text color, border-radius etc are inherited or set here
        btn->setStyleSheet(QString("QPushButton { background-color: %1; color: white; padding: 8px 16px; border: none; border-radius: 4px; font-size: 14px; font-weight: 600; } "
                                   "QPushButton:hover { opacity: 0.9; }").arg(color));
        return btn;
    };

    m_btnAdd = createBtn("Add Student", "#007AFF"); // Updated to match macOS blue

    toolbarLayout->addWidget(m_btnAdd);
    toolbarLayout->addStretch();
    
    mainLayout->addLayout(toolbarLayout);

    // Table
    m_view = new QTableView(this);
    m_model = new QStandardItemModel(this);
    m_model->setColumnCount(6); // Added Actions Column
    m_model->setHorizontalHeaderLabels({"ID", "Name", "Year", "Department", "Section ID", "Actions"});
    
    m_view->setModel(m_model);
    styleTable();
    
    mainLayout->addWidget(m_view);

    // Connections
    connect(m_btnAdd, &QPushButton::clicked, this, &StudentPortal::onAddStudent);
    connect(m_searchBar, &QLineEdit::textChanged, this, &StudentPortal::onSearch);
}

void StudentPortal::styleTable()
{
    m_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // Make ID and Year/Section smaller
    m_view->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_view->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_view->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setAlternatingRowColors(true);
    m_view->verticalHeader()->setVisible(false);
    m_view->verticalHeader()->setDefaultSectionSize(50); // Ensure rows are tall enough for buttons
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view->setShowGrid(false);
    
    // Clean style that works with both themes
    m_view->setStyleSheet("QTableView::item { padding: 5px; }"); 
}

void StudentPortal::loadStudents()
{
    m_model->removeRows(0, m_model->rowCount());
    
    std::vector<Student> students;
    
    // If Student, only load self
    if (m_currentUserRole == "Student" && m_currentUserId != -1) {
        auto s = m_repo.getStudentById(m_currentUserId);
        if (s) {
            students.push_back(*s);
        }
    } else {
        // Admin/Faculty see all
        students = m_repo.getAllStudents();
    }
    
    for (const auto &s : students) {
        QList<QStandardItem*> row;
        row << new QStandardItem(QString::number(s.id));
        row << new QStandardItem(s.name);
        row << new QStandardItem(QString::number(s.year));
        row << new QStandardItem(s.department);
        row << new QStandardItem(QString::number(s.sectionId));
        row << new QStandardItem(""); // Placeholder for Actions
        
        // Store ID in user data for retrieval
        row[0]->setData(s.id, Qt::UserRole);
        
        m_model->appendRow(row);
        
        // Create Action Widget
        QWidget* actionWidget = new QWidget();
        QHBoxLayout* layout = new QHBoxLayout(actionWidget);
        layout->setContentsMargins(4, 4, 4, 4);
        layout->setSpacing(8);
        
        // VIEW BUTTON always visible
        QPushButton* viewBtn = new QPushButton("View");
        viewBtn->setCursor(Qt::PointingHandCursor);
        viewBtn->setStyleSheet(
            "QPushButton { "
            "   background-color: #E8F5E9; "
            "   color: #2E7D32; "
            "   border: none; "
            "   border-radius: 6px; "
            "   padding: 6px 12px; "
            "   font-weight: 600; "
            "   font-size: 12px; "
            "}"
            "QPushButton:hover { background-color: #C8E6C9; }"
        );
        layout->addWidget(viewBtn);

        // Edit/Delete buttons only if NOT Student
        if (m_currentUserRole != "Student") {
            QPushButton* editBtn = new QPushButton("Edit");
            editBtn->setCursor(Qt::PointingHandCursor);
            editBtn->setStyleSheet(
                "QPushButton { "
                "   background-color: #E3F2FD; "
                "   color: #1565C0; "
                "   border: none; "
                "   border-radius: 6px; "
                "   padding: 6px 12px; "
                "   font-weight: 600; "
                "   font-size: 12px; "
                "}"
                "QPushButton:hover { background-color: #BBDEFB; }"
            );
            
            QPushButton* deleteBtn = new QPushButton("Delete");
            deleteBtn->setCursor(Qt::PointingHandCursor);
            deleteBtn->setStyleSheet(
                "QPushButton { "
                "   background-color: #FFEBEE; "
                "   color: #C62828; "
                "   border: none; "
                "   border-radius: 6px; "
                "   padding: 6px 12px; "
                "   font-weight: 600; "
                "   font-size: 12px; "
                "}"
                "QPushButton:hover { background-color: #FFCDD2; }"
            );
            
            layout->addWidget(editBtn);
            layout->addWidget(deleteBtn);
            
            int studentId = s.id;
            connect(editBtn, &QPushButton::clicked, this, [this, studentId]() { editStudent(studentId); });
            connect(deleteBtn, &QPushButton::clicked, this, [this, studentId]() { deleteStudent(studentId); });
        }
        
        layout->addStretch(); 
        
        // Connect View Button
        int studentId = s.id; 
        connect(viewBtn, &QPushButton::clicked, this, [this, studentId]() { viewStudent(studentId); });

        // Set widget in the last column of the newly added row
        m_view->setIndexWidget(m_model->index(m_model->rowCount() - 1, 5), actionWidget);
    }
}

void StudentPortal::refreshData()
{
    loadStudents();
}

void StudentPortal::onAddStudent()
{
    StudentDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        Student s = dialog.getStudent();
        if (m_repo.addStudent(s)) {
            refreshData();
            emit dataChanged(); // Notify that data has changed
            QMessageBox::information(this, "Success", "Student added successfully.");
        } else {
            QMessageBox::critical(this, "Error", "Failed to add student.");
        }
    }
}

void StudentPortal::viewStudent(int id)
{
    // For now, View behaves similar to Edit but we could make it Read-Only
    auto studentOpt = m_repo.getStudentById(id);
    if (studentOpt) {
        QString details = QString("ID: %1\nName: %2\nYear: %3\nDepartment: %4\nSection: %5")
                          .arg(studentOpt->id)
                          .arg(studentOpt->name)
                          .arg(studentOpt->year)
                          .arg(studentOpt->department)
                          .arg(studentOpt->sectionId);
        QMessageBox::information(this, "Student Details", details);
    }
}

void StudentPortal::editStudent(int id)
{
    auto studentOpt = m_repo.getStudentById(id);
    if (studentOpt) {
        StudentDialog dialog(this, &(*studentOpt));
        if (dialog.exec() == QDialog::Accepted) {
            Student updated = dialog.getStudent();
            updated.id = id; // Ensure ID is preserved
            if (m_repo.updateStudent(updated)) {
                refreshData();
                QMessageBox::information(this, "Success", "Student updated successfully.");
            } else {
                QMessageBox::critical(this, "Error", "Failed to update student.");
            }
        }
    }
}



void StudentPortal::deleteStudent(int id)
{
    if (QMessageBox::Yes == QMessageBox::question(this, "Confirm Delete", "Are you sure you want to delete this student?")) {
        if (m_repo.deleteStudent(id)) {
            refreshData();
            emit dataChanged(); // Notify that data has changed
        } else {
            QMessageBox::critical(this, "Error", "Failed to delete student.");
        }
    }
}

void StudentPortal::onSearch(const QString &text)
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
