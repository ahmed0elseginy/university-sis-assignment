#include "profilewidget.h"
#include <QFrame>

ProfileWidget::ProfileWidget(const QString& username, const QString& role, QWidget* parent)
    : QWidget(parent)
{
    setupUi(username, role);
}

void ProfileWidget::setupUi(const QString& username, const QString& role)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);
    
    QFrame* card = new QFrame();
    card->setFixedWidth(400);
    card->setStyleSheet("QFrame { background-color: white; border-radius: 10px; border: 1px solid #ddd; }");
    
    QVBoxLayout* cardLayout = new QVBoxLayout(card);
    cardLayout->setSpacing(20);
    cardLayout->setContentsMargins(40, 40, 40, 40);
    
    // Avatar / Icon
    QLabel* icon = new QLabel("👤");
    icon->setAlignment(Qt::AlignCenter);
    icon->setStyleSheet("font-size: 64px; border: none;");
    cardLayout->addWidget(icon);
    
    // Info
    QLabel* userLabel = new QLabel(username);
    userLabel->setAlignment(Qt::AlignCenter);
    userLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #2c3e50; border: none;");
    cardLayout->addWidget(userLabel);
    
    QLabel* roleLabel = new QLabel(role);
    roleLabel->setAlignment(Qt::AlignCenter);
    roleLabel->setStyleSheet("font-size: 16px; color: #7f8c8d; border: none;");
    cardLayout->addWidget(roleLabel);
    
    // Divider
    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("background-color: #eee;");
    cardLayout->addWidget(line);
    
    // Logout Button
    QPushButton* logoutBtn = new QPushButton("Log Out");
    logoutBtn->setCursor(Qt::PointingHandCursor);
    logoutBtn->setStyleSheet(
        "QPushButton { "
        "   background-color: #e74c3c; "
        "   color: white; "
        "   border: none; "
        "   padding: 12px; "
        "   border-radius: 5px; "
        "   font-weight: bold; "
        "   font-size: 14px; "
        "}"
        "QPushButton:hover { background-color: #c0392b; }"
    );
    cardLayout->addWidget(logoutBtn);
    
    mainLayout->addWidget(card);
    
    connect(logoutBtn, &QPushButton::clicked, this, &ProfileWidget::logoutRequested);
}
