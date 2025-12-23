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

    // macOS Style Card
    QFrame* card = new QFrame();
    card->setFixedWidth(420);
    card->setStyleSheet(
        "QFrame {"
        "   background-color: rgba(255, 255, 255, 250);"
        "   border: 0.5px solid rgba(209, 209, 214, 0.6);"
        "   border-radius: 12px;"
        "}"
    );

    // Subtle Shadow - macOS style
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 25));
    shadow->setOffset(0, 4);
    card->setGraphicsEffect(shadow);
    
    // Card Layout
    auto cardLayout = new QVBoxLayout(card);
    cardLayout->setSpacing(16);
    cardLayout->setContentsMargins(40, 40, 40, 40);

    // --- Header Section - macOS Style ---
    auto title = new QLabel("University Portal");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 28px; font-weight: 600; color: #1D1D1F; font-family: -apple-system, 'SF Pro Display', sans-serif; background: transparent; border: none;");
    cardLayout->addWidget(title);
    
    auto subTitle = new QLabel("Sign in to continue");
    subTitle->setAlignment(Qt::AlignCenter);
    subTitle->setStyleSheet("font-size: 13px; color: #8E8E93; margin-bottom: 8px; margin-top: 4px; background: transparent; border: none; font-weight: 400;");
    cardLayout->addWidget(subTitle);

    // --- macOS styled inputs ---
    auto createInput = [](const QString& ph, bool isPass) -> QLineEdit* {
        QLineEdit* edit = new QLineEdit();
        edit->setPlaceholderText(ph);
        if (isPass) edit->setEchoMode(QLineEdit::Password);
        edit->setStyleSheet(
            "QLineEdit {"
            "   padding: 8px 12px;"
            "   background-color: rgba(255, 255, 255, 240);"
            "   border: 0.5px solid rgba(209, 209, 214, 0.8);"
            "   border-radius: 6px;"
            "   font-size: 13px;"
            "   color: #1D1D1F;"
            "   font-weight: 400;"
            "   selection-background-color: #007AFF;"
            "   selection-color: white;"
            "}"
            "QLineEdit:focus {"
            "   background-color: #ffffff;"
            "   border: 0.5px solid #007AFF;"
            "}"
            "QLineEdit:hover {"
            "   background-color: rgba(255, 255, 255, 250);"
            "   border-color: rgba(199, 199, 204, 0.8);"
            "}"
        );
        return edit;
    };

    m_userEdit = createInput("Username", false);
    cardLayout->addWidget(m_userEdit);
    
    m_passEdit = createInput("Password", true);
    cardLayout->addWidget(m_passEdit);

    // Role Selection - macOS Style
    m_roleCombo = new QComboBox();
    m_roleCombo->addItems({"Administrator", "Student", "Faculty Member", "Finance Officer", "Registrar Officer"});
    m_roleCombo->setStyleSheet(
        "QComboBox {"
        "   padding: 8px 12px;"
        "   background-color: rgba(255, 255, 255, 240);"
        "   border: 0.5px solid rgba(209, 209, 214, 0.8);"
        "   border-radius: 6px;"
        "   font-size: 13px;"
        "   color: #1D1D1F;"
        "   font-weight: 400;"
        "}"
        "QComboBox:hover {"
        "   background-color: rgba(255, 255, 255, 250);"
        "   border-color: rgba(199, 199, 204, 0.8);"
        "}"
        "QComboBox:focus {"
        "   background-color: #ffffff;"
        "   border: 0.5px solid #007AFF;"
        "}"
        "QComboBox::drop-down { border: none; width: 24px; }"
        "QComboBox::down-arrow { width: 0; height: 0; border-left: 4px solid transparent; border-right: 4px solid transparent; border-top: 5px solid #8E8E93; }"
        "QComboBox QAbstractItemView {"
        "   background-color: #ffffff;"
        "   border: 0.5px solid #D1D1D6;"
        "   border-radius: 8px;"
        "   selection-background-color: #007AFF;"
        "   selection-color: white;"
        "   padding: 4px;"
        "}"
    );
    cardLayout->addWidget(m_roleCombo);

    cardLayout->addSpacing(10);

    // Action Button - macOS Style
    auto btn = new QPushButton("Sign In");
    btn->setCursor(Qt::PointingHandCursor);
    btn->setFixedHeight(32);
    btn->setStyleSheet(
        "QPushButton { "
        "   background-color: #007AFF;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 6px;"
        "   font-size: 13px;"
        "   font-weight: 400;"
        "}"
        "QPushButton:hover {"
        "   background-color: #0051D5;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #0040AA;"
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
