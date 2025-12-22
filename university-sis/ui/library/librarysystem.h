#ifndef LIBRARYSYSTEM_H
#define LIBRARYSYSTEM_H

#include <QWidget>
#include <QTableView>
#include <QStandardItemModel>
#include <QLineEdit>

class LibrarySystem : public QWidget {
    Q_OBJECT
public:
    explicit LibrarySystem(QWidget* parent = nullptr);

private slots:
    void onSearch(const QString& text);

private:
    QTableView* m_view;
    QStandardItemModel* m_model;
    QLineEdit* m_search;
    
    void loadBooks(const QString& filter = "");
};

#endif // LIBRARYSYSTEM_H
