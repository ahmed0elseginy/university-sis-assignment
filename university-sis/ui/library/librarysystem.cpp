#include "librarysystem.h"
#include "../../modules/student/studentrepository.h"
#include "../../modules/faculty/facultyrepository.h"
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
#include <QDialogButtonBox>
#include <QStandardItem>
#include <QBrush>
#include <QColor>
#include <QPair>
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

LibrarySystem::BorrowerInfo LibrarySystem::selectBorrowerByName(QWidget* parent) {
    BorrowerInfo result;
    QDialog dialog(parent);
    dialog.setWindowTitle("Select Borrower");
    dialog.setModal(true);
    dialog.setMinimumSize(600, 500);
    
    auto layout = new QVBoxLayout(&dialog);
    layout->setSpacing(15);
    
    // Search box
    auto searchLayout = new QHBoxLayout();
    searchLayout->addWidget(new QLabel("Search by name:"));
    auto searchEdit = new QLineEdit(&dialog);
    searchEdit->setPlaceholderText("Type name to search...");
    searchLayout->addWidget(searchEdit);
    layout->addLayout(searchLayout);
    
    // Type selection
    auto typeLayout = new QHBoxLayout();
    typeLayout->addWidget(new QLabel("Type:"));
    auto typeCombo = new QComboBox(&dialog);
    typeCombo->addItem("All", "All");
    typeCombo->addItem("Students", "Student");
    typeCombo->addItem("Faculty", "Faculty");
    typeLayout->addWidget(typeCombo);
    typeLayout->addStretch();
    layout->addLayout(typeLayout);
    
    // Results table
    auto table = new QTableView(&dialog);
    auto model = new QStandardItemModel(&dialog);
    model->setColumnCount(3);
    model->setHorizontalHeaderLabels({"ID", "Name", "Type"});
    table->setModel(model);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->horizontalHeader()->setStretchLastSection(true);
    table->verticalHeader()->setVisible(false);
    table->setAlternatingRowColors(true);
    layout->addWidget(table);
    
    // Populate function
    auto populateResults = [&](const QString& searchText, const QString& typeFilter) {
        model->removeRows(0, model->rowCount());
        
        // Add students
        if (typeFilter == "All" || typeFilter == "Student") {
            StudentRepository studentRepo;
            auto students = studentRepo.getAllStudents();
            for (const auto& student : students) {
                if (searchText.isEmpty() || student.name.contains(searchText, Qt::CaseInsensitive)) {
                    int row = model->rowCount();
                    model->insertRow(row);
                    model->setItem(row, 0, new QStandardItem(QString::number(student.id)));
                    model->setItem(row, 1, new QStandardItem(student.name));
                    model->setItem(row, 2, new QStandardItem("Student"));
                    model->item(row, 0)->setData(student.id, Qt::UserRole);
                    model->item(row, 2)->setData("Student", Qt::UserRole);
                }
            }
        }
        
        // Add faculty
        if (typeFilter == "All" || typeFilter == "Faculty") {
            FacultyRepository facultyRepo;
            auto facultyList = facultyRepo.getAllFaculty();
            for (const auto& faculty : facultyList) {
                if (searchText.isEmpty() || faculty.name.contains(searchText, Qt::CaseInsensitive)) {
                    int row = model->rowCount();
                    model->insertRow(row);
                    model->setItem(row, 0, new QStandardItem(QString::number(faculty.id)));
                    model->setItem(row, 1, new QStandardItem(faculty.name));
                    model->setItem(row, 2, new QStandardItem("Faculty"));
                    model->item(row, 0)->setData(faculty.id, Qt::UserRole);
                    model->item(row, 2)->setData("Faculty", Qt::UserRole);
                }
            }
        }
    };
    
    // Initial population
    populateResults("", "All");
    
    // Connect search
    connect(searchEdit, &QLineEdit::textChanged, [&](const QString& text) {
        populateResults(text, typeCombo->currentData().toString());
    });
    
    // Connect type filter
    connect(typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int) {
        populateResults(searchEdit->text(), typeCombo->currentData().toString());
    });
    
    // Buttons
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttonBox);
    
    if (dialog.exec() == QDialog::Accepted) {
        auto selection = table->selectionModel()->selectedRows();
        if (!selection.isEmpty()) {
            int row = selection.first().row();
            result.id = model->item(row, 0)->data(Qt::UserRole).toInt();
            result.name = model->item(row, 1)->text();
            result.type = model->item(row, 2)->data(Qt::UserRole).toString();
        }
    }
    
    return result;
}

int LibrarySystem::getStudentIdByName(const QString& name) {
    StudentRepository repo;
    auto students = repo.getAllStudents();
    for (const auto& student : students) {
        if (student.name == name) {
            return student.id;
        }
    }
    return -1;
}

int LibrarySystem::getFacultyIdByName(const QString& name) {
    FacultyRepository repo;
    auto facultyList = repo.getAllFaculty();
    for (const auto& faculty : facultyList) {
        if (faculty.name == name) {
            return faculty.id;
        }
    }
    return -1;
}

QString LibrarySystem::getBorrowerNameById(int id, const QString& type) {
    if (type == "Student") {
        StudentRepository repo;
        auto student = repo.getStudentById(id);
        if (student) {
            return student->name;
        }
    } else if (type == "Faculty") {
        FacultyRepository repo;
        auto faculty = repo.getFacultyById(id);
        if (faculty) {
            return faculty->name;
        }
    }
    return "Unknown";
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
    
    // Select borrower by name
    BorrowerInfo borrower = selectBorrowerByName(this);
    if (borrower.id == 0) {
        return; // User cancelled
    }
    
    // Calculate due date (14 days from now)
    QDate checkoutDate = QDate::currentDate();
    QDate dueDate = checkoutDate.addDays(14);
    
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    if (borrower.type == "Student") {
        query.prepare("INSERT INTO book_loans (book_id, student_id, checkout_date, due_date, status) "
                      "VALUES (:book_id, :student_id, :checkout_date, :due_date, 'Checked Out')");
        query.bindValue(":book_id", bookId);
        query.bindValue(":student_id", borrower.id);
    } else {
        query.prepare("INSERT INTO book_loans (book_id, faculty_id, checkout_date, due_date, status) "
                      "VALUES (:book_id, :faculty_id, :checkout_date, :due_date, 'Checked Out')");
        query.bindValue(":book_id", bookId);
        query.bindValue(":faculty_id", borrower.id);
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
            QString("'%1' has been checked out to %2 (%3).\n\nDue Date: %4")
            .arg(bookTitle).arg(borrower.name).arg(borrower.type).arg(dueDate.toString("MMMM dd, yyyy")));
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
    
    // Find active loans for this book with borrower names
    QSqlQuery findQuery(DatabaseManager::instance().getDatabase());
    findQuery.prepare("SELECT bl.loan_id, bl.student_id, bl.faculty_id, bl.due_date, "
                      "COALESCE(s.name, f.name) as borrower_name, "
                      "CASE WHEN bl.student_id IS NOT NULL THEN 'Student' ELSE 'Faculty' END as borrower_type "
                      "FROM book_loans bl "
                      "LEFT JOIN students s ON bl.student_id = s.student_id "
                      "LEFT JOIN faculty f ON bl.faculty_id = f.faculty_id "
                      "WHERE bl.book_id = :book_id AND bl.status = 'Checked Out' "
                      "ORDER BY bl.checkout_date DESC");
    findQuery.bindValue(":book_id", bookId);
    
    if (!findQuery.exec()) {
        QMessageBox::warning(this, "No Active Loan", 
            QString("No active loan found for '%1'.").arg(bookTitle));
        return;
    }
    
    // Collect all loans first
    QList<QPair<int, QPair<QDate, QString>>> loans;
    while (findQuery.next()) {
        loans.append(qMakePair(
            findQuery.value("loan_id").toInt(),
            qMakePair(
                findQuery.value("due_date").toDate(),
                findQuery.value("borrower_name").toString()
            )
        ));
    }
    
    if (loans.isEmpty()) {
        QMessageBox::warning(this, "No Active Loan", 
            QString("No active loan found for '%1'.").arg(bookTitle));
        return;
    }
    
    // If multiple loans, let user select which one to return
    int loanId = -1;
    QDate dueDate;
    QString borrowerName;
    
    if (loans.size() > 1) {
        // Multiple loans - show selection dialog
        QDialog selectDialog(this);
        selectDialog.setWindowTitle("Select Loan to Return");
        selectDialog.setModal(true);
        selectDialog.setMinimumSize(500, 400);
        
        auto layout = new QVBoxLayout(&selectDialog);
        layout->addWidget(new QLabel(QString("Multiple active loans found for '%1'. Select which one to return:").arg(bookTitle)));
        
        auto table = new QTableView(&selectDialog);
        auto model = new QStandardItemModel(&selectDialog);
        model->setColumnCount(3);
        model->setHorizontalHeaderLabels({"Borrower Name", "Type", "Due Date"});
        table->setModel(model);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setSelectionMode(QAbstractItemView::SingleSelection);
        table->horizontalHeader()->setStretchLastSection(true);
        table->verticalHeader()->setVisible(false);
        
        // Re-query to get borrower type
        QSqlQuery typeQuery(DatabaseManager::instance().getDatabase());
        typeQuery.prepare("SELECT bl.loan_id, bl.student_id, bl.faculty_id, bl.due_date, "
                          "COALESCE(s.name, f.name) as borrower_name, "
                          "CASE WHEN bl.student_id IS NOT NULL THEN 'Student' ELSE 'Faculty' END as borrower_type "
                          "FROM book_loans bl "
                          "LEFT JOIN students s ON bl.student_id = s.student_id "
                          "LEFT JOIN faculty f ON bl.faculty_id = f.faculty_id "
                          "WHERE bl.book_id = :book_id AND bl.status = 'Checked Out' "
                          "ORDER BY bl.checkout_date DESC");
        typeQuery.bindValue(":book_id", bookId);
        typeQuery.exec();
        
        while (typeQuery.next()) {
            int row = model->rowCount();
            model->insertRow(row);
            model->setItem(row, 0, new QStandardItem(typeQuery.value("borrower_name").toString()));
            model->setItem(row, 1, new QStandardItem(typeQuery.value("borrower_type").toString()));
            model->setItem(row, 2, new QStandardItem(typeQuery.value("due_date").toDate().toString("yyyy-MM-dd")));
            model->item(row, 0)->setData(typeQuery.value("loan_id").toInt(), Qt::UserRole);
            model->item(row, 0)->setData(typeQuery.value("due_date").toDate(), Qt::UserRole + 1);
        }
        layout->addWidget(table);
        
        auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &selectDialog);
        connect(buttonBox, &QDialogButtonBox::accepted, &selectDialog, &QDialog::accept);
        connect(buttonBox, &QDialogButtonBox::rejected, &selectDialog, &QDialog::reject);
        layout->addWidget(buttonBox);
        
        if (selectDialog.exec() == QDialog::Accepted) {
            auto selected = table->selectionModel()->selectedRows();
            if (!selected.isEmpty()) {
                int selectedRow = selected.first().row();
                loanId = model->item(selectedRow, 0)->data(Qt::UserRole).toInt();
                dueDate = model->item(selectedRow, 0)->data(Qt::UserRole + 1).toDate();
                borrowerName = model->item(selectedRow, 0)->text();
            } else {
                return; // No selection
            }
        } else {
            return; // User cancelled
        }
    } else {
        // Single loan
        loanId = findQuery.value("loan_id").toInt();
        dueDate = findQuery.value("due_date").toDate();
        borrowerName = findQuery.value("borrower_name").toString();
    }
    
    if (loanId == -1) {
        return;
    }
    
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
        
        QString message = QString("'%1' has been returned successfully.\nBorrower: %2").arg(bookTitle).arg(borrowerName);
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
