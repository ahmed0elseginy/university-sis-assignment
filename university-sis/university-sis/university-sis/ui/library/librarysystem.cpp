#include "librarysystem.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>

LibrarySystem::LibrarySystem(QWidget* parent) : QWidget(parent) {
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(20);
    
    auto header = new QLabel("Library System");
    QFont f = header->font();
    f.setPointSize(24);
    f.setBold(true);
    header->setFont(f);
    layout->addWidget(header);
    
    // Controls
    auto controlLayout = new QHBoxLayout();
    m_search = new QLineEdit();
    m_search->setPlaceholderText("Search for books, authors, or ISBN...");
    m_search->setFixedWidth(300);
    
    // Create label for controls
    auto searchLabel = new QLabel("Find a book:");
    
    controlLayout->addStretch();
    controlLayout->addWidget(searchLabel);
    controlLayout->addWidget(m_search);
    
    layout->addLayout(controlLayout);
    
    // Table
    m_view = new QTableView(this);
    m_model = new QStandardItemModel(this);
    m_model->setColumnCount(4);
    m_model->setHorizontalHeaderLabels({"ISBN", "Title", "Author", "Status"});
    m_view->setModel(m_model);
    m_view->verticalHeader()->setVisible(false);
    m_view->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_view->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view->setAlternatingRowColors(true);
    
    layout->addWidget(m_view);
    
    // Connect Search
    connect(m_search, &QLineEdit::textChanged, this, &LibrarySystem::onSearch);
    
    loadBooks();
}

void LibrarySystem::onSearch(const QString& text) {
    loadBooks(text);
}

void LibrarySystem::loadBooks(const QString& filter) {
    m_model->removeRows(0, m_model->rowCount());
    
    // Dummy Data
    struct Book { QString isbn; QString title; QString author; QString status; };
    QList<Book> books = {
        {"978-0131103627", "The C++ Programming Language", "Bjarne Stroustrup", "Available"},
        {"978-0201633610", "Design Patterns", "Erich Gamma et al.", "Checked Out"},
        {"978-0132350884", "Clean Code", "Robert C. Martin", "Available"},
        {"978-0321751041", "The Art of Computer Programming", "Donald Knuth", "Reference Only"},
        {"978-1449331818", "Learning React", "Alex Banks", "Available"},
        {"978-0596007126", "Head First Design Patterns", "Freeman & Robson", "Available"},
        {"978-0134685991", "Effective Java", "Joshua Bloch", "Available"},
        {"978-0135957059", "The Pragmatic Programmer", "Thomas & Hunt", "Available"}
    };
    
    for (const auto& b : books) {
        if (filter.isEmpty() || b.title.contains(filter, Qt::CaseInsensitive) || b.author.contains(filter, Qt::CaseInsensitive)) {
            QList<QStandardItem*> row;
            row << new QStandardItem(b.isbn);
            row << new QStandardItem(b.title);
            row << new QStandardItem(b.author);
            auto statusItem = new QStandardItem(b.status);
            
            if (b.status == "Available") {
                 statusItem->setForeground(QColor("#28cd41")); // iOS Green
                 statusItem->setFont(QFont("Segoe UI", 9, QFont::Bold));
            }
            else if (b.status == "Checked Out") {
                 statusItem->setForeground(QColor("#ff3b30")); // iOS Red
            }
            else {
                 statusItem->setForeground(QColor("#ff9500")); // iOS Orange
            }
            
            row << statusItem;
            m_model->appendRow(row);
        }
    }
}
