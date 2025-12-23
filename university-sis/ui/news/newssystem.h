#ifndef NEWSSYSTEM_H
#define NEWSSYSTEM_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>

class NewsSystem : public QWidget {
    Q_OBJECT
public:
    explicit NewsSystem(QWidget* parent = nullptr);
    void refreshNews();

private:
    void addNewsItem(const QString& title, const QString& date, const QString& content, const QString& type);
    void loadEventsFromDatabase();
    void clearNews();
    QWidget* m_contentContainer;
    QVBoxLayout* m_containerLayout;
};

#endif // NEWSSYSTEM_H
