#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "clickablelabel.h"
#include <QPalette>
#include <QVBoxLayout>
#include <QPushButton>
#include <QColor>
#include <QPixmap>
#include <QFont>
#include <QLineEdit>
#include <QCheckBox>
#include <QMessageBox>
#include <QIcon>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Login");
    this->setFixedSize(840, 880);


    QIcon windowIcon(":/imgs/logo.png");
    setWindowIcon(windowIcon);

    m_myWidget = new QWidget(this);
    m_myWidget->setAutoFillBackground(true);

    QPalette pal;
    pal.setColor(QPalette::Window, QColor(173, 216, 230));
    m_myWidget->setPalette(pal);

    // Set as central widget so it displays properly
    setCentralWidget(m_myWidget);

    // Using layout to control widget positioning
    QVBoxLayout *layout = new QVBoxLayout(m_myWidget);

    QLabel *userLabel = new QLabel("Username", m_myWidget);
    QLabel *pwLabel = new QLabel("Password", m_myWidget);
    QLabel *noacc = new QLabel("Don't have an account?", m_myWidget);

    // Set font size and bold
    QFont font2("Times New Roman", 16, QFont::Bold);
    userLabel->setFont(font2);
    userLabel->setFixedSize(300, 60);
    userLabel->move(243, 320);
    userLabel->setStyleSheet("color: black;");

    QLineEdit *userLineEdit = new QLineEdit(m_myWidget);
    QFont font3("Times New Roman", 12);
    userLineEdit->setPlaceholderText("Username");
    userLineEdit->setFont(font3);
    userLineEdit->setFixedSize(360, 40);
    userLineEdit->move(243, 370);

    userLineEdit->setStyleSheet(
        "QLineEdit {"
        "   font-size: 16px;"
        "   font-family: 'Times New Roman';"
        "   color: black;"
        "   background: white;"
        "   border: 2px solid black;"  // Visible border
        "   border-radius: 12px;"       // Rounded corners
        "   padding: 5px;"             // Better spacing inside the field
        "}"
        "QLineEdit:focus {"
        "   border: 2px solid royalblue;"  // Highlight border when focused
        "}"
        );

    pwLabel->setFont(font2);
    pwLabel->setFixedSize(300, 60);
    pwLabel->move(243, 420);
    pwLabel->setStyleSheet("color: black;");

    QLineEdit *pwLineEdit = new QLineEdit(m_myWidget);
    pwLineEdit->setPlaceholderText("Password");
    pwLineEdit->setFont(font3);
    pwLineEdit->setFixedSize(360, 40);
    pwLineEdit->move(243, 470);
    pwLineEdit->setEchoMode(QLineEdit::Password); // Initially hide password

    pwLineEdit->setStyleSheet(
        "QLineEdit {"
        "   font-size: 16px;"
        "   font-family: 'Times New Roman';"
        "   color: black;"
        "   background: white;"
        "   border: 2px solid black;"
        "   border-radius: 12px;"
        "   padding: 5px;"
        "}"
        "QLineEdit:focus {"
        "   border: 2px solid royalblue;"
        "}"
        );

    ClickableLabel *eyebtn = new ClickableLabel(this);
    QPixmap coverPixmap(":/imgs/eye.png");
    QPixmap uncoverPixmap(":/imgs/eye_closed.png");

    if (coverPixmap.isNull() || uncoverPixmap.isNull()) {
        qDebug() << "Error: Failed to load eye icons";
        return;
    }

    eyebtn->setPixmap(coverPixmap.scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    eyebtn->setFixedSize(30, 30);
    eyebtn->setCursor(Qt::PointingHandCursor);
    eyebtn->move(560, 475);

    bool isCovering = true; // Initially masked
    connect(eyebtn, &ClickableLabel::clicked, [=]() mutable {
        isCovering = !isCovering;
        pwLineEdit->setEchoMode(isCovering ? QLineEdit::Password : QLineEdit::Normal);
        eyebtn->setPixmap((isCovering ? coverPixmap : uncoverPixmap).scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    });





    QCheckBox *rmcheckbox = new QCheckBox("Remember Me", m_myWidget);
    rmcheckbox->setFixedSize(180, 50);  // Set size for the whole checkbox widget
    rmcheckbox->move(243, 525);

    // Adjust indicator (checkbox) size
    rmcheckbox->setStyleSheet(
        "QCheckBox {"
        "   spacing: 12px;"  // Space between checkbox and text
        "   font-size: 16px;"
        "   font-weight: bold;"
        "   color: black;"
        "   font-family: 'Times New Roman';"
        "}"
        "QCheckBox::indicator {"
        "   width: 22px;"
        "   height: 20px;"
        "   border-radius: 6px;"
        "   background: white;"
        "   border: 2px solid black;"
        "}"
        "QCheckBox::indicator:hover {"
        "   border-color: lightgreen;"
        "}"
        "QCheckBox::indicator:checked {"
        "   background-color: lightgreen;"
        "   border: 2px solid black;"
        "   image: url(:/imgs/check mark.png);"  // Use an image instead
        "}"
        );

    QPushButton *loginpb = new QPushButton("Log In", m_myWidget);  // Uppercase for Spotify style
    loginpb->setFixedSize(360, 50);
    loginpb->move(243, 610);

    loginpb->setStyleSheet(
        "QPushButton {"
        "   font-size: 20px;"
        "   font-weight: bold;"         // Bold text
        "   font-family: 'Times New Roman';" // Spotify uses a sans-serif font // Make text uppercase like Spotify
        "   color: white;"              // White text
        "   background-color: royalblue;" // Spotify Green
        "   border: none;"              // No border
        "   border-radius: 25px;"       // Fully rounded edges
        "   padding: 10px 20px;"        // Better padding
        "}"
        "QPushButton:hover {"
        "   background-color: darkblue;" // Slightly darker green on hover
        "}"
        "QPushButton:pressed {"
        "   background-color: blue;" // Even darker green when pressed
        "}"
        );

    connect(loginpb, &QPushButton::clicked, this, [=]() {
        if (!userLineEdit || !pwLineEdit) {
            qDebug() << "Error: userLineEdit or pwLineEdit is null!";
            return;
        }

        QString username = userLineEdit->text();
        QString password = pwLineEdit->text();
        bool rememberMe = rmcheckbox->isChecked();

        qDebug() << "Username: " << username;
        qDebug() << "Password: " << password;
        qDebug() << "Remember Me: " << rememberMe;

        if (username.isEmpty() || password.isEmpty()) {
            QMessageBox::warning(nullptr, "Input Error", "Please enter both username and password.");
            return;
        }

        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("users.db");

        if (!db.open()) {
            qDebug() << "Database open error: " << db.lastError().text();
            QMessageBox::critical(nullptr, "Database Error", "Failed to open the database.");
            return;
        }

        QSqlQuery query;
        query.prepare("SELECT password, stay_logged_in FROM users WHERE username = :username");
        query.bindValue(":username", username);

        if (!query.exec()) {
            qDebug() << "Query execution failed: " << query.lastError().text();
            QMessageBox::critical(nullptr, "Error", "Failed to execute query.");
            db.close();
            return;
        }

        if (query.next()) {
            QString storedPassword = query.value(0).toString();
            bool stayLoggedIn = query.value(1).toBool();

            if (storedPassword == password) {
                QMessageBox::information(nullptr, "Login Successful", "Welcome back, " + username + "!");

                QSqlQuery updateQuery;
                updateQuery.prepare("UPDATE users SET stay_logged_in = :stayLoggedIn WHERE username = :username");
                updateQuery.bindValue(":stayLoggedIn", rememberMe ? true : false);
                updateQuery.bindValue(":username", username);
                if (!updateQuery.exec()) {
                    qDebug() << "Update query failed: " << updateQuery.lastError().text();
                }

                db.close();
                if (!musicwin) {
                    musicwin = new MusicPlayer(username);  // Pass the username here
                    musicwin->setAttribute(Qt::WA_DeleteOnClose);

                    connect(musicwin, &QDialog::destroyed, this, [this]() {
                        musicwin = nullptr;
                    });
                }
                this->close();  // Close the current login window (MainWindow)
                musicwin->show();  // Show the music player window
                musicwin->raise();
                musicwin->activateWindow();
            } else {
                QMessageBox::warning(nullptr, "Login Failed", "Incorrect username or password.");
            }
        } else {
            QMessageBox::warning(nullptr, "Login Failed", "Username not found.");
        }

        db.close();
    });


    ClickableLabel *forgetpw = new ClickableLabel(m_myWidget);
    forgetpw->setText("Forgot your password?");
    forgetpw->setStyleSheet(
        "QLabel {"
        "   color: blue;"
        "   text-decoration: underline;"
        "   font-size: 18px;"  // Set font size
        "   font-family: 'Times New Roman';"  // Set font family
        "}"
        "QLabel:hover { color: darkblue; }"
        );
    forgetpw->setCursor(Qt::PointingHandCursor);
    forgetpw->move(340, 695);
    forgetpw->setFixedSize(200, 40);

    // Connect the clicked signal to a slot
    connect(forgetpw, &ClickableLabel::clicked, this, [this]() {
        if (!resetwin) {
            resetwin = new ResetPassword();  // No parent, independent window
            resetwin->setAttribute(Qt::WA_DeleteOnClose);

            connect(resetwin, &QDialog::destroyed, this, [this]() {
                resetwin = nullptr;  // Reset pointer when destroyed
            });
        }

        this->close();  // Close the current window (MainWindow)
        resetwin->show();
        resetwin->raise();
        resetwin->activateWindow();
    });

    QFont font4("Times New Roman", 14, QFont::Bold);
    noacc->setFont(font4);
    noacc->setFixedSize(300, 60);
    noacc->move(262, 770);
    noacc->setStyleSheet("color: black;");



    ClickableLabel *signup = new ClickableLabel(m_myWidget);
    signup->setText("Sign up for Ultra");
    signup->setStyleSheet(
        "QLabel {"
        "   color: blue;"
        "   text-decoration: underline;"
        "   font-size: 18px;"  // Set font size
        "   font-family: 'Times New Roman';"  // Set font family
        "}"
        "QLabel:hover { color: darkblue; }"
        );
    signup->setCursor(Qt::PointingHandCursor);
    signup->move(462, 780);
    signup->setFixedSize(200, 40);

    // Connect the clicked signal to a slot
    connect(signup, &ClickableLabel::clicked, this, [this]() {
        if (!signupwin) {
            signupwin = new SignUp();  // No parent, independent window
            signupwin->setAttribute(Qt::WA_DeleteOnClose);

            connect(signupwin, &QDialog::destroyed, this, [this]() {
                signupwin = nullptr;  // Reset pointer when destroyed
            });
        }

        this->close();  // Close the current window (MainWindow)
        signupwin->show();
        signupwin->raise();
        signupwin->activateWindow();
    });


    ClickableLabel *exit = new ClickableLabel(m_myWidget);
    exit->setText("EXIT");
    exit->setStyleSheet(
        "QLabel {"
        "   color: red;"
        "   font-size: 24px;"
        "   font-weight: bold;"        // Set font size
        "   font-family: 'Times New Roman';"  // Set font family
        "}"
        "QLabel:hover { color: darkred; }"
        );
    exit->setCursor(Qt::PointingHandCursor);
    exit->move(775, 0);
    exit->setFixedSize(200, 40);

    // Connect the clicked signal to a slot
    connect(exit, &ClickableLabel::clicked, this, [this]() {
        this->close();
    });




    QLabel *imageLabel = new QLabel(m_myWidget);
    QPixmap pixmap(":/imgs/logo.png"); // Replace with your actual image path
    imageLabel->setFixedSize(250, 250);
    imageLabel->move(285, -30);

    QLabel *titleLabel = new QLabel("Log in to Ultra", m_myWidget);

    // Set font size and bold
    QFont font("Times New Roman", 36, QFont::Bold);
    titleLabel->setFont(font);
    titleLabel->setFixedSize(300, 60);
    titleLabel->move(270, 210);
    titleLabel->setStyleSheet("color: black;");



    // Center align text
    titleLabel->setAlignment(Qt::AlignCenter);


    // Scale the image to fit the fixed size
    imageLabel->setPixmap(pixmap.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    m_myWidget->setLayout(layout);
}

MainWindow::~MainWindow()
{
    delete ui;
}
