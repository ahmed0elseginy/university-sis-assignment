#include "calendarsystem.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QTime>
#include <QStringList>
#include "../../database/databasemanager.h"
#include <QDate>

CalendarSystem::CalendarSystem(QWidget *parent)
    : BaseSystemWidget("Calendar & Schedule", parent)
{
    setupUi();
    refreshEvents();
}

void CalendarSystem::setupUi()
{
    auto mainLayout = qobject_cast<QVBoxLayout*>(layout());
    if (!mainLayout) return;
    
    // Calendar and Events Layout
    auto contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(20);
    
    // Left: Calendar Widget
    auto calendarGroup = new QGroupBox("Calendar");
    auto calendarLayout = new QVBoxLayout(calendarGroup);
    calendarLayout->setSpacing(12);
    
    m_calendar = new QCalendarWidget(this);
    m_calendar->setGridVisible(true);
    m_calendar->setNavigationBarVisible(true);
    m_calendar->setMinimumHeight(300);
    calendarLayout->addWidget(m_calendar);
    
    m_selectedDateLabel = new QLabel("Selected: " + QDate::currentDate().toString("MMMM dd, yyyy"));
    m_selectedDateLabel->setStyleSheet("font-size: 13px; color: #8E8E93; padding: 8px;");
    calendarLayout->addWidget(m_selectedDateLabel);
    
    contentLayout->addWidget(calendarGroup, 1);
    
    // Right: Events List and Form
    auto rightLayout = new QVBoxLayout();
    rightLayout->setSpacing(15);
    
    // Events Table
    auto eventsGroup = new QGroupBox("Events for Selected Date");
    auto eventsLayout = new QVBoxLayout(eventsGroup);
    
    m_eventsTable = new QTableView(this);
    m_eventsModel = new QStandardItemModel(0, 3, this);
    QStringList headers;
    headers << "Time" << "Title" << "Type";
    m_eventsModel->setHorizontalHeaderLabels(headers);
    m_eventsTable->setModel(m_eventsModel);
    m_eventsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_eventsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_eventsTable->horizontalHeader()->setStretchLastSection(true);
    m_eventsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_eventsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_eventsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_eventsTable->verticalHeader()->setVisible(false);
    m_eventsTable->setAlternatingRowColors(true);
    m_eventsTable->setShowGrid(false);
    m_eventsTable->setSortingEnabled(true);
    eventsLayout->addWidget(m_eventsTable);
    
    rightLayout->addWidget(eventsGroup, 2);
    
    // Add Event Form
    auto formGroup = new QGroupBox("Add New Event");
    auto formLayout = new QVBoxLayout(formGroup);
    formLayout->setSpacing(10);
    
    auto dateLayout = new QHBoxLayout();
    dateLayout->addWidget(new QLabel("Date:"));
    m_dateEdit = new QDateEdit(this);
    m_dateEdit->setDate(QDate::currentDate());
    m_dateEdit->setCalendarPopup(true);
    m_dateEdit->setDisplayFormat("MMM dd, yyyy");
    dateLayout->addWidget(m_dateEdit);
    dateLayout->addStretch();
    formLayout->addLayout(dateLayout);
    
    auto titleLayout = new QHBoxLayout();
    titleLayout->addWidget(new QLabel("Title:"));
    m_titleEdit = new QLineEdit(this);
    m_titleEdit->setPlaceholderText("Event title");
    titleLayout->addWidget(m_titleEdit);
    formLayout->addLayout(titleLayout);
    
    auto typeLayout = new QHBoxLayout();
    typeLayout->addWidget(new QLabel("Type:"));
    m_typeCombo = new QComboBox(this);
    m_typeCombo->addItems({"Class", "Exam", "Assignment Due", "Meeting", "Holiday", "Other"});
    typeLayout->addWidget(m_typeCombo);
    formLayout->addLayout(typeLayout);
    
    auto descLayout = new QVBoxLayout();
    descLayout->addWidget(new QLabel("Description:"));
    m_descriptionEdit = new QTextEdit(this);
    m_descriptionEdit->setPlaceholderText("Event description");
    m_descriptionEdit->setMaximumHeight(80);
    descLayout->addWidget(m_descriptionEdit);
    formLayout->addLayout(descLayout);
    
    auto buttonLayout = new QHBoxLayout();
    m_addBtn = new QPushButton("Add Event", this);
    m_addBtn->setProperty("type", "primary");
    m_deleteBtn = new QPushButton("Delete Selected", this);
    m_deleteBtn->setProperty("type", "danger");
    buttonLayout->addWidget(m_addBtn);
    buttonLayout->addWidget(m_deleteBtn);
    buttonLayout->addStretch();
    formLayout->addLayout(buttonLayout);
    
    rightLayout->addWidget(formGroup, 1);
    
    contentLayout->addLayout(rightLayout, 1);
    
    mainLayout->addLayout(contentLayout);
    
    // Connections
    connect(m_calendar, &QCalendarWidget::selectionChanged, this, &CalendarSystem::onCalendarSelectionChanged);
    connect(m_dateEdit, &QDateEdit::dateChanged, this, &CalendarSystem::onDateEditChanged);
    connect(m_addBtn, &QPushButton::clicked, this, &CalendarSystem::onAddEvent);
    connect(m_deleteBtn, &QPushButton::clicked, this, &CalendarSystem::onDeleteEvent);
    
    // Set initial date
    onDateSelected(QDate::currentDate());
}

void CalendarSystem::onCalendarSelectionChanged()
{
    QDate date = m_calendar->selectedDate();
    onDateSelected(date);
}

void CalendarSystem::onDateSelected(const QDate &date)
{
    m_selectedDateLabel->setText("Selected: " + date.toString("MMMM dd, yyyy"));
    m_dateEdit->setDate(date);
    loadEventsForDate(date);
}

void CalendarSystem::onDateEditChanged(const QDate &date)
{
    m_calendar->setSelectedDate(date);
}

void CalendarSystem::loadEventsForDate(const QDate &date)
{
    m_eventsModel->removeRows(0, m_eventsModel->rowCount());
    
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT id, time, title, type FROM calendar_events WHERE date = :date ORDER BY time");
    query.bindValue(":date", date.toString("yyyy-MM-dd"));
    
    if (query.exec()) {
        while (query.next()) {
            int row = m_eventsModel->rowCount();
            m_eventsModel->insertRow(row);
            m_eventsModel->setItem(row, 0, new QStandardItem(query.value("time").toString()));
            m_eventsModel->setItem(row, 1, new QStandardItem(query.value("title").toString()));
            m_eventsModel->setItem(row, 2, new QStandardItem(query.value("type").toString()));
            m_eventsModel->item(row, 2)->setData(query.value("id").toInt(), Qt::UserRole);
        }
    }
}

void CalendarSystem::onAddEvent()
{
    if (m_titleEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", 
            "Please enter an event title.\n\nEvent titles cannot be empty.");
        m_titleEdit->setFocus();
        return;
    }
    
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("INSERT INTO calendar_events (date, time, title, type, description) "
                  "VALUES (:date, :time, :title, :type, :description)");
    query.bindValue(":date", m_dateEdit->date().toString("yyyy-MM-dd"));
    query.bindValue(":time", QTime::currentTime().toString("HH:mm"));
    query.bindValue(":title", m_titleEdit->text().trimmed());
    query.bindValue(":type", m_typeCombo->currentText());
    query.bindValue(":description", m_descriptionEdit->toPlainText());
    
    if (query.exec()) {
        QMessageBox::information(this, "Success", 
            QString("Event '%1' has been added successfully.").arg(m_titleEdit->text()));
        m_titleEdit->clear();
        m_descriptionEdit->clear();
        m_typeCombo->setCurrentIndex(0);
        loadEventsForDate(m_dateEdit->date());
        refreshEvents();
    } else {
        QMessageBox::critical(this, "Database Error", 
            QString("Failed to add event.\n\nError: %1").arg(query.lastError().text()));
    }
}

void CalendarSystem::onDeleteEvent()
{
    auto selection = m_eventsTable->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, "No Selection", 
            "Please select an event from the list to delete.");
        return;
    }
    
    int row = selection.first().row();
    QString eventTitle = m_eventsModel->item(row, 1)->text();
    int eventId = m_eventsModel->item(row, 2)->data(Qt::UserRole).toInt();
    
    int ret = QMessageBox::question(this, "Confirm Delete", 
        QString("Are you sure you want to delete the event '%1'?\n\nThis action cannot be undone.").arg(eventTitle),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    
    if (ret != QMessageBox::Yes) {
        return;
    }
    
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("DELETE FROM calendar_events WHERE id = :id");
    query.bindValue(":id", eventId);
    
    if (query.exec()) {
        QMessageBox::information(this, "Success", 
            QString("Event '%1' has been deleted successfully.").arg(eventTitle));
        loadEventsForDate(m_dateEdit->date());
        refreshEvents();
    } else {
        QMessageBox::critical(this, "Database Error", 
            QString("Failed to delete event.\n\nError: %1").arg(query.lastError().text()));
    }
}

void CalendarSystem::onEventTypeChanged(int index)
{
    // Could filter events by type if needed
    Q_UNUSED(index);
}

void CalendarSystem::refreshEvents()
{
    // Refresh calendar highlighting - calendar updates automatically
    // No need to call protected updateCells() method
}

