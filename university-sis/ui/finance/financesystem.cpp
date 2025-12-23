#include "financesystem.h"
#include "paymentdialog.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QStandardItem>

FinanceSystem::FinanceSystem(QWidget *parent) : QWidget(parent)
{
    setupUi();
    loadPayments();
}

void FinanceSystem::setupUi()
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // Header Area
    auto headerLayout = new QHBoxLayout();
    
    auto title = new QLabel("Financial Management");
    title->setStyleSheet("font-size: 24px; font-weight: bold;");
    headerLayout->addWidget(title);
    
    headerLayout->addStretch();
    
    m_searchBar = new QLineEdit();
    m_searchBar->setPlaceholderText("Search payments...");
    m_searchBar->setFixedWidth(300);
    m_searchBar->setStyleSheet("QLineEdit { padding: 8px 12px; border: 1px solid #bdc3c7; border-radius: 20px; } QLineEdit:focus { border-color: #3498db; }");
    headerLayout->addWidget(m_searchBar);
    
    mainLayout->addLayout(headerLayout);

    // Toolbar
    auto toolbarLayout = new QHBoxLayout();
    
    m_btnAdd = new QPushButton("New Payment");
    m_btnAdd->setCursor(Qt::PointingHandCursor);
    m_btnAdd->setStyleSheet("QPushButton { background-color: #27ae60; color: white; padding: 8px 16px; border: none; border-radius: 4px; font-size: 14px; font-weight: 600; } "
                            "QPushButton:hover { opacity: 0.9; }");

    toolbarLayout->addWidget(m_btnAdd);
    toolbarLayout->addStretch();
    
    mainLayout->addLayout(toolbarLayout);

    // Table
    m_view = new QTableView(this);
    m_model = new QStandardItemModel(this);
    m_model->setColumnCount(8); 
    // ID, Student ID, Student Name, Amount, Description, Status, Date, Actions
    m_model->setHorizontalHeaderLabels({"ID", "Student ID", "Student Name", "Amount", "Description", "Status", "Date", "Actions"});
    
    m_view->setModel(m_model);
    styleTable();
    
    mainLayout->addWidget(m_view);

    // Connections
    connect(m_btnAdd, &QPushButton::clicked, this, &FinanceSystem::onAddPayment);
    connect(m_searchBar, &QLineEdit::textChanged, this, &FinanceSystem::onSearch);
}

void FinanceSystem::styleTable()
{
    m_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_view->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_view->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_view->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setAlternatingRowColors(true);
    m_view->verticalHeader()->setVisible(false);
    m_view->verticalHeader()->setDefaultSectionSize(50);
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view->setShowGrid(false);
    m_view->setStyleSheet("QTableView::item { padding: 5px; }"); 
}

void FinanceSystem::loadPayments()
{
    m_model->removeRows(0, m_model->rowCount());
    auto payments = m_repo.getAllPaymentsWithNames();
    
    for (const auto &p : payments) {
        QList<QStandardItem*> row;
        row << new QStandardItem(QString::number(p.id));
        row << new QStandardItem(QString::number(p.studentId));
        row << new QStandardItem(p.studentName.isEmpty() ? "N/A" : p.studentName);
        row << new QStandardItem(QString("$%1").arg(p.amount, 0, 'f', 2));
        row << new QStandardItem(p.description);
        
        auto statusItem = new QStandardItem(p.status);
        if (p.status == "Paid") statusItem->setForeground(QBrush(QColor("#27ae60")));
        else if (p.status == "Overdue") statusItem->setForeground(QBrush(QColor("#c0392b")));
        else statusItem->setForeground(QBrush(QColor("#f39c12")));
        
        row << statusItem;
        row << new QStandardItem(p.date.toString("yyyy-MM-dd"));
        row << new QStandardItem(""); // Actions placeholder
        
        row[0]->setData(p.id, Qt::UserRole);
        
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
        
        int paymentId = p.id;
        connect(editBtn, &QPushButton::clicked, this, [this, paymentId]() { editPayment(paymentId); });
        connect(deleteBtn, &QPushButton::clicked, this, [this, paymentId]() { deletePayment(paymentId); });

        m_view->setIndexWidget(m_model->index(m_model->rowCount() - 1, 7), actionWidget);
    }
}

void FinanceSystem::refreshData()
{
    loadPayments();
}

void FinanceSystem::onAddPayment()
{
    PaymentDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        Payment p = dialog.getPayment();
        if (m_repo.addPayment(p)) {
            refreshData();
            QMessageBox::information(this, "Success", "Payment added successfully.");
        } else {
            QMessageBox::critical(this, "Error", "Failed to add payment.");
        }
    }
}

void FinanceSystem::editPayment(int id)
{
    auto paymentOpt = m_repo.getPaymentById(id);
    if (paymentOpt) {
        PaymentDialog dialog(this, &(*paymentOpt));
        if (dialog.exec() == QDialog::Accepted) {
            Payment updated = dialog.getPayment();
            updated.id = id;
            if (m_repo.updatePayment(updated)) {
                refreshData();
                QMessageBox::information(this, "Success", "Payment updated successfully.");
            } else {
                QMessageBox::critical(this, "Error", "Failed to update payment.");
            }
        }
    }
}

void FinanceSystem::deletePayment(int id)
{
    if (QMessageBox::Yes == QMessageBox::question(this, "Confirm Delete", "Are you sure you want to delete this payment?")) {
        if (m_repo.deletePayment(id)) {
            refreshData();
        } else {
            QMessageBox::critical(this, "Error", "Failed to delete payment.");
        }
    }
}

void FinanceSystem::onSearch(const QString &text)
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
