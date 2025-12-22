#include "newssystem.h"
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>

NewsSystem::NewsSystem(QWidget* parent) : QWidget(parent) {
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(20);
    
    // Header
    auto header = new QLabel("News & Announcements");
    QFont f = header->font();
    f.setPointSize(24);
    f.setBold(true);
    header->setFont(f);
    layout->addWidget(header);
    
    QScrollArea* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet("background: transparent;");
    
    m_contentContainer = new QWidget();
    m_contentContainer->setStyleSheet("background: transparent;");
    auto containerLayout = new QVBoxLayout(m_contentContainer);
    containerLayout->setSpacing(20);
    containerLayout->setContentsMargins(5, 5, 5, 20); // Margins for shadow
    
    scroll->setWidget(m_contentContainer);
    layout->addWidget(scroll);
    
    // Add Dummy Data
    addNewsItem("Fall Semester Registration Open", "Aug 15, 2025", 
                "Registration for the upcoming Fall semester is now open. Please login to the portal to select your courses. Dealing: Sept 1st.", "Important");
                
    addNewsItem("Library Maintenance Scheduled", "Aug 20, 2025", 
                "The main library will be closed for maintenance this weekend. Digital resources remain available. Returns can be made at the drop-box.", "Notice");
                
    addNewsItem("New Cafeteria Opening", "Sept 01, 2025", 
                "We are excited to announce the opening of the new Sustainability Cafeteria in Block B. 50% off on all salads for the first week!", "Event");

    addNewsItem("Guest Lecture: AI of Future", "Sept 05, 2025", 
                "Join us for a talk by Dr. Alan Turing (AI Simulation) on the future of generative agents.", "Event");
                
    containerLayout->addStretch();
}

void NewsSystem::addNewsItem(const QString& title, const QString& date, const QString& content, const QString& type) {
    QFrame* card = new QFrame();
    // Modern Card Style
    card->setStyleSheet(R"(
        QFrame { 
            background-color: white; 
            border-radius: 12px; 
            border: 1px solid #E5E5EA; 
        }
        QFrame:hover {
            border: 1px solid #007AFF;
        }
    )");
    
    auto layout = new QVBoxLayout(card);
    layout->setContentsMargins(20, 20, 20, 20);
    
    auto headerLayout = new QHBoxLayout();
    auto lblTitle = new QLabel(title);
    QFont titleFont = lblTitle->font();
    titleFont.setPixelSize(18);
    titleFont.setBold(true);
    lblTitle->setFont(titleFont);
    lblTitle->setStyleSheet("color: #1D1D1F; border: none; background: transparent;");
    
    auto lblType = new QLabel(type);
    QString typeBg = (type == "Important") ? "#FF3B30" : (type == "Notice" ? "#FF9500" : "#34C759");
    lblType->setStyleSheet(QString("background-color: %1; color: white; padding: 4px 12px; border-radius: 12px; font-weight: bold; font-size: 11px; border: none;").arg(typeBg));
    
    headerLayout->addWidget(lblTitle);
    headerLayout->addStretch();
    headerLayout->addWidget(lblType);
    
    auto lblDate = new QLabel(date);
    lblDate->setStyleSheet("color: #8E8E93; font-size: 13px; border: none; background: transparent; margin-bottom: 8px;");
    
    auto lblContent = new QLabel(content);
    lblContent->setWordWrap(true);
    lblContent->setStyleSheet("color: #3A3A3C; font-size: 15px; border: none; background: transparent; line-height: 1.4;");
    
    layout->addLayout(headerLayout);
    layout->addWidget(lblDate);
    layout->addWidget(lblContent);
    
    // Shadow
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 15));
    shadow->setOffset(0, 5);
    card->setGraphicsEffect(shadow);
    
    m_contentContainer->layout()->addWidget(card);
}
