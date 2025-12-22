#ifndef BASESYSTEMWIDGET_H
#define BASESYSTEMWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include "../utils/thememanager.h"

// A Helper base class for all new system modules to ensure consistent look
class BaseSystemWidget : public QWidget {
    Q_OBJECT
public:
    explicit BaseSystemWidget(const QString& title, QWidget* parent = nullptr);
};

#endif // BASESYSTEMWIDGET_H
