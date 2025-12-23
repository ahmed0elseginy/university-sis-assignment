#include "newssystem.h"
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QSqlQuery>
#include <QSqlError>
#include <QDate>
#include <QPushButton>
#include "../../database/databasemanager.h"

NewsSystem::NewsSystem(QWidget* parent) : QWidget(parent) {
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(40, 30, 40, 30);
    layout->setSpacing(25);
    
    // Header Section with Refresh Button
    auto headerLayout = new QHBoxLayout();
    headerLayout->setContentsMargins(0, 0, 0, 0);
    
    auto headerWidget = new QWidget();
    headerWidget->setStyleSheet("background: transparent;");
    auto headerVLayout = new QVBoxLayout(headerWidget);
    headerVLayout->setContentsMargins(0, 0, 0, 0);
    headerVLayout->setSpacing(4);
    
    auto header = new QLabel("News & Announcements");
    QFont f = header->font();
    f.setPointSize(28);
    f.setBold(true);
    f.setWeight(QFont::Bold);
    header->setFont(f);
    header->setStyleSheet("color: #1D1D1F; background: transparent;");
    headerVLayout->addWidget(header);
    
    auto subtitle = new QLabel("Stay updated with the latest university news and events");
    subtitle->setStyleSheet("color: #8E8E93; font-size: 13px; background: transparent;");
    headerVLayout->addWidget(subtitle);
    
    headerLayout->addWidget(headerWidget);
    headerLayout->addStretch();
    
    // Refresh Button
    auto refreshBtn = new QPushButton("Refresh");
    refreshBtn->setCursor(Qt::PointingHandCursor);
    refreshBtn->setProperty("type", "info");
    refreshBtn->setFixedSize(100, 36);
    refreshBtn->setToolTip("Refresh news and announcements");
    refreshBtn->setStyleSheet(
        "QPushButton { "
        "   background-color: rgba(0, 122, 255, 0.1); "
        "   color: #007AFF; "
        "   border: 0.5px solid rgba(0, 122, 255, 0.3); "
        "   border-radius: 8px; "
        "   font-weight: 500; "
        "   font-size: 13px; "
        "   padding: 8px 16px; "
        "}"
        "QPushButton:hover { "
        "   background-color: rgba(0, 122, 255, 0.15); "
        "   border-color: rgba(0, 122, 255, 0.5); "
        "}"
        "QPushButton:pressed { "
        "   background-color: rgba(0, 122, 255, 0.2); "
        "}"
    );
    connect(refreshBtn, &QPushButton::clicked, this, &NewsSystem::refreshNews);
    headerLayout->addWidget(refreshBtn);
    
    layout->addLayout(headerLayout);
    
    // Divider
    auto divider = new QFrame();
    divider->setFrameShape(QFrame::HLine);
    divider->setFrameShadow(QFrame::Sunken);
    divider->setStyleSheet("QFrame { border: none; border-top: 1px solid #E5E5EA; margin: 10px 0; }");
    layout->addWidget(divider);
    
    // Scroll Area for News Items
    QScrollArea* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet("QScrollArea { background: transparent; border: none; }");
    
    m_contentContainer = new QWidget();
    m_contentContainer->setStyleSheet("background: transparent;");
    m_containerLayout = new QVBoxLayout(m_contentContainer);
    m_containerLayout->setSpacing(16);
    m_containerLayout->setContentsMargins(0, 0, 0, 20);
    
    scroll->setWidget(m_contentContainer);
    layout->addWidget(scroll, 1);
    
    // Load events from database
    loadEventsFromDatabase();
    
    m_containerLayout->addStretch();
}

void NewsSystem::loadEventsFromDatabase()
{
    clearNews();
    
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    // Cross-database compatible date query - show events from last 7 days or future events
    QString dateFilter;
    if (DatabaseManager::instance().getDatabase().driverName() == "QSQLITE") {
        dateFilter = "date >= DATE('now', '-7 days')";
    } else {
        dateFilter = "date >= DATE_SUB(CURDATE(), INTERVAL 7 DAY)";
    }
    query.prepare(QString("SELECT title, date, time, type, description FROM calendar_events "
                  "WHERE %1 "
                  "ORDER BY date DESC, time DESC LIMIT 20").arg(dateFilter));
    
    if (query.exec()) {
        while (query.next()) {
            QString title = query.value("title").toString();
            QDate eventDate = query.value("date").toDate();
            QString time = query.value("time").toString();
            QString type = query.value("type").toString();
            QString description = query.value("description").toString();
            
            QString dateStr = eventDate.toString("MMM dd, yyyy");
            if (!time.isEmpty()) {
                dateStr += " at " + time;
            }
            
            // Map calendar event types to news types
            QString newsType = "Event";
            if (type == "Exam" || type == "Assignment Due") {
                newsType = "Important";
            } else if (type == "Holiday" || type == "Meeting") {
                newsType = "Notice";
            }
            
            QString content = description.isEmpty() ? 
                QString("Event scheduled for %1").arg(dateStr) : description;
            
            addNewsItem(title, dateStr, content, newsType);
        }
    }
    
    // If no events, show welcome message
    if (m_containerLayout->count() == 1) { // Only stretch
        addNewsItem("Welcome to University SIS", QDate::currentDate().toString("MMMM dd, yyyy"), 
                    "Stay updated with the latest news and events. Calendar events will appear here automatically when you add them through the Calendar section.", "Notice");
    }
}

void NewsSystem::clearNews()
{
    QLayoutItem* item;
    while ((item = m_containerLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
}

void NewsSystem::refreshNews()
{
    loadEventsFromDatabase();
}

void NewsSystem::addNewsItem(const QString& title, const QString& date, const QString& content, const QString& type) {
    QFrame* card = new QFrame();
    card->setCursor(Qt::PointingHandCursor);
    
    // Enhanced macOS-style Card
    card->setStyleSheet(R"(
        QFrame { 
            background-color: #FFFFFF; 
            border-radius: 12px; 
            border: 0.5px solid #D1D1D6; 
        }
        QFrame:hover {
            border: 0.5px solid #007AFF;
            background-color: #FAFAFA;
        }
    )");
    
    auto layout = new QVBoxLayout(card);
    layout->setContentsMargins(24, 20, 24, 20);
    layout->setSpacing(12);
    
    // Header with Type Badge and Title
    auto headerLayout = new QHBoxLayout();
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(12);
    
    // Type Badge (on left for better visual hierarchy)
    auto lblType = new QLabel(type);
    QString typeBg, typeColor;
    if (type == "Important") {
        typeBg = "#FF3B30";
        typeColor = "white";
    } else if (type == "Notice") {
        typeBg = "#FF9500";
        typeColor = "white";
    } else {
        typeBg = "#34C759";
        typeColor = "white";
    }
    lblType->setStyleSheet(QString(
        "background-color: %1; "
        "color: %2; "
        "padding: 6px 14px; "
        "border-radius: 14px; "
        "font-weight: 600; "
        "font-size: 11px; "
        "border: none; "
        "min-width: 60px; "
    ).arg(typeBg).arg(typeColor));
    lblType->setAlignment(Qt::AlignCenter);
    headerLayout->addWidget(lblType);
    
    // Title
    auto lblTitle = new QLabel(title);
    QFont titleFont = lblTitle->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleFont.setWeight(QFont::Bold);
    lblTitle->setFont(titleFont);
    lblTitle->setStyleSheet("color: #1D1D1F; border: none; background: transparent;");
    lblTitle->setWordWrap(true);
    headerLayout->addWidget(lblTitle, 1);
    
    layout->addLayout(headerLayout);
    
    // Date with icon-like styling
    auto dateLayout = new QHBoxLayout();
    dateLayout->setContentsMargins(0, 0, 0, 0);
    dateLayout->setSpacing(8);
    
    auto lblDate = new QLabel(date);
    lblDate->setStyleSheet(
        "color: #8E8E93; "
        "font-size: 12px; "
        "border: none; "
        "background: transparent; "
        "font-weight: 400;"
    );
    dateLayout->addWidget(lblDate);
    dateLayout->addStretch();
    layout->addLayout(dateLayout);
    
    // Content with better typography
    auto lblContent = new QLabel(content);
    lblContent->setWordWrap(true);
    lblContent->setStyleSheet(
        "color: #1D1D1F; "
        "font-size: 14px; "
        "border: none; "
        "background: transparent; "
        "line-height: 1.5; "
        "padding-top: 4px;"
    );
    layout->addWidget(lblContent);
    
    // Enhanced Shadow Effect
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(12);
    shadow->setColor(QColor(0, 0, 0, 8));
    shadow->setOffset(0, 2);
    card->setGraphicsEffect(shadow);
    
    m_containerLayout->addWidget(card);
}
