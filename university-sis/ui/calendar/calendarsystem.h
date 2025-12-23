#ifndef CALENDARSYSTEM_H
#define CALENDARSYSTEM_H

#include <QWidget>
#include <QCalendarWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTableView>
#include <QStandardItemModel>
#include <QLabel>
#include <QDateEdit>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QGroupBox>
#include "../basesystemwidget.h"

class CalendarSystem : public BaseSystemWidget
{
    Q_OBJECT

public:
    explicit CalendarSystem(QWidget *parent = nullptr);

private slots:
    void onCalendarSelectionChanged();
    void onDateSelected(const QDate &date);
    void onDateEditChanged(const QDate &date);
    void onAddEvent();
    void onDeleteEvent();
    void onEventTypeChanged(int index);
    void refreshEvents();

private:
    void setupUi();
    void loadEventsForDate(const QDate &date);
    
    QCalendarWidget *m_calendar;
    QTableView *m_eventsTable;
    QStandardItemModel *m_eventsModel;
    QDateEdit *m_dateEdit;
    QLineEdit *m_titleEdit;
    QTextEdit *m_descriptionEdit;
    QComboBox *m_typeCombo;
    QPushButton *m_addBtn;
    QPushButton *m_deleteBtn;
    QLabel *m_selectedDateLabel;
};

#endif // CALENDARSYSTEM_H

