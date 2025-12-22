#ifndef PROFILEWIDGET_H
#define PROFILEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

class ProfileWidget : public QWidget {
    Q_OBJECT
public:
    explicit ProfileWidget(const QString& username, const QString& role, QWidget* parent = nullptr);

signals:
    void logoutRequested();

private:
    void setupUi(const QString& username, const QString& role);
};

#endif // PROFILEWIDGET_H
