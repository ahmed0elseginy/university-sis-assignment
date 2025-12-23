#include "basesystemwidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QFont>

BaseSystemWidget::BaseSystemWidget(const QString& title, QWidget* parent) : QWidget(parent) {
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(40, 30, 40, 30);
    layout->setSpacing(25);
    
    // Modern Header Section
    auto headerContainer = new QWidget();
    headerContainer->setStyleSheet("background: transparent;");
    auto headerLayout = new QVBoxLayout(headerContainer);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(8);
    
    auto header = new QLabel(title);
    QFont f = header->font();
    f.setPointSize(32);
    f.setBold(true);
    f.setWeight(QFont::Bold);
    header->setFont(f);
    header->setStyleSheet("color: #1E3A5F; font-weight: 800; background: transparent;");
    headerLayout->addWidget(header);
    
    auto divider = new QFrame();
    divider->setFrameShape(QFrame::HLine);
    divider->setFrameShadow(QFrame::Sunken);
    divider->setStyleSheet("QFrame { border: none; border-top: 2px solid #DEE2E6; margin: 5px 0; }");
    headerLayout->addWidget(divider);
    
    layout->addWidget(headerContainer);
    
    // Modern Placeholder Content Card
    auto contentCard = new QWidget();
    contentCard->setStyleSheet(
        "QWidget {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #FFFFFF, stop:1 #F8F9FA);"
        "   border: 1px solid #DEE2E6;"
        "   border-radius: 16px;"
        "}"
    );
    
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(contentCard);
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 15));
    shadow->setOffset(0, 4);
    contentCard->setGraphicsEffect(shadow);
    
    auto contentLayout = new QVBoxLayout(contentCard);
    contentLayout->setContentsMargins(40, 40, 40, 40);
    contentLayout->setSpacing(15);
    
    auto content = new QLabel("Module: " + title);
    content->setAlignment(Qt::AlignCenter);
    content->setStyleSheet(
        "QLabel { "
        "   color: #495057; "
        "   font-size: 18px; "
        "   font-weight: 600; "
        "   background: transparent; "
        "   padding: 20px; "
        "}"
    );
    contentLayout->addWidget(content);
    
    auto subContent = new QLabel("Select an action from the menu to get started.");
    subContent->setAlignment(Qt::AlignCenter);
    subContent->setStyleSheet(
        "QLabel { "
        "   color: #6C757D; "
        "   font-size: 14px; "
        "   background: transparent; "
        "}"
    );
    contentLayout->addWidget(subContent);
    
    layout->addWidget(contentCard, 1);
    layout->addStretch();
}
