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
    // macOS-inspired Modern Design - Clean, Simple, Elegant
    return R"(
        QMainWindow, QWidget { 
            background-color: #F5F5F7; 
            font-family: -apple-system, 'SF Pro Display', 'SF Pro Text', 'Helvetica Neue', 'Segoe UI', sans-serif;
            font-size: 13px;
            color: #1D1D1F; 
        }
        
        /* Sidebar - macOS Style */
        QListWidget { 
            background-color: #F5F5F7; 
            border: none; 
            outline: none;
            padding: 12px 8px;
        }
        QListWidget::item { 
            padding: 8px 16px; 
            margin: 2px 8px;
            border-radius: 8px;
            color: #1D1D1F; 
            font-weight: 400;
            font-size: 13px;
            min-height: 32px;
        }
        QListWidget::item:selected { 
            background-color: rgba(0, 122, 255, 0.1); 
            color: #007AFF; 
            font-weight: 500;
        }
        QListWidget::item:hover:!selected { 
            background-color: rgba(0, 0, 0, 0.04); 
        }

        /* Content Area - Clean macOS Style */
        QStackedWidget {
            background-color: #FFFFFF;
            border-radius: 0px;
        }

        /* Tables - macOS Clean Style */
        QTableView { 
            background-color: #FFFFFF; 
            gridline-color: #E5E5EA; 
            selection-background-color: #007AFF; 
            selection-color: white; 
            color: #1D1D1F; 
            border: none; 
            border-radius: 10px;
            alternate-background-color: #FAFAFA;
        }
        QHeaderView::section { 
            background-color: #F5F5F7; 
            border: none; 
            border-bottom: 1px solid #E5E5EA;
            padding: 10px 12px; 
            font-weight: 600; 
            color: #8E8E93; 
            font-size: 11px;
            text-transform: none;
        }
        QTableView::item { 
            padding: 8px 12px; 
            border: none;
        }
        QTableView::item:hover {
            background-color: rgba(0, 122, 255, 0.05);
        }

        /* Buttons - macOS Style */
        QPushButton { 
            background-color: #FFFFFF; 
            border: 0.5px solid #D1D1D6; 
            border-radius: 6px; 
            padding: 6px 16px; 
            color: #007AFF; 
            font-weight: 400;
            min-height: 28px;
            font-size: 13px;
        }
        QPushButton:hover { 
            background-color: #F5F5F7; 
            border-color: #C7C7CC;
        }
        QPushButton:pressed { 
            background-color: #E5E5EA; 
        }
        
        /* Inputs - macOS Style */
        QLineEdit, QDateEdit, QSpinBox, QComboBox, QTextEdit { 
            background-color: #FFFFFF; 
            border: 0.5px solid #D1D1D6; 
            border-radius: 6px; 
            padding: 6px 10px; 
            color: #1D1D1F; 
            font-size: 13px;
            selection-background-color: #007AFF;
            selection-color: white;
        }
        QLineEdit:focus, QDateEdit:focus, QSpinBox:focus, QComboBox:focus, QTextEdit:focus { 
            border: 0.5px solid #007AFF; 
            background-color: #FFFFFF;
            outline: none;
        }
        QLineEdit:hover, QDateEdit:hover, QSpinBox:hover, QComboBox:hover { 
            border-color: #C7C7CC;
        }
        
        /* ComboBox - macOS Style */
        QComboBox::drop-down {
            border: none;
            width: 24px;
        }
        QComboBox::down-arrow {
            width: 0;
            height: 0;
            border-left: 4px solid transparent;
            border-right: 4px solid transparent;
            border-top: 5px solid #8E8E93;
        }
        QComboBox QAbstractItemView {
            background-color: #FFFFFF;
            border: 0.5px solid #D1D1D6;
            border-radius: 8px;
            selection-background-color: #007AFF;
            selection-color: white;
            padding: 4px;
        }
        
        /* Scrollbars - macOS Style */
        QScrollBar:vertical {
            border: none;
            background: transparent;
            width: 11px;
            margin: 0px;
        }
        QScrollBar::handle:vertical {
            background: #C7C7CC;
            min-height: 20px;
            border-radius: 5px;
            margin: 2px;
        }
        QScrollBar::handle:vertical:hover {
            background: #AEAEB2;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }
        QScrollBar:horizontal {
            border: none;
            background: transparent;
            height: 11px;
            margin: 0px;
        }
        QScrollBar::handle:horizontal {
            background: #C7C7CC;
            min-width: 20px;
            border-radius: 5px;
            margin: 2px;
        }
        QScrollBar::handle:horizontal:hover {
            background: #AEAEB2;
        }

        /* Tabs - macOS Style */
        QTabWidget::pane { 
            border: none; 
            background-color: #FFFFFF;
            top: -1px;
        }
        QTabBar::tab {
            padding: 8px 16px;
            margin-right: 2px;
            border-top-left-radius: 6px;
            border-top-right-radius: 6px;
            background: transparent;
            color: #8E8E93;
            font-weight: 400;
            border: none;
        }
        QTabBar::tab:selected {
            background: transparent;
            color: #007AFF;
            font-weight: 500;
            border-bottom: 2px solid #007AFF;
        }
        QTabBar::tab:hover:!selected {
            color: #1D1D1F;
        }

        /* Specialized Buttons - macOS Colors */
        QPushButton[type="primary"] {
            background-color: #007AFF;
            color: white;
            border: none;
            font-weight: 400;
        }
        QPushButton[type="primary"]:hover { 
            background-color: #0051D5;
        }
        QPushButton[type="primary"]:pressed {
            background-color: #0040AA;
        }

        QPushButton[type="success"] {
            background-color: #34C759;
            color: white;
            border: none;
            font-weight: 400;
        }
        QPushButton[type="success"]:hover { background-color: #30B04A; }

        QPushButton[type="danger"] {
            background-color: #FF3B30;
            color: white;
            border: none;
            font-weight: 400;
        }
        QPushButton[type="danger"]:hover { background-color: #FF2D20; }

        QPushButton[type="info"] {
            background-color: rgba(0, 122, 255, 0.1);
            color: #007AFF;
            border: 0.5px solid rgba(0, 122, 255, 0.3);
            font-weight: 400;
        }
        QPushButton[type="info"]:hover { 
            background-color: rgba(0, 122, 255, 0.15); 
        }
        
        /* Labels - macOS Style */
        QLabel {
            color: #1D1D1F;
            font-weight: 400;
        }
        
        /* Group Box - macOS Style */
        QGroupBox {
            border: 0.5px solid #D1D1D6;
            border-radius: 12px;
            margin-top: 16px;
            padding-top: 20px;
            padding-bottom: 12px;
            font-weight: 500;
            color: #1D1D1F;
            background-color: #FFFFFF;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 16px;
            padding: 0 8px;
            background-color: #FFFFFF;
            color: #1D1D1F;
            font-size: 13px;
            font-weight: 600;
        }
        
        /* Calendar Widget - macOS Style */
        QCalendarWidget {
            background-color: #FFFFFF;
            border: 0.5px solid #D1D1D6;
            border-radius: 12px;
        }
        QCalendarWidget QTableView {
            alternate-background-color: #FAFAFA;
            selection-background-color: #007AFF;
            selection-color: white;
        }
        QCalendarWidget QAbstractItemView:enabled {
            color: #1D1D1F;
            selection-background-color: #007AFF;
            selection-color: white;
        }
        QCalendarWidget QAbstractItemView:disabled {
            color: #C7C7CC;
        }
        
        /* Checkbox & Radio - macOS Style */
        QCheckBox, QRadioButton {
            color: #1D1D1F;
            spacing: 8px;
            font-weight: 400;
        }
        QCheckBox::indicator, QRadioButton::indicator {
            width: 16px;
            height: 16px;
            border: 1.5px solid #8E8E93;
            border-radius: 4px;
            background-color: #FFFFFF;
        }
        QCheckBox::indicator:checked, QRadioButton::indicator:checked {
            background-color: #007AFF;
            border-color: #007AFF;
        }
        QCheckBox::indicator:hover, QRadioButton::indicator:hover {
            border-color: #007AFF;
        }
        QRadioButton::indicator {
            border-radius: 8px;
        }
        
        /* Dialog - macOS Style */
        QDialog {
            background-color: #FFFFFF;
        }
        
        /* Status Bar - macOS Style */
        QStatusBar {
            background-color: #F5F5F7;
            border-top: 0.5px solid #D1D1D6;
            color: #8E8E93;
        }
    )";
}

QString ThemeManager::getDarkStyle()
{
    // macOS Dark Mode - Clean, Modern Design
    return R"(
        QMainWindow, QWidget { 
            background-color: #1C1C1E; 
            font-family: -apple-system, 'SF Pro Display', 'SF Pro Text', 'Helvetica Neue', 'Segoe UI', sans-serif;
            font-size: 13px;
            color: #F5F5F7; 
        }
        
        /* Sidebar - macOS Dark */
        QListWidget { 
            background-color: #1C1C1E; 
            border: none; 
            outline: none;
            padding: 12px 8px;
        }
        QListWidget::item { 
            padding: 8px 16px; 
            margin: 2px 8px;
            border-radius: 8px;
            color: rgba(255, 255, 255, 0.85); 
            font-weight: 400;
            font-size: 13px;
            min-height: 32px;
        }
        QListWidget::item:selected { 
            background-color: rgba(0, 122, 255, 0.15); 
            color: #0A84FF; 
            font-weight: 500;
        }
        QListWidget::item:hover:!selected { 
            background-color: rgba(255, 255, 255, 0.08); 
        }

        /* Content Area */
        QStackedWidget {
            background-color: #000000;
            border-radius: 0px;
        }

        /* Tables - macOS Dark */
        QTableView { 
            background-color: #1C1C1E; 
            gridline-color: #38383A; 
            selection-background-color: #0A84FF; 
            selection-color: white; 
            color: #F5F5F7; 
            border: none; 
            border-radius: 10px;
            alternate-background-color: #2C2C2E;
        }
        QHeaderView::section { 
            background-color: #1C1C1E; 
            border: none; 
            border-bottom: 1px solid #38383A;
            padding: 10px 12px; 
            font-weight: 600; 
            color: #8E8E93; 
            font-size: 11px;
            text-transform: none;
        }
        QTableView::item { 
            padding: 8px 12px; 
            border: none;
        }
        QTableView::item:hover {
            background-color: rgba(10, 132, 255, 0.1);
        }

        /* Buttons - macOS Dark */
        QPushButton { 
            background-color: #2C2C2E; 
            border: 0.5px solid #3A3A3C; 
            border-radius: 6px; 
            padding: 6px 16px; 
            color: #0A84FF; 
            font-weight: 400;
            min-height: 28px;
            font-size: 13px;
        }
        QPushButton:hover { 
            background-color: #3A3A3C; 
            border-color: #48484A;
        }
        QPushButton:pressed { 
            background-color: #48484A; 
        }

        /* Tabs - macOS Dark */
        QTabWidget::pane { 
            border: none; 
            background-color: #1C1C1E;
            top: -1px;
        }
        QTabBar::tab {
            padding: 8px 16px;
            margin-right: 2px;
            border-top-left-radius: 6px;
            border-top-right-radius: 6px;
            background: transparent;
            color: #8E8E93;
            font-weight: 400;
            border: none;
        }
        QTabBar::tab:selected {
            background: transparent;
            color: #0A84FF;
            font-weight: 500;
            border-bottom: 2px solid #0A84FF;
        }
        QTabBar::tab:hover:!selected {
            color: #F5F5F7;
        }

        /* Specialized Buttons - macOS Dark */
        QPushButton[type="primary"] {
            background-color: #0A84FF;
            color: white;
            border: none;
            font-weight: 400;
        }
        QPushButton[type="primary"]:hover { 
            background-color: #0071E3;
        }
        QPushButton[type="primary"]:pressed {
            background-color: #0051D5;
        }

        QPushButton[type="success"] {
            background-color: #30D158;
            color: white;
            border: none;
            font-weight: 400;
        }
        QPushButton[type="success"]:hover { background-color: #28CD41; }

        QPushButton[type="danger"] {
            background-color: #FF453A;
            color: white;
            border: none;
            font-weight: 400;
        }
        QPushButton[type="danger"]:hover { background-color: #FF3B30; }

        QPushButton[type="info"] {
            background-color: rgba(10, 132, 255, 0.15);
            color: #0A84FF;
            border: 0.5px solid rgba(10, 132, 255, 0.3);
            font-weight: 400;
        }
        QPushButton[type="info"]:hover { 
            background-color: rgba(10, 132, 255, 0.2); 
        }
        
        /* Inputs - macOS Dark */
        QLineEdit, QDateEdit, QSpinBox, QComboBox, QTextEdit { 
            background-color: #2C2C2E; 
            border: 0.5px solid #3A3A3C; 
            border-radius: 6px; 
            padding: 6px 10px; 
            color: #F5F5F7; 
            font-size: 13px;
            selection-background-color: #0A84FF;
            selection-color: white;
        }
        QLineEdit:focus, QDateEdit:focus, QSpinBox:focus, QComboBox:focus, QTextEdit:focus { 
            border: 0.5px solid #0A84FF; 
            background-color: #2C2C2E;
        }
        QLineEdit:hover, QDateEdit:hover, QSpinBox:hover, QComboBox:hover { 
            border-color: #48484A;
        }
        
        /* ComboBox - macOS Dark */
        QComboBox::drop-down {
            border: none;
            width: 24px;
        }
        QComboBox::down-arrow {
            width: 0;
            height: 0;
            border-left: 4px solid transparent;
            border-right: 4px solid transparent;
            border-top: 5px solid #8E8E93;
        }
        QComboBox QAbstractItemView {
            background-color: #2C2C2E;
            border: 0.5px solid #3A3A3C;
            border-radius: 8px;
            selection-background-color: #0A84FF;
            selection-color: white;
            padding: 4px;
        }

        /* Scrollbars - macOS Dark */
        QScrollBar:vertical {
            border: none;
            background: transparent;
            width: 11px;
            margin: 0px;
        }
        QScrollBar::handle:vertical {
            background: #636366;
            min-height: 20px;
            border-radius: 5px;
            margin: 2px;
        }
        QScrollBar::handle:vertical:hover {
            background: #8E8E93;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }
        QScrollBar:horizontal {
            border: none;
            background: transparent;
            height: 11px;
            margin: 0px;
        }
        QScrollBar::handle:horizontal {
            background: #636366;
            min-width: 20px;
            border-radius: 5px;
            margin: 2px;
        }
        QScrollBar::handle:horizontal:hover {
            background: #8E8E93;
        }
        
        /* Labels - macOS Dark */
        QLabel {
            color: #F5F5F7;
            font-weight: 400;
        }
        
        /* Group Box - macOS Dark */
        QGroupBox {
            border: 0.5px solid #3A3A3C;
            border-radius: 12px;
            margin-top: 16px;
            padding-top: 20px;
            padding-bottom: 12px;
            font-weight: 500;
            color: #F5F5F7;
            background-color: #1C1C1E;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 16px;
            padding: 0 8px;
            background-color: #1C1C1E;
            color: #F5F5F7;
            font-size: 13px;
            font-weight: 600;
        }
        
        /* Calendar Widget - macOS Dark */
        QCalendarWidget {
            background-color: #1C1C1E;
            border: 0.5px solid #3A3A3C;
            border-radius: 12px;
        }
        QCalendarWidget QTableView {
            alternate-background-color: #2C2C2E;
            selection-background-color: #0A84FF;
            selection-color: white;
        }
        QCalendarWidget QAbstractItemView:enabled {
            color: #F5F5F7;
            selection-background-color: #0A84FF;
            selection-color: white;
        }
        QCalendarWidget QAbstractItemView:disabled {
            color: #48484A;
        }
        
        /* Checkbox & Radio - macOS Dark */
        QCheckBox, QRadioButton {
            color: #F5F5F7;
            spacing: 8px;
            font-weight: 400;
        }
        QCheckBox::indicator, QRadioButton::indicator {
            width: 16px;
            height: 16px;
            border: 1.5px solid #8E8E93;
            border-radius: 4px;
            background-color: #2C2C2E;
        }
        QCheckBox::indicator:checked, QRadioButton::indicator:checked {
            background-color: #0A84FF;
            border-color: #0A84FF;
        }
        QCheckBox::indicator:hover, QRadioButton::indicator:hover {
            border-color: #0A84FF;
        }
        QRadioButton::indicator {
            border-radius: 8px;
        }
        
        /* Dialog - macOS Dark */
        QDialog {
            background-color: #1C1C1E;
        }
        
        /* Status Bar - macOS Dark */
        QStatusBar {
            background-color: #1C1C1E;
            border-top: 0.5px solid #3A3A3C;
            color: #8E8E93;
        }
    )";
}
