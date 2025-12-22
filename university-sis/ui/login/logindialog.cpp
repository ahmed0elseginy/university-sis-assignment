#include "logindialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QScreen>
#include <QApplication>
#include <QPainter>
#include <QPainterPath>
#include "../../database/databasemanager.h"
#include "../../modules/student/studentrepository.h"
#include "../../modules/faculty/facultyrepository.h"

// Custom Widget for Background with Image
class BackgroundWidget : public QWidget {
public:
    explicit BackgroundWidget(QWidget* parent = nullptr) : QWidget(parent) {}
protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        QPixmap bg(":/resources/images/login_bg.jpg");
        if (!bg.isNull()) {
            p.drawPixmap(rect(), bg.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
        } else {
             // Fallback gradient if image missing
            QLinearGradient gradient(rect().topLeft(), rect().bottomRight());
            gradient.setColorAt(0, QColor("#e0c3fc"));
            gradient.setColorAt(1, QColor("#8ec5fc"));
            p.fillRect(rect(), gradient);
        }
        
        // Add a dark overlay for better text contrast if needed (optional)
        p.fillRect(rect(), QColor(0, 0, 0, 50));
    }
};

LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("University SIS - Login");
    
    // Set size to be large or fullscreen-ish
    // QScreen *screen = QGuiApplication::primaryScreen();
    // QRect screenGeometry = screen->geometry();
    // resize(screenGeometry.width() * 0.8, screenGeometry.height() * 0.8);
    resize(1200, 800);
    
    // Remove frame for modern look? No, standard window is safer for resizing/moving.
    // But we want the background image to cover everything.
    
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // Background
    auto bgWidget = new BackgroundWidget(this);
    mainLayout->addWidget(bgWidget);
    
    // Container for Glass Card
    auto containerLayout = new QVBoxLayout(bgWidget);
    containerLayout->setAlignment(Qt::AlignCenter);

    // Liquid Glass Card
    QFrame* card = new QFrame();
    card->setFixedWidth(450);
    card->setStyleSheet(
        "QFrame {"
        "   background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, "
        "       stop:0 rgba(255, 255, 255, 210), stop:1 rgba(255, 255, 255, 120));"
        "   border: 1px solid rgba(255, 255, 255, 200);"
        "   border-radius: 20px;"
        "   border-bottom: 2px solid rgba(255, 255, 255, 100);"
        "   border-right: 2px solid rgba(255, 255, 255, 100);"
        "}"
    );

    // Deep Shadow for depth
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(50);
    shadow->setColor(QColor(0, 0, 0, 80));
    shadow->setOffset(0, 15);
    card->setGraphicsEffect(shadow);
    
    // Card Layout
    auto cardLayout = new QVBoxLayout(card);
    cardLayout->setSpacing(25);
    cardLayout->setContentsMargins(50, 60, 50, 60);

    // --- Header Section ---
    auto title = new QLabel("University Portal");
    title->setAlignment(Qt::AlignCenter);
    // Modern Gradient Text effect is hard in Widgets, sticking to standard robust color
    title->setStyleSheet("font-size: 36px; font-weight: 900; color: #2D3436; font-family: 'Segoe UI', sans-serif; background: transparent; border: none;");
    cardLayout->addWidget(title);
    
    auto subTitle = new QLabel("Sign in to your dashboard");
    subTitle->setAlignment(Qt::AlignCenter);
    subTitle->setStyleSheet("font-size: 16px; color: #636e72; margin-bottom: 10px; background: transparent; border: none; font-weight: 500;");
    cardLayout->addWidget(subTitle);

    // --- styled inputs ---
    auto createInput = [](const QString& ph, bool isPass) -> QLineEdit* {
        QLineEdit* edit = new QLineEdit();
        edit->setPlaceholderText(ph);
        if (isPass) edit->setEchoMode(QLineEdit::Password);
        edit->setStyleSheet(
            "QLineEdit {"
            "   padding: 14px 18px;"
            "   background-color: rgba(255, 255, 255, 180);"
            "   border: 1px solid rgba(255, 255, 255, 100);"
            "   border-radius: 12px;"
            "   font-size: 15px;"
            "   color: #2d3436;"
            "   selection-background-color: #0984e3;"
            "}"
            "QLineEdit:focus {"
            "   background-color: #ffffff;"
            "   border: 2px solid #74b9ff;"
            "}"
            "QLineEdit:hover {"
            "   background-color: rgba(255, 255, 255, 230);"
            "}"
        );
        return edit;
    };

    m_userEdit = createInput("Username", false);
    cardLayout->addWidget(m_userEdit);
    
    m_passEdit = createInput("Password", true);
    cardLayout->addWidget(m_passEdit);

    // Role Selection
    m_roleCombo = new QComboBox();
    m_roleCombo->addItems({"Administrator", "Student", "Faculty Member", "Finance Officer", "Registrar Officer"});
    m_roleCombo->setStyleSheet(
        "QComboBox {"
        "   padding: 12px;"
        "   background-color: rgba(255, 255, 255, 180);"
        "   border: 1px solid rgba(255, 255, 255, 100);"
        "   border-radius: 12px;"
        "   font-size: 15px;"
        "   color: #2d3436;"
        "}"
        "QComboBox::drop-down { border: none; width: 30px; }"
        "QComboBox::down-arrow { image: none; border-left: 2px solid #dfe6e9; width: 0; height: 0; border-top: 5px solid #636e72; border-right: 5px solid transparent; border-left: 5px solid transparent; margin-right: 10px; }" 
        // Note: Custom Arrow styling in Qt StyleSheets is tricky without images, keeping simple or default
    );
    cardLayout->addWidget(m_roleCombo);

    cardLayout->addSpacing(10);

    // Action Button
    auto btn = new QPushButton("Login");
    btn->setCursor(Qt::PointingHandCursor);
    btn->setFixedHeight(55);
    // Gradient Button
    btn->setStyleSheet(
        "QPushButton { "
        "   background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 #0984e3, stop:1 #74b9ff);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 12px;"
        "   font-size: 18px;"
        "   font-weight: 800;"
        "   letter-spacing: 1px;"
        "}"
        "QPushButton:hover {"
        "   background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 #00cec9, stop:1 #81ecec);"
        "}"
        "QPushButton:pressed {"
        "   background-color: #0984e3;"
        "}"
    );

    cardLayout->addWidget(btn);
    
    containerLayout->addWidget(card);

    connect(btn, &QPushButton::clicked, this, &LoginDialog::onLogin);
    
    // Defaults for demo
    m_userEdit->setText("admin");
    m_passEdit->setText("1234");
    
    m_userEdit->setFocus();
}

void LoginDialog::onLogin()
{
    QString user = m_userEdit->text().trimmed();
    QString pass = m_passEdit->text().trimmed();
    QString role = m_roleCombo->currentText();

    if (user.isEmpty() || pass.isEmpty()) {
        QMessageBox::warning(this, "Login Failed", "Please enter both username and password.");
        return;
    }

    // 1. Authenticate Student
    if (role == "Student") {
        StudentRepository repo;
        auto student = repo.authenticate(user, pass);
        if (student) {
            m_session.username = student->username.isEmpty() ? student->name : student->username;
            m_session.role = "Student";
            m_session.userId = student->id;
            accept();
        } else {
             QMessageBox::critical(this, "Login Failed", "Invalid username or password for Student.");
        }
        return; // Don't proceed to admin/default check
    }

    // 2. Default/Simulation for other roles (Admin, Faculty, etc.)
    // In a real app, we would authenticate them against 'users' table or 'faculty' table.
    // For now, we simulate success for Admin/Faculty as per previous setup, 
    // unless you want to lock it down. Let's keep existing simulation for non-students for simplicity 
    // BUT verify against admin hardcoded creds if role is Admin to be slightly safer.
    
    // 3. Authenticate Faculty
    if (role == "Faculty Member") {
        FacultyRepository repo;
        auto faculty = repo.authenticate(user, pass);
        if (faculty) {
            m_session.username = faculty->username.isEmpty() ? faculty->name : faculty->username;
            m_session.role = "Faculty Member";
            m_session.userId = faculty->id;
            accept();
            return;
        } else {
            // Check fallback demo credentials
            if (user == "faculty" && pass == "faculty123") {
                m_session.username = "Dr. Simulation";
                m_session.role = "Faculty Member";
                m_session.userId = 999;
                accept();
                return;
            }
             QMessageBox::critical(this, "Login Failed", "Invalid credentials for Faculty.");
             return;
        }
    }

    if (role == "Administrator") {
        if (user == "admin" && pass == "1234") {
            m_session.username = "Administrator";
            m_session.role = "Administrator";
            m_session.userId = 1;
            accept();
        } else {
             QMessageBox::critical(this, "Login Failed", "Invalid Admin credentials (try admin/1234).");
        }
    } else {
        // Fallback for Finance/Others simulation
        m_session.username = user;
        m_session.role = role;
        m_session.userId = 999; 
        accept();
    }
}

LoginDialog::UserSession LoginDialog::getSession() const {
    return m_session;
}
