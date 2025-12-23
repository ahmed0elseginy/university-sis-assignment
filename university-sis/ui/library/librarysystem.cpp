#include "librarysystem.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QInputDialog>
#include <QDateEdit>
#include <QSpinBox>
#include <QGroupBox>
#include <QStringList>
#include <QDialog>
#include <QDate>
#include "../../database/databasemanager.h"

LibrarySystem::LibrarySystem(QWidget* parent) 
    : BaseSystemWidget("Library System", parent)
    , m_currentUserRole("")
    , m_currentUserId(0)
{
    setupUi();
}

void LibrarySystem::setUserContext(const QString& role, int userId) {
    m_currentUserRole = role;
    m_currentUserId = userId;
}

void LibrarySystem::setupUi()
{
    auto mainLayout = qobject_cast<QVBoxLayout*>(layout());
    if (!mainLayout) return;
    
    // Search and Filter Section
    auto searchGroup = new QGroupBox("Search & Filter");
    auto searchLayout = new QHBoxLayout(searchGroup);
    searchLayout->setSpacing(12);
    
    searchLayout->addWidget(new QLabel("Search:"));
    m_search = new QLineEdit(this);
    m_search->setPlaceholderText("Search by title, author, ISBN, or category...");
    m_search->setMinimumWidth(300);
    searchLayout->addWidget(m_search);
    
    searchLayout->addWidget(new QLabel("Filter:"));
    m_filterCombo = new QComboBox(this);
    m_filterCombo->addItems({"All Books", "Available", "Checked Out", "Overdue"});
    searchLayout->addWidget(m_filterCombo);
    searchLayout->addStretch();
    
    mainLayout->addWidget(searchGroup);
    
    // Books Table
    auto tableGroup = new QGroupBox("Books");
    auto tableLayout = new QVBoxLayout(tableGroup);
    
    m_view = new QTableView(this);
    m_model = new QStandardItemModel(0, 7, this);
    QStringList headers;
    headers << "ISBN" << "Title" << "Author" << "Category" << "Available" << "Total" << "Location";
    m_model->setHorizontalHeaderLabels(headers);
    m_view->setModel(m_model);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->horizontalHeader()->setStretchLastSection(true);
    m_view->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_view->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_view->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_view->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_view->verticalHeader()->setVisible(false);
    m_view->setAlternatingRowColors(true);
    m_view->setShowGrid(false);
    m_view->setSortingEnabled(true);
    tableLayout->addWidget(m_view);
    
    mainLayout->addWidget(tableGroup, 2);
    
    // Action Buttons
    auto buttonGroup = new QGroupBox("Actions");
    auto buttonLayout = new QHBoxLayout(buttonGroup);
    buttonLayout->setSpacing(12);
    
    m_addBtn = new QPushButton("Add Book", this);
    m_addBtn->setProperty("type", "primary");
    m_checkoutBtn = new QPushButton("Check Out", this);
    m_checkoutBtn->setProperty("type", "success");
    m_returnBtn = new QPushButton("Return Book", this);
    m_returnBtn->setProperty("type", "info");
    m_deleteBtn = new QPushButton("Delete Book", this);
    m_deleteBtn->setProperty("type", "danger");
    
    buttonLayout->addWidget(m_addBtn);
    buttonLayout->addWidget(m_checkoutBtn);
    buttonLayout->addWidget(m_returnBtn);
    buttonLayout->addWidget(m_deleteBtn);
    buttonLayout->addStretch();
    
    mainLayout->addWidget(buttonGroup);
    
    // Connections
    connect(m_search, &QLineEdit::textChanged, this, &LibrarySystem::onSearch);
    connect(m_filterCombo, &QComboBox::currentIndexChanged, this, &LibrarySystem::onFilterChanged);
    connect(m_addBtn, &QPushButton::clicked, this, &LibrarySystem::onAddBook);
    connect(m_checkoutBtn, &QPushButton::clicked, this, &LibrarySystem::onCheckOutBook);
    connect(m_returnBtn, &QPushButton::clicked, this, &LibrarySystem::onReturnBook);
    connect(m_deleteBtn, &QPushButton::clicked, this, &LibrarySystem::onDeleteBook);
    
    // Load books
    loadBooks();
}

void LibrarySystem::onSearch(const QString& text) {
    loadBooks(text);
}

void LibrarySystem::onFilterChanged(int index) {
    loadBooks(m_search->text());
}

void LibrarySystem::loadBooks(const QString& filter) {
    m_model->removeRows(0, m_model->rowCount());
    
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    
    QString whereClause = "WHERE 1=1";
    
    // Apply search filter
    if (!filter.isEmpty()) {
        whereClause += QString(" AND (title LIKE '%%1%' OR author LIKE '%%1%' OR isbn LIKE '%%1%' OR category LIKE '%%1%')")
                      .arg(filter);
    }
    
    // Apply status filter
    QString filterText = m_filterCombo->currentText();
    if (filterText == "Available") {
        whereClause += " AND available_copies > 0";
    } else if (filterText == "Checked Out") {
        whereClause += " AND available_copies < total_copies";
    } else if (filterText == "Overdue") {
        // This would require a join with book_loans, simplified for now
        whereClause += " AND available_copies < total_copies";
    }
    
    query.prepare(QString("SELECT book_id, isbn, title, author, category, available_copies, total_copies, location "
                  "FROM books %1 ORDER BY title").arg(whereClause));
    
    if (query.exec()) {
        while (query.next()) {
            int row = m_model->rowCount();
            m_model->insertRow(row);
            
            int bookId = query.value("book_id").toInt();
            int available = query.value("available_copies").toInt();
            int total = query.value("total_copies").toInt();
            
            m_model->setItem(row, 0, new QStandardItem(query.value("isbn").toString()));
            m_model->setItem(row, 1, new QStandardItem(query.value("title").toString()));
            m_model->setItem(row, 2, new QStandardItem(query.value("author").toString()));
            m_model->setItem(row, 3, new QStandardItem(query.value("category").toString()));
            
            // Available copies with color coding
            auto availableItem = new QStandardItem(QString::number(available));
            if (available > 0) {
                availableItem->setForeground(QColor("#34C759")); // Green
            } else {
                availableItem->setForeground(QColor("#FF3B30")); // Red
            }
            m_model->setItem(row, 4, availableItem);
            
            m_model->setItem(row, 5, new QStandardItem(QString::number(total)));
            m_model->setItem(row, 6, new QStandardItem(query.value("location").toString()));
            
            // Store book_id in first column for reference
            m_model->item(row, 0)->setData(bookId, Qt::UserRole);
        }
    }
}

void LibrarySystem::refreshBooks() {
    loadBooks(m_search->text());
}

void LibrarySystem::onAddBook() {
    // Create dialog for adding book
    QDialog dialog(this);
    dialog.setWindowTitle("Add New Book");
    dialog.setModal(true);
    dialog.setMinimumWidth(500);
    
    auto layout = new QVBoxLayout(&dialog);
    layout->setSpacing(15);
    
    // ISBN
    auto isbnLayout = new QHBoxLayout();
    isbnLayout->addWidget(new QLabel("ISBN:"));
    auto isbnEdit = new QLineEdit(&dialog);
    isbnEdit->setPlaceholderText("e.g., 978-0131103627");
    isbnLayout->addWidget(isbnEdit);
    layout->addLayout(isbnLayout);
    
    // Title
    auto titleLayout = new QHBoxLayout();
    titleLayout->addWidget(new QLabel("Title:"));
    auto titleEdit = new QLineEdit(&dialog);
    titleEdit->setPlaceholderText("Book title");
    titleLayout->addWidget(titleEdit);
    layout->addLayout(titleLayout);
    
    // Author
    auto authorLayout = new QHBoxLayout();
    authorLayout->addWidget(new QLabel("Author:"));
    auto authorEdit = new QLineEdit(&dialog);
    authorEdit->setPlaceholderText("Author name");
    authorLayout->addWidget(authorEdit);
    layout->addLayout(authorLayout);
    
    // Publisher
    auto publisherLayout = new QHBoxLayout();
    publisherLayout->addWidget(new QLabel("Publisher:"));
    auto publisherEdit = new QLineEdit(&dialog);
    publisherEdit->setPlaceholderText("Publisher name");
    publisherLayout->addWidget(publisherEdit);
    layout->addLayout(publisherLayout);
    
    // Year
    auto yearLayout = new QHBoxLayout();
    yearLayout->addWidget(new QLabel("Year:"));
    auto yearSpin = new QSpinBox(&dialog);
    yearSpin->setRange(1900, QDate::currentDate().year());
    yearSpin->setValue(QDate::currentDate().year());
    yearLayout->addWidget(yearSpin);
    yearLayout->addStretch();
    layout->addLayout(yearLayout);
    
    // Category
    auto categoryLayout = new QHBoxLayout();
    categoryLayout->addWidget(new QLabel("Category:"));
    auto categoryEdit = new QLineEdit(&dialog);
    categoryEdit->setPlaceholderText("e.g., Computer Science");
    categoryLayout->addWidget(categoryEdit);
    layout->addLayout(categoryLayout);
    
    // Copies
    auto copiesLayout = new QHBoxLayout();
    copiesLayout->addWidget(new QLabel("Total Copies:"));
    auto copiesSpin = new QSpinBox(&dialog);
    copiesSpin->setRange(1, 100);
    copiesSpin->setValue(1);
    copiesLayout->addWidget(copiesSpin);
    copiesLayout->addStretch();
    layout->addLayout(copiesLayout);
    
    // Location
    auto locationLayout = new QHBoxLayout();
    locationLayout->addWidget(new QLabel("Location:"));
    auto locationEdit = new QLineEdit(&dialog);
    locationEdit->setPlaceholderText("e.g., Main Library - CS Section");
    locationLayout->addWidget(locationEdit);
    layout->addLayout(locationLayout);
    
    // Buttons
    auto buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    auto addBtn = new QPushButton("Add Book", &dialog);
    addBtn->setProperty("type", "primary");
    auto cancelBtn = new QPushButton("Cancel", &dialog);
    buttonLayout->addWidget(cancelBtn);
    buttonLayout->addWidget(addBtn);
    layout->addLayout(buttonLayout);
    
    connect(addBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        if (titleEdit->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, "Validation Error", "Please enter a book title.");
            return;
        }
        
        QSqlQuery query(DatabaseManager::instance().getDatabase());
        query.prepare("INSERT INTO books (isbn, title, author, publisher, publication_year, category, total_copies, available_copies, location) "
                      "VALUES (:isbn, :title, :author, :publisher, :year, :category, :copies, :copies, :location)");
        query.bindValue(":isbn", isbnEdit->text().trimmed());
        query.bindValue(":title", titleEdit->text().trimmed());
        query.bindValue(":author", authorEdit->text().trimmed());
        query.bindValue(":publisher", publisherEdit->text().trimmed());
        query.bindValue(":year", yearSpin->value());
        query.bindValue(":category", categoryEdit->text().trimmed());
        query.bindValue(":copies", copiesSpin->value());
        query.bindValue(":location", locationEdit->text().trimmed());
        
        if (query.exec()) {
            QMessageBox::information(this, "Success", 
                QString("Book '%1' has been added successfully.").arg(titleEdit->text()));
            refreshBooks();
        } else {
            QMessageBox::critical(this, "Database Error", 
                QString("Failed to add book.\n\nError: %1").arg(query.lastError().text()));
        }
    }
}

void LibrarySystem::onCheckOutBook() {
    auto selection = m_view->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, "No Selection", "Please select a book to check out.");
        return;
    }
    
    int row = selection.first().row();
    int bookId = m_model->item(row, 0)->data(Qt::UserRole).toInt();
    QString bookTitle = m_model->item(row, 1)->text();
    int available = m_model->item(row, 4)->text().toInt();
    
    if (available <= 0) {
        QMessageBox::warning(this, "Not Available", 
            QString("'%1' is currently not available for checkout.").arg(bookTitle));
        return;
    }
    
    // Get borrower ID
    bool ok;
    int borrowerId = QInputDialog::getInt(this, "Check Out Book", 
        QString("Enter Student/Faculty ID for '%1':").arg(bookTitle), 
        0, 1, 999999, 1, &ok);
    
    if (!ok) return;
    
    // Check if borrower exists and determine type
    QSqlQuery checkStudentQuery(DatabaseManager::instance().getDatabase());
    checkStudentQuery.prepare("SELECT student_id FROM students WHERE student_id = :id");
    checkStudentQuery.bindValue(":id", borrowerId);
    
    bool isStudent = false;
    bool isFaculty = false;
    
    if (checkStudentQuery.exec() && checkStudentQuery.next()) {
        isStudent = true;
    } else {
        QSqlQuery checkFacultyQuery(DatabaseManager::instance().getDatabase());
        checkFacultyQuery.prepare("SELECT faculty_id FROM faculty WHERE faculty_id = :id");
        checkFacultyQuery.bindValue(":id", borrowerId);
        if (checkFacultyQuery.exec() && checkFacultyQuery.next()) {
            isFaculty = true;
        }
    }
    
    if (!isStudent && !isFaculty) {
        QMessageBox::warning(this, "Invalid ID", 
            "The entered ID does not exist in the system.\n\nPlease verify the student or faculty ID.");
        return;
    }
    
    // Calculate due date (14 days from now)
    QDate checkoutDate = QDate::currentDate();
    QDate dueDate = checkoutDate.addDays(14);
    
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    if (isStudent) {
        query.prepare("INSERT INTO book_loans (book_id, student_id, checkout_date, due_date, status) "
                      "VALUES (:book_id, :student_id, :checkout_date, :due_date, 'Checked Out')");
        query.bindValue(":book_id", bookId);
        query.bindValue(":student_id", borrowerId);
    } else {
        query.prepare("INSERT INTO book_loans (book_id, faculty_id, checkout_date, due_date, status) "
                      "VALUES (:book_id, :faculty_id, :checkout_date, :due_date, 'Checked Out')");
        query.bindValue(":book_id", bookId);
        query.bindValue(":faculty_id", borrowerId);
    }
    query.bindValue(":checkout_date", checkoutDate.toString("yyyy-MM-dd"));
    query.bindValue(":due_date", dueDate.toString("yyyy-MM-dd"));
    
    if (query.exec()) {
        // Update available copies
        QSqlQuery updateQuery(DatabaseManager::instance().getDatabase());
        updateQuery.prepare("UPDATE books SET available_copies = available_copies - 1 WHERE book_id = :id");
        updateQuery.bindValue(":id", bookId);
        updateQuery.exec();
        
        QMessageBox::information(this, "Success", 
            QString("'%1' has been checked out successfully.\n\nDue Date: %2")
            .arg(bookTitle).arg(dueDate.toString("MMMM dd, yyyy")));
        refreshBooks();
    } else {
        QMessageBox::critical(this, "Database Error", 
            QString("Failed to check out book.\n\nError: %1").arg(query.lastError().text()));
    }
}

void LibrarySystem::onReturnBook() {
    auto selection = m_view->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, "No Selection", "Please select a book to return.");
        return;
    }
    
    int row = selection.first().row();
    int bookId = m_model->item(row, 0)->data(Qt::UserRole).toInt();
    QString bookTitle = m_model->item(row, 1)->text();
    int available = m_model->item(row, 4)->text().toInt();
    int total = m_model->item(row, 5)->text().toInt();
    
    if (available >= total) {
        QMessageBox::information(this, "Already Returned", 
            QString("All copies of '%1' are already available.").arg(bookTitle));
        return;
    }
    
    // Find active loan for this book
    QSqlQuery findQuery(DatabaseManager::instance().getDatabase());
    findQuery.prepare("SELECT loan_id, student_id, due_date FROM book_loans "
                      "WHERE book_id = :book_id AND status = 'Checked Out' "
                      "ORDER BY checkout_date DESC LIMIT 1");
    findQuery.bindValue(":book_id", bookId);
    
    if (!findQuery.exec() || !findQuery.next()) {
        QMessageBox::warning(this, "No Active Loan", 
            QString("No active loan found for '%1'.").arg(bookTitle));
        return;
    }
    
    int loanId = findQuery.value("loan_id").toInt();
    QDate dueDate = findQuery.value("due_date").toDate();
    QDate returnDate = QDate::currentDate();
    
    // Update loan status
    QSqlQuery updateLoanQuery(DatabaseManager::instance().getDatabase());
    updateLoanQuery.prepare("UPDATE book_loans SET return_date = :return_date, status = 'Returned' WHERE loan_id = :loan_id");
    updateLoanQuery.bindValue(":return_date", returnDate.toString("yyyy-MM-dd"));
    updateLoanQuery.bindValue(":loan_id", loanId);
    
    if (updateLoanQuery.exec()) {
        // Update available copies
        QSqlQuery updateBookQuery(DatabaseManager::instance().getDatabase());
        updateBookQuery.prepare("UPDATE books SET available_copies = available_copies + 1 WHERE book_id = :id");
        updateBookQuery.bindValue(":id", bookId);
        updateBookQuery.exec();
        
        QString message = QString("'%1' has been returned successfully.").arg(bookTitle);
        if (returnDate > dueDate) {
            int daysOverdue = dueDate.daysTo(returnDate);
            message += QString("\n\nNote: This book was %1 day(s) overdue.").arg(daysOverdue);
        }
        
        QMessageBox::information(this, "Success", message);
        refreshBooks();
    } else {
        QMessageBox::critical(this, "Database Error", 
            QString("Failed to return book.\n\nError: %1").arg(updateLoanQuery.lastError().text()));
    }
}

void LibrarySystem::onDeleteBook() {
    auto selection = m_view->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, "No Selection", "Please select a book to delete.");
        return;
    }
    
    int row = selection.first().row();
    int bookId = m_model->item(row, 0)->data(Qt::UserRole).toInt();
    QString bookTitle = m_model->item(row, 1)->text();
    int available = m_model->item(row, 4)->text().toInt();
    int total = m_model->item(row, 5)->text().toInt();
    
    if (available < total) {
        QMessageBox::warning(this, "Cannot Delete", 
            QString("Cannot delete '%1' because there are %2 copy/copies currently checked out.")
            .arg(bookTitle).arg(total - available));
        return;
    }
    
    int ret = QMessageBox::question(this, "Confirm Delete", 
        QString("Are you sure you want to delete '%1'?\n\nThis action cannot be undone.").arg(bookTitle),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    
    if (ret != QMessageBox::Yes) {
        return;
    }
    
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("DELETE FROM books WHERE book_id = :id");
    query.bindValue(":id", bookId);
    
    if (query.exec()) {
        QMessageBox::information(this, "Success", 
            QString("'%1' has been deleted successfully.").arg(bookTitle));
        refreshBooks();
    } else {
        QMessageBox::critical(this, "Database Error", 
            QString("Failed to delete book.\n\nError: %1").arg(query.lastError().text()));
    }
}
