#include "attendancesystem.h"
#include "attendancedialog.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QStandardItem>

AttendanceSystem::AttendanceSystem(QWidget *parent) : QWidget(parent)
{
    setupUi();
    loadAttendance();
}

void AttendanceSystem::setupUi()
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // Header
    auto headerLayout = new QHBoxLayout();
    auto title = new QLabel("Attendance Monitoring");
    title->setStyleSheet("font-size: 24px; font-weight: bold;");
    headerLayout->addWidget(title);
    headerLayout->addStretch();
    mainLayout->addLayout(headerLayout);

    // Toolbar
    auto toolbarLayout = new QHBoxLayout();
    m_btnAdd = new QPushButton("Record Attendance");
    m_btnAdd->setCursor(Qt::PointingHandCursor);
    m_btnAdd->setStyleSheet("QPushButton { background-color: #34495e; color: white; padding: 8px 16px; border: none; border-radius: 4px; font-size: 14px; font-weight: 600; } QPushButton:hover { opacity: 0.9; }");
    toolbarLayout->addWidget(m_btnAdd);
    toolbarLayout->addStretch();
    mainLayout->addLayout(toolbarLayout);

    // Table
    m_view = new QTableView(this);
    m_model = new QStandardItemModel(this);
    m_model->setColumnCount(6);
    m_model->setHorizontalHeaderLabels({"ID", "Student ID", "Course ID", "Status", "Date", "Actions"});
    
    m_view->setModel(m_model);
    styleTable();
    
    mainLayout->addWidget(m_view);

    connect(m_btnAdd, &QPushButton::clicked, this, &AttendanceSystem::onAddAttendance);
}

void AttendanceSystem::styleTable()
{
    m_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_view->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_view->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_view->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);

    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setAlternatingRowColors(true);
    m_view->verticalHeader()->setVisible(false);
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view->setShowGrid(false);
    m_view->setStyleSheet("QTableView::item { padding: 5px; }");
}

void AttendanceSystem::loadAttendance()
{
    m_model->removeRows(0, m_model->rowCount());
    auto list = m_repo.getAllAttendance();
    
    for (const auto &a : list) {
        QList<QStandardItem*> row;
        row << new QStandardItem(QString::number(a.id));
        row << new QStandardItem(QString::number(a.studentId));
        row << new QStandardItem(QString::number(a.courseId));
        
        auto statusItem = new QStandardItem(a.status);
        if (a.status == "Present") statusItem->setForeground(QBrush(QColor("#27ae60")));
        else if (a.status == "Absent") statusItem->setForeground(QBrush(QColor("#c0392b")));
        else statusItem->setForeground(QBrush(QColor("#f39c12")));
        
        row << statusItem;
        row << new QStandardItem(a.date.toString("yyyy-MM-dd"));
        
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
        
        int attId = a.id;
        connect(editBtn, &QPushButton::clicked, this, [this, attId]() { editAttendance(attId); });
        connect(deleteBtn, &QPushButton::clicked, this, [this, attId]() { deleteAttendance(attId); });
        
        m_view->setIndexWidget(m_model->index(m_model->rowCount() - 1, 5), actionWidget);
    }
}

void AttendanceSystem::onAddAttendance()
{
    AttendanceDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        Attendance a = dialog.getAttendance();
        if (m_repo.addAttendance(a)) {
            refreshData();
            QMessageBox::information(this, "Success", "Attendance recorded successfully.");
        } else {
            QMessageBox::critical(this, "Error", "Failed to record attendance. Check IDs.");
        }
    }
}

void AttendanceSystem::editAttendance(int id)
{
    auto opt = m_repo.getAttendanceById(id);
    if (opt) {
        AttendanceDialog dialog(this, &(*opt));
        if (dialog.exec() == QDialog::Accepted) {
            Attendance updated = dialog.getAttendance();
            updated.id = id;
            if (m_repo.updateAttendance(updated)) {
                refreshData();
                QMessageBox::information(this, "Success", "Attendance updated.");
            }
        }
    }
}

void AttendanceSystem::deleteAttendance(int id)
{
    if (QMessageBox::Yes == QMessageBox::question(this, "Confirm", "Delete this attendance record?")) {
        if (m_repo.deleteAttendance(id)) {
            refreshData();
        }
    }
}

void AttendanceSystem::refreshData()
{
    loadAttendance();
}
