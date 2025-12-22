#include "thememanager.h"
#include <QPalette>
#include <QStyleFactory>

ThemeManager::AppTheme ThemeManager::s_currentTheme = ThemeManager::Light;

void ThemeManager::applyTheme(AppTheme theme)
{
    s_currentTheme = theme;
    qApp->setStyle(QStyleFactory::create("Fusion"));
    
    if (theme == Dark) {
        qApp->setStyleSheet(getDarkStyle());
    } else {
        qApp->setStyleSheet(getLightStyle());
    }
}

ThemeManager::AppTheme ThemeManager::currentTheme()
{
    return s_currentTheme;
}

void ThemeManager::toggleTheme()
{
    applyTheme(s_currentTheme == Light ? Dark : Light);
}

QString ThemeManager::getLightStyle()
{
    // macOS Light Theme Inspired
    return R"(
        QMainWindow, QWidget { 
            background-color: #F5F5F7; 
            font-family: 'Segoe UI', 'Helvetica Neue', sans-serif;
            font-size: 14px;
            color: #1D1D1F; 
        }
        
        /* Sidebar */
        QListWidget { 
            background-color: #EBEDF0; 
            border: none; 
            outline: none;
            padding-top: 20px;
        }
        QListWidget::item { 
            padding: 12px 24px; 
            margin: 4px 12px;
            border-radius: 8px;
            color: #424245; 
            font-weight: 500;
        }
        QListWidget::item:selected { 
            background-color: #FFFFFF; 
            color: #007AFF; 
            box-shadow: 0 1px 3px rgba(0,0,0,0.1);
        }
        QListWidget::item:hover:!selected { 
            background-color: rgba(255, 255, 255, 0.5); 
        }

        /* QStackedWidget Content Area */
        QStackedWidget {
            background-color: #FFFFFF;
            border-top-left-radius: 12px;
            border-bottom-left-radius: 12px; /* Smooth transition from sidebar */
        }

        /* Tables (DataGrid) */
        QTableView { 
            background-color: #FFFFFF; 
            gridline-color: #E5E5E5; 
            selection-background-color: #007AFF; 
            selection-color: white; 
            color: #1D1D1F; 
            border: 1px solid #E5E5E5; 
            border-radius: 8px;
            alternate-background-color: #FAFAFA;
        }
        QHeaderView::section { 
            background-color: #FFFFFF; 
            border: none; 
            border-bottom: 2px solid #F2F2F7;
            padding: 12px; 
            font-weight: 600; 
            color: #86868B; 
            text-transform: uppercase;
            font-size: 12px;
        }
        QTableView::item { padding: 8px; }

        /* Buttons (Soft macOS style) */
        QPushButton { 
            background-color: #FFFFFF; 
            border: 1px solid #D1D1D6; 
            border-radius: 6px; 
            padding: 8px 16px; 
            color: #1D1D1F; 
            font-weight: 500;
        }
        QPushButton:hover { 
            background-color: #F5F5F7; 
            border-color: #C7C7CC;
        }
        QPushButton:pressed { 
            background-color: #E5E5EA; 
        }
        
        /* Inputs */
        QLineEdit, QDateEdit, QSpinBox { 
            background-color: #FFFFFF; 
            border: 1px solid #D1D1D6; 
            border-radius: 8px; 
            padding: 8px 12px; 
            color: #1D1D1F; 
        }
        QLineEdit:focus, QDateEdit:focus, QSpinBox:focus { 
            border: 2px solid #007AFF; 
            background-color: #FFFFFF;
        }
        
        /* Scrollbars (Modern slim) */
        QScrollBar:vertical {
            border: none;
            background: transparent;
            width: 10px;
            margin: 0px;
        }
        QScrollBar::handle:vertical {
            background: #C7C7CC;
            min-height: 20px;
            border-radius: 5px;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }

        /* Tabs */
        QTabWidget::pane { border: none; }
        QTabBar::tab {
            padding: 8px 16px;
            margin-right: 4px;
            border-radius: 6px;
            background: #E5E5EA;
            color: #48484A;
            font-weight: 500;
        }
        QTabBar::tab:selected {
            background: #007AFF;
            color: white;
        }
        QTabBar::tab:hover:!selected {
            background: #D1D1D6;
        }

        /* Specialized Buttons */
        QPushButton[type="primary"] {
            background-color: #007AFF;
            color: white;
            border: none;
        }
        QPushButton[type="primary"]:hover { background-color: #0062CC; }

        QPushButton[type="success"] {
            background-color: #34C759;
            color: white;
            border: none;
        }
        QPushButton[type="success"]:hover { background-color: #248A3D; }

        QPushButton[type="danger"] {
            background-color: #FF3B30;
            color: white;
            border: none;
        }
        QPushButton[type="danger"]:hover { background-color: #D70015; }

        QPushButton[type="info"] {
            background-color: #E3F2FD;
            color: #007AFF;
            border: none;
        }
        QPushButton[type="info"]:hover { background-color: #D1E9FC; }
    )";
}

QString ThemeManager::getDarkStyle()
{
    // macOS Dark Theme Inspired
    return R"(
        QMainWindow, QWidget { 
            background-color: #1C1C1E; 
            font-family: 'Segoe UI', 'Helvetica Neue', sans-serif;
            font-size: 14px;
            color: #F5F5F7; 
        }
        
        /* Sidebar */
        QListWidget { 
            background-color: #2C2C2E; 
            border: none; 
            outline: none;
            padding-top: 20px;
        }
        QListWidget::item { 
            padding: 12px 24px; 
            margin: 4px 12px;
            border-radius: 8px;
            color: #AEAEB2; 
            font-weight: 500;
        }
        QListWidget::item:selected { 
            background-color: #3A3A3C; 
            color: #0A84FF; 
        }
        QListWidget::item:hover:!selected { 
            background-color: #3A3A3C; 
        }

        /* Content Area */
        QStackedWidget {
            background-color: #000000;
        }

        /* Tables */
        QTableView { 
            background-color: #1C1C1E; 
            gridline-color: #38383A; 
            selection-background-color: #0A84FF; 
            selection-color: white; 
            color: #F5F5F7; 
            border: 1px solid #38383A; 
            border-radius: 8px;
            alternate-background-color: #2C2C2E;
        }
        QHeaderView::section { 
            background-color: #1C1C1E; 
            border: none; 
            border-bottom: 2px solid #38383A; 
            padding: 12px; 
            font-weight: 600; 
            color: #8E8E93; 
            text-transform: uppercase;
            font-size: 12px;
        }
        QTableView::item { padding: 8px; }

        /* Buttons */
        QPushButton { 
            background-color: #2C2C2E; 
            border: 1px solid #3A3A3C; 
            border-radius: 6px; 
            padding: 8px 16px; 
            color: #F5F5F7; 
            font-weight: 500;
        }
        QPushButton:hover { 
            background-color: #3A3A3C; 
        }
        QPushButton:pressed { 
            background-color: #48484A; 
        }

        /* Tabs */
        QTabWidget::pane { border: none; }
        QTabBar::tab {
            padding: 8px 16px;
            margin-right: 4px;
            border-radius: 6px;
            background: #2C2C2E;
            color: #AEAEB2;
            font-weight: 500;
        }
        QTabBar::tab:selected {
            background: #0A84FF;
            color: white;
        }
        QTabBar::tab:hover:!selected {
            background: #3A3A3C;
        }

        /* Specialized Buttons */
        QPushButton[type="primary"] {
            background-color: #0A84FF;
            color: white;
            border: none;
        }
        QPushButton[type="primary"]:hover { background-color: #0071E3; }

        QPushButton[type="success"] {
            background-color: #30D158;
            color: black; /* Better contrast on bright green */
            border: none;
        }
        QPushButton[type="success"]:hover { background-color: #28CD41; }

        QPushButton[type="danger"] {
            background-color: #FF453A;
            color: white;
            border: none;
        }
        QPushButton[type="danger"]:hover { background-color: #FF3B30; }

        QPushButton[type="info"] {
            background-color: rgba(10, 132, 255, 0.15);
            color: #0A84FF;
            border: none;
        }
        QPushButton[type="info"]:hover { background-color: rgba(10, 132, 255, 0.25); }
        
        /* Inputs */
        QLineEdit, QDateEdit, QSpinBox { 
            background-color: #1C1C1E; 
            border: 1px solid #3A3A3C; 
            border-radius: 8px; 
            padding: 8px 12px; 
            color: #F5F5F7; 
        }
        QLineEdit:focus, QDateEdit:focus, QSpinBox:focus { 
            border: 2px solid #0A84FF; 
        }

        /* Scrollbars */
        QScrollBar:vertical {
            border: none;
            background: transparent;
            width: 10px;
        }
        QScrollBar::handle:vertical {
            background: #636366;
            min-height: 20px;
            border-radius: 5px;
        }
    )";
}
