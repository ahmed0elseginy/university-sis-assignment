#include "basesystemwidget.h"
#include <QVBoxLayout>
#include <QLabel>

BaseSystemWidget::BaseSystemWidget(const QString& title, QWidget* parent) : QWidget(parent) {
    auto layout = new QVBoxLayout(this);
    // Header
    auto header = new QLabel(title);
    QFont f = header->font();
    f.setPointSize(24);
    f.setBold(true);
    header->setFont(f);
    layout->addWidget(header);
    
    // Placeholder Content
    auto content = new QLabel("Module Loaded: " + title + "\n\nSelect an action from the menu (Future Implementation)");
    content->setAlignment(Qt::AlignCenter);
    content->setStyleSheet("QLabel { color: #7f8c8d; font-size: 16px; border: 2px dashed #bdc3c7; border-radius: 10px; }");
    layout->addWidget(content, 1);
}
