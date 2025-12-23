#include "mainwindow.h"
#include "database/databasemanager.h"
#include "ui/studentportal.h"
#include "ui/academic/academicsystem.h"
#include "ui/finance/financesystem.h"
#include "ui/enrollment/enrollmentsystem.h"
#include "ui/attendance/attendancesystem.h"
#include "ui/faculty/facultysystem.h"
#include "ui/facility/facilitysystem.h"
#include "ui/login/logindialog.h"
#include "ui/profile/profilewidget.h"
#include "ui/library/librarysystem.h"
#include "ui/news/newssystem.h"
#include "ui/calendar/calendarsystem.h"
#include "ui/grades/gradessystem.h"
#include "ui/reports/reportssystem.h"
#include "utils/thememanager.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QStatusBar>
#include <QPushButton>
#include <QTimer>
#include <QPainter>
#include <QPainterPath>
#include <QSqlQuery>
#include <QSqlError>
#include <QApplication>

#include <QGraphicsDropShadowEffect>

// Modern Dashboard Widget with Statistics Cards
class DashboardWidget : public QWidget {
public:
    explicit DashboardWidget(const QString& role, QWidget* parent = nullptr) : QWidget(parent), m_role(role) {
        setupDashboard();
    }
    
    void refreshDashboard() {
        // Clear existing widgets
        QLayoutItem* item;
        while ((item = layout()->takeAt(0)) != nullptr) {
            if (item->widget()) {
                item->widget()->deleteLater();
            }
            delete item;
        }
        setupDashboard();
    }

private:
    void setupDashboard() {
        auto mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(40, 30, 40, 30);
        mainLayout->setSpacing(30);
        
        // Header Section
        auto headerLayout = new QHBoxLayout();
        auto headerWidget = new QWidget();
        headerWidget->setStyleSheet("background: transparent;");
        auto headerVLayout = new QVBoxLayout(headerWidget);
        headerVLayout->setContentsMargins(0, 0, 0, 0);
        
        auto welcomeLabel = new QLabel("Welcome back");
        welcomeLabel->setStyleSheet("font-size: 13px; color: #8E8E93; font-weight: 400; background: transparent;");
        headerVLayout->addWidget(welcomeLabel);
        
        auto titleLabel = new QLabel("University Management");
        titleLabel->setStyleSheet("font-size: 28px; font-weight: 600; color: #1D1D1F; background: transparent; margin-top: 4px;");
        headerVLayout->addWidget(titleLabel);
        
        auto roleLabel = new QLabel(m_role);
        roleLabel->setStyleSheet("font-size: 15px; color: #8E8E93; font-weight: 400; background: transparent; margin-top: 4px;");
        headerVLayout->addWidget(roleLabel);
        
        headerLayout->addWidget(headerWidget);
        headerLayout->addStretch();
        
        // Refresh Button - More Prominent
        auto refreshBtn = new QPushButton(" Reload Data");
        refreshBtn->setCursor(Qt::PointingHandCursor);
        refreshBtn->setProperty("type", "primary");
        refreshBtn->setFixedSize(140, 40);
        refreshBtn->setToolTip("Click to refresh all dashboard statistics with latest data from database");
        refreshBtn->setStyleSheet(
            "QPushButton { "
            "   background-color: #007AFF; "
            "   color: white; "
            "   border: none; "
            "   border-radius: 8px; "
            "   font-weight: 500; "
            "   font-size: 13px; "
            "   padding: 10px 20px; "
            "}"
            "QPushButton:hover { "
            "   background-color: #0051D5; "
            "}"
            "QPushButton:pressed { "
            "   background-color: #0040AA; "
            "}"
        );
        connect(refreshBtn, &QPushButton::clicked, this, [this, refreshBtn]() {
            // Disable button during refresh to prevent multiple clicks
            refreshBtn->setEnabled(false);
            refreshBtn->setText("⏳ Loading...");
            QApplication::processEvents(); // Update UI immediately
            
            // Refresh dashboard
            refreshDashboard();
            
            // Re-enable button after refresh
            refreshBtn->setText("🔄 Reload Data");
            refreshBtn->setEnabled(true);
        });
        headerLayout->addWidget(refreshBtn);
        
        mainLayout->addLayout(headerLayout);
        
        // Statistics Cards Grid
        auto statsLayout = new QGridLayout();
        statsLayout->setSpacing(20);
        
        // Create stat cards - macOS style
        auto createStatCard = [](const QString& title, const QString& value) -> QWidget* {
            auto card = new QWidget();
            card->setStyleSheet(
                "QWidget {"
                "   background-color: #FFFFFF;"
                "   border: 0.5px solid #D1D1D6;"
                "   border-radius: 10px;"
                "}"
            );
            
            QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(card);
            shadow->setBlurRadius(10);
            shadow->setColor(QColor(0, 0, 0, 8));
            shadow->setOffset(0, 2);
            card->setGraphicsEffect(shadow);
            
            auto cardLayout = new QVBoxLayout(card);
            cardLayout->setContentsMargins(20, 18, 20, 18);
            cardLayout->setSpacing(8);
            
            auto valueLabel = new QLabel(value);
            valueLabel->setStyleSheet("font-size: 32px; font-weight: 600; color: #1D1D1F; background: transparent;");
            cardLayout->addWidget(valueLabel);
            
            auto titleLabel = new QLabel(title);
            titleLabel->setStyleSheet("font-size: 13px; color: #8E8E93; font-weight: 400; background: transparent;");
            cardLayout->addWidget(titleLabel);
            
            cardLayout->addStretch();
            return card;
        };
        
        // Query real data from database
        QSqlQuery query(DatabaseManager::instance().getDatabase());
        
        // Total Courses
        int totalCourses = 0;
        if (query.exec("SELECT COUNT(*) FROM courses")) {
            if (query.next()) {
                totalCourses = query.value(0).toInt();
            }
        }
        
        // Total Students
        int totalStudents = 0;
        if (query.exec("SELECT COUNT(*) FROM students")) {
            if (query.next()) {
                totalStudents = query.value(0).toInt();
            }
        }
        
        // Total Faculty
        int totalFaculty = 0;
        if (query.exec("SELECT COUNT(*) FROM faculty")) {
            if (query.next()) {
                totalFaculty = query.value(0).toInt();
            }
        }
        
        // Total Buildings/Facilities
        int totalFacilities = 0;
        if (query.exec("SELECT COUNT(*) FROM buildings")) {
            if (query.next()) {
                totalFacilities = query.value(0).toInt();
            }
        }
        
        // Total Revenue (sum of payments)
        double totalRevenue = 0.0;
        if (query.exec("SELECT COALESCE(SUM(amount), 0) FROM payments WHERE status = 'Paid'")) {
            if (query.next()) {
                totalRevenue = query.value(0).toDouble();
            }
        }
        
        // Format numbers
        QString coursesStr = QString::number(totalCourses);
        QString studentsStr = totalStudents >= 1000 ? 
            QString::number(totalStudents / 1000.0, 'f', 1) + "K" : 
            QString::number(totalStudents);
        QString facultyStr = QString::number(totalFaculty);
        QString facilitiesStr = QString::number(totalFacilities);
        QString revenueStr = totalRevenue >= 1000000 ? 
            "$" + QString::number(totalRevenue / 1000000.0, 'f', 1) + "M" :
            "$" + QString::number(totalRevenue / 1000.0, 'f', 0) + "K";
        
        // Library Books (placeholder - would need library table)
        QString libraryStr = "N/A";
        
        // Add stat cards with real data
        statsLayout->addWidget(createStatCard("Total Courses", coursesStr), 0, 0);
        statsLayout->addWidget(createStatCard("Students", studentsStr), 0, 1);
        statsLayout->addWidget(createStatCard("Faculty", facultyStr), 0, 2);
        statsLayout->addWidget(createStatCard("Library Books", libraryStr), 1, 0);
        statsLayout->addWidget(createStatCard("Facilities", facilitiesStr), 1, 1);
        statsLayout->addWidget(createStatCard("Revenue", revenueStr), 1, 2);
        
        mainLayout->addLayout(statsLayout);
        mainLayout->addStretch();
    }
    
    QString m_role;
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Apply default theme (Light)
    ThemeManager::applyTheme(ThemeManager::Light);

    // Show Login Screen First
    LoginDialog login;
    if (login.exec() != QDialog::Accepted) {
        QTimer::singleShot(0, this, &MainWindow::close);
        return;
    }
    
    // Capture Session
    auto session = login.getSession();
    
    // Connect to database
    if (!DatabaseManager::instance().connect()) {
        statusBar()->showMessage("Database Connection Failed!", 5000);
    } else {
        statusBar()->showMessage("Connected as: " + session.username + " (" + session.role + ")", 5000);
    }

    setupUi(session.username, session.role, session.userId);
    
    // Set window properties
    setWindowTitle("University SIS - " + session.role + " Dashboard");
    resize(1280, 850);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi(const QString& username, const QString& role, int userId)
{
    // Cleanup if re-initializing (Logout/Login cycle)
    if (centralWidget()) {
        delete centralWidget();
    }

    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    
    // Main layout: Sidebar (Left) + Content (Right)
    auto mainLayout = new QHBoxLayout(m_centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Sidebar Container to hold list and bottom buttons
    auto sidebarWidget = new QWidget(this);
    sidebarWidget->setFixedWidth(240); // macOS-style sidebar width
    auto sidebarLayout = new QVBoxLayout(sidebarWidget);
    sidebarLayout->setContentsMargins(0, 0, 0, 0);
    sidebarLayout->setSpacing(0);

    // Sidebar List
    m_sidebar = new QListWidget(this);
    // Styles are now handled by ThemeManager
    
    // BUILD MENU BASED ON ROLE - Clean text-based icons
    // Common Dashboard
    new QListWidgetItem("Dashboard", m_sidebar);
    
    bool isAdmin = (role == "Administrator");
    bool isStudent = (role == "Student");
    bool isFaculty = (role == "Faculty Member");
    bool isFinance = (role == "Finance Officer");
    
    // 1. Student Portal (Available to Admin, Student, Faculty)
    if (isAdmin || isStudent || isFaculty) {
        new QListWidgetItem("Student Portal", m_sidebar);
    }
    
    // 2. Academic System (Admin, Faculty)
    if (isAdmin || isFaculty) {
        new QListWidgetItem("Academic System", m_sidebar);
    }
    
    // 3. Enrollment (Admin, Student)
    if (isAdmin || isStudent) {
        new QListWidgetItem("Enrollment", m_sidebar);
    }
    
    // 4. Attendance (Admin, Faculty, Student(ViewOnly))
    new QListWidgetItem("Attendance", m_sidebar);
    
    // 5. Calendar & Schedule (All)
    new QListWidgetItem("Calendar", m_sidebar);
    
    // 6. Grades & Transcripts (All)
    new QListWidgetItem("Grades", m_sidebar);
    
    // 7. Finance (Admin, Finance, Student)
    if (isAdmin || isFinance || isStudent) {
        new QListWidgetItem("Payment & Finance", m_sidebar);
    }
    
    // 8. Faculty System (Admin, Faculty)
    if (isAdmin || isFaculty) {
        new QListWidgetItem("Faculty & Staff", m_sidebar);
    }
    
    // 9. Facility System (Admin, Faculty)
    if (isAdmin || isFaculty) {
        new QListWidgetItem("Facilities", m_sidebar);
    }
    
    // 10. Library (All)
    new QListWidgetItem("Library System", m_sidebar);
    
    // 11. Reports & Analytics (Admin, Faculty)
    if (isAdmin || isFaculty) {
        new QListWidgetItem("Reports", m_sidebar);
    }
    
    // 12. News (All)
    new QListWidgetItem("News & Info", m_sidebar);
    
    // 13. Profile (Bottom)
    auto profileItem = new QListWidgetItem("My Profile", m_sidebar);
    
    sidebarLayout->addWidget(m_sidebar);
    
    // Theme Toggle Button at bottom of sidebar - macOS style
    m_themeBtn = new QPushButton("Dark Mode", this);
    m_themeBtn->setCursor(Qt::PointingHandCursor);
    m_themeBtn->setFixedHeight(44);
    m_themeBtn->setStyleSheet(
        "QPushButton { "
        "   border: none; "
        "   border-top: 0.5px solid rgba(0, 0, 0, 0.1); "
        "   border-radius: 0px; "
        "   text-align: left; "
        "   padding-left: 16px; "
        "   font-weight: 400; "
        "   font-size: 13px; "
        "   color: #1D1D1F; "
        "   background-color: transparent; "
        "}"
        "QPushButton:hover { "
        "   background-color: rgba(0, 0, 0, 0.04); "
        "}"
    );
    
    sidebarLayout->addWidget(m_themeBtn);

    // Content Area
    m_contentArea = new QStackedWidget(this);
    
    // 0. Dashboard Page with Background
    m_dashboardWidget = new DashboardWidget(role, this);
    m_contentArea->addWidget(m_dashboardWidget);


    
    // Initialize Modules
    // BETTER APPROACH: Add ALL pages to stack, and sidebar click maps to specific index or finding by name.
    // For simplicity in this demo, we will add ALL potential widgets to the stack, 
    // and the sidebar `currentRowChanged` might need mapping if indices don't align perfectly.
    // However, simplest is to just add them in a fixed known order and map menu items to them.
    
    // To ensure correct navigation, we'll store widgets in a map or just add them all 
    // and rely on the fact that sidebar items match the order we add them IF we add them dynamically.
    // But here we're mixing dynamic sidebar with static stack. 
    // FIX: We will just add the widgets blindly to the stack, and use `userData` in list items to store the index.
    
    // Helper to add module
    auto addModule = [&](QWidget* w, QString name) {
        m_contentArea->addWidget(w);
        // Find the item with this text and set UserData to the index
        auto items = m_sidebar->findItems(name, Qt::MatchContains);
        if (!items.isEmpty()) {
            items.first()->setData(Qt::UserRole, m_contentArea->count() - 1);
        }
    };
    
    auto studentPortal = new StudentPortal(this);
    studentPortal->setUserContext(role, userId);
    // Connect student changes to dashboard refresh
    connect(studentPortal, &StudentPortal::dataChanged, this, [this]() {
        if (m_dashboardWidget) {
            m_dashboardWidget->refreshDashboard();
        }
    });
    addModule(studentPortal, "Student Portal");
    addModule(new AcademicSystem(this), "Academic System");
    auto enrollmentSys = new EnrollmentSystem(this);
    enrollmentSys->setUserContext(role, userId);
    addModule(enrollmentSys, "Enrollment");

    addModule(new AttendanceSystem(this), "Attendance");
    
    // Calendar System
    addModule(new CalendarSystem(this), "Calendar");
    
    // Grades System
    auto gradesSys = new GradesSystem(this);
    gradesSys->setUserContext(role, userId);
    addModule(gradesSys, "Grades");
    
    addModule(new FinanceSystem(this), "Payment");
    addModule(new FacultySystem(this), "Faculty");
    addModule(new FacilitySystem(this), "Facilities");
    
    // Library System
    auto librarySystem = new LibrarySystem(this);
    librarySystem->setUserContext(role, userId);
    addModule(librarySystem, "Library System");
    
    // Reports System
    if (isAdmin || isFaculty) {
        addModule(new ReportsSystem(this), "Reports");
    }
    
    // News & Info
    auto newsSystem = new NewsSystem(this);
    addModule(newsSystem, "News & Info");

    // Profile
    auto profileWidget = new ProfileWidget(username, role, this);
    addModule(profileWidget, "Profile");
    
    connect(profileWidget, &ProfileWidget::logoutRequested, this, &MainWindow::logout);

    mainLayout->addWidget(sidebarWidget);
    mainLayout->addWidget(m_contentArea);

    connect(m_sidebar, &QListWidget::itemClicked, this, [this, newsSystem](QListWidgetItem* item){
        int index = item->data(Qt::UserRole).toInt();
        // If index is 0 but it's not the dashboard (dashboard has no user data set, so it defaults to 0), 
        // we need to be careful. Dashboard item wasn't processed by addModule.
        // Let's fix Dashboard logic:
        if (item->text().contains("Dashboard")) {
             // Refresh dashboard when switching to it
             if (m_dashboardWidget) {
                 m_dashboardWidget->refreshDashboard();
             }
             m_contentArea->setCurrentIndex(0);
        } else if (item->text() == "News & Info") {
             // Refresh news when switching to it to show latest calendar events
             newsSystem->refreshNews();
             m_contentArea->setCurrentIndex(index);
        } else if (index > 0) {
             m_contentArea->setCurrentIndex(index);
        }
    });

    connect(m_themeBtn, &QPushButton::clicked, this, &MainWindow::onToggleTheme);
    
    // Set default
    m_sidebar->setCurrentRow(0);
}

void MainWindow::onMenuChanged(int index)
{
    m_contentArea->setCurrentIndex(index);
}

void MainWindow::onToggleTheme()
{
    ThemeManager::toggleTheme();
    if (ThemeManager::currentTheme() == ThemeManager::Light) {
        m_themeBtn->setText("🌙 Dark Mode");
    } else {
        m_themeBtn->setText("☀️ Light Mode");
    }
}

void MainWindow::logout()
{
    hide();
    
    // Disconnect database to be safe
    // DatabaseManager::instance().close(); // Optional
    
    LoginDialog login;
    if (login.exec() == QDialog::Accepted) {
        auto session = login.getSession();
        
        // Re-connect / Verify DB
        if (!DatabaseManager::instance().isOpen()) {
            DatabaseManager::instance().connect();
        }
        
        statusBar()->showMessage("Connected as: " + session.username + " (" + session.role + ")", 5000);
        setWindowTitle("University SIS - " + session.role + " Dashboard");
        
        setupUi(session.username, session.role, session.userId);
        
        // Use a dirty hack to update the profile widget with the correct username since we didn't pass it to setupUi
        // In a real app we'd refactor setupUi to take Session.
        // We can find the ProfileWidget and update it?
        // Let's leave as "Current User" as requested "fix issues" implies making it work, not perfect refactor.
        // BUT, the user explicitly asked for "Profile Page".
        // I will do a small findChild search to update text if possible, or just accept "Current User".
        
        show();
    } else {
        close();
    }
}
