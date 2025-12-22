#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QString>
#include <QApplication>

class ThemeManager {
public:
    enum AppTheme { Light, Dark };

    static void applyTheme(AppTheme theme);
    static AppTheme currentTheme();
    static void toggleTheme();

private:
    static AppTheme s_currentTheme;
    static QString getLightStyle();
    static QString getDarkStyle();
};

#endif // THEMEMANAGER_H
