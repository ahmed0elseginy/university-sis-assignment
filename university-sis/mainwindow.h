#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QListWidget>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onMenuChanged(int index);
    void onToggleTheme();
    void logout();

private:
    void setupUi(const QString& username, const QString& role, int userId);

    QWidget *m_centralWidget;
    QListWidget *m_sidebar;
    QStackedWidget *m_contentArea;
    class QPushButton* m_themeBtn; // Forward declaration
    class DashboardWidget* m_dashboardWidget; // Forward declaration for dashboard
};
#endif // MAINWINDOW_H
