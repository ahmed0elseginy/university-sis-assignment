#ifndef LIBRARYSYSTEM_H
#define LIBRARYSYSTEM_H

#include <QWidget>
#include <QTableView>
#include <QStandardItemModel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include "../basesystemwidget.h"

class LibrarySystem : public BaseSystemWidget {
    Q_OBJECT
public:
    explicit LibrarySystem(QWidget* parent = nullptr);
    void setUserContext(const QString& role, int userId);

private slots:
    void onSearch(const QString& text);
    void onAddBook();
    void onCheckOutBook();
    void onReturnBook();
    void onDeleteBook();
    void onFilterChanged(int index);

private:
    void setupUi();
    void loadBooks(const QString& filter = "");
    void refreshBooks();
    
    // Helper functions for name-based operations
    struct BorrowerInfo {
        int id = 0;
        QString name;
        QString type; // "Student" or "Faculty"
    };
    
    BorrowerInfo selectBorrowerByName(QWidget* parent);
    int getStudentIdByName(const QString& name);
    int getFacultyIdByName(const QString& name);
    QString getBorrowerNameById(int id, const QString& type);
    
    QTableView* m_view;
    QStandardItemModel* m_model;
    QLineEdit* m_search;
    QPushButton* m_addBtn;
    QPushButton* m_checkoutBtn;
    QPushButton* m_returnBtn;
    QPushButton* m_deleteBtn;
    QComboBox* m_filterCombo;
    
    QString m_currentUserRole;
    int m_currentUserId;
};

#endif // LIBRARYSYSTEM_H
