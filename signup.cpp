#include "signup.h"
#include "ui_signup.h"
#include "mainwindow.h"
#include "clickablelabel.h"
#include <QPushButton>
#include <QColor>
#include <QPixmap>
#include <QFont>
#include <QLineEdit>
#include <QLabel>
#include <QIcon>
#include <QFrame>
#include <QHBoxLayout>
#include <QMessageBox>

SignUp::SignUp(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SignUp)
{
    ui->setupUi(this);
    this->setWindowTitle("Sign Up");
    this->setFixedSize(840, 860);

    QIcon windowIcon(":/imgs/logo.png");  // Make sure to use the correct path
    setWindowIcon(windowIcon);

    this->setStyleSheet("background-color: rgb(255, 255, 224);");


    QLabel *imageLabel = new QLabel(this);
    QPixmap pixmap(":/imgs/logo.png");
    imageLabel->setPixmap(pixmap.scaled(250, 250, Qt::KeepAspectRatio)); // Adjust image size
    imageLabel->move(285, -30); //


    QLabel *userLabel = new QLabel("Username", this);
    QLabel *pwLabel = new QLabel("Password", this);
    QLabel *noacc = new QLabel("Already have an account?", this);
    QLabel *cfpwLabel = new QLabel("Confirm Password", this);

    // Set font size and bold
    QFont font2("Times New Roman", 16, QFont::Bold);
    userLabel->setFont(font2);
    userLabel->setFixedSize(300, 60);
    userLabel->move(246, 320);
    userLabel->setStyleSheet("color: black;");

    QLineEdit *userLineEdit = new QLineEdit(this);
    QFont font3("Times New Roman", 12);
    userLineEdit->setPlaceholderText("Username");
    userLineEdit->setFont(font3);
    userLineEdit->setFixedSize(360, 40);
    userLineEdit->move(246, 370);

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
        "   border: 2px solid gold;"  // Highlight border when focused
        "}"
        );

    pwLabel->setFont(font2);
    pwLabel->setFixedSize(300, 60);
    pwLabel->move(246, 420);
    pwLabel->setStyleSheet("color: black;");


    // Use the class members directly, without redeclaring them
    pwFrame = createPasswordField(this, pwLineEdit);
    pwFrame->move(246, 470);

    cfpwFrame = createPasswordField(this, cfpwLineEdit);
    cfpwFrame->move(246, 570);



    pwLineEdit->setFont(font3);
    pwLineEdit->setPlaceholderText("Password");
    pwLineEdit->setFixedSize(360, 40);
    pwLineEdit->move(246, 470);

    pwLineEdit->setStyleSheet(
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
        "   border: 2px solid gold;"  // Highlight border when focused
        "}"
        );


    cfpwLabel->setFont(font2);
    cfpwLabel->setFixedSize(300, 60);
    cfpwLabel->move(246, 520);
    cfpwLabel->setStyleSheet("color: black;");

    cfpwLineEdit->setFont(font3);
    cfpwLineEdit->setPlaceholderText("Confirm password");
    cfpwLineEdit->setFixedSize(360, 40);
    cfpwLineEdit->move(246, 570);

    cfpwLineEdit->setStyleSheet(
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
        "   border: 2px solid gold;"  // Highlight border when focused
        "}"
        );



    QPushButton *loginpb = new QPushButton("Sign Up", this);  // Uppercase for Spotify style
    loginpb->setFixedSize(360, 50);
    loginpb->move(246, 675);

    loginpb->setStyleSheet(
        "QPushButton {"
        "   font-size: 20px;"
        "   font-weight: bold;"         // Bold text
        "   font-family: 'Times New Roman';" // Spotify uses a sans-serif font // Make text uppercase like Spotify
        "   color: black;"              // White text
        "   background-color: gold;" // Spotify Green
        "   border: none;"              // No border
        "   border-radius: 25px;"       // Fully rounded edges
        "   padding: 10px 20px;"        // Better padding
        "}"
        "QPushButton:hover {"
        "   background-color: orange;" // Slightly darker green on hover
        "}"
        "QPushButton:pressed {"
        "   background-color: yellow;" // Even darker green when pressed
        "}"
        );


    connect(loginpb, &QPushButton::clicked, this, [this, userLineEdit]() {
        QString username = userLineEdit->text().trimmed();
        QString password = this->pwLineEdit->text();
        QString confirmPassword = this->cfpwLineEdit->text();


        if (username.isEmpty() || password.isEmpty() || confirmPassword.isEmpty()) {
            QMessageBox::warning(this, "Input Error", "All fields are required.");
            return;
        }


        if (password != confirmPassword) {
            QMessageBox::warning(this, "Password Error", "Passwords do not match.");
            return;
        }


        // Regular expression for password validation
        QRegularExpression passwordRegex("^(?=.*[A-Z])(?=.*\\d)(?=.*[@$!%*?&])[A-Za-z\\d@$!%*?&]{8,}$");

        if (!passwordRegex.match(password).hasMatch()) {
            QMessageBox::warning(this, "Weak Password",
                                 "Password must be at least 8 characters long, contain at least one uppercase letter, one number, and one special character.");
            return;
        }


        saveUserData(username, password);
    });


    QFont font4("Times New Roman", 14, QFont::Bold);
    noacc->setFont(font4);
    noacc->setFixedSize(300, 60);
    noacc->move(290, 770);
    noacc->setStyleSheet("color: black;");



    ClickableLabel *backlog = new ClickableLabel(this);
    backlog->setText("Log In");
    backlog->setStyleSheet(
        "QLabel {"
        "   color: blue;"
        "   text-decoration: underline;"
        "   font-size: 18px;"  // Set font size
        "   font-family: 'Times New Roman';"  // Set font family
        "}"
        "QLabel:hover { color: darkblue; }"
        );
    backlog->setCursor(Qt::PointingHandCursor);
    backlog->move(510, 780);
    backlog->setFixedSize(200, 40);

    connect(backlog, &ClickableLabel::clicked, this, [this]() {
        if (!mwin) {
            mwin = new MainWindow();
            mwin->setAttribute(Qt::WA_DeleteOnClose);

            connect(mwin, &QDialog::destroyed, this, [this]() {
                mwin = nullptr;
            });
        }

        this->close(); // Close the current window
        mwin->show();
        mwin->raise();
        mwin->activateWindow();
    });




    ClickableLabel *exit = new ClickableLabel(this);
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




    QLabel *titleLabel = new QLabel("Sign up for Ultra", this);

    // Set font size and bold
    QFont font("Times New Roman", 36, QFont::Bold);
    titleLabel->setFont(font);
    titleLabel->setFixedSize(500, 60);
    titleLabel->move(173, 210);
    titleLabel->setStyleSheet("color: black;");

    // Center align text
    titleLabel->setAlignment(Qt::AlignCenter);


    // Scale the image to fit the fixed siz



}


QFrame* SignUp::createPasswordField(QWidget *parent, QLineEdit* &pwLineEdit) {
    QFrame *frame = new QFrame(parent);
    frame->setFixedSize(370, 40);

    // Create password input field
    pwLineEdit = new QLineEdit(frame);
    pwLineEdit->setEchoMode(QLineEdit::Password);
    pwLineEdit->setFixedSize(360, 40);
    pwLineEdit->setStyleSheet(
        "QLineEdit {"
        "   font-size: 16px;"
        "   border: 2px solid black;"
        "   border-radius: 12px;"
        "   background: white;"
        "   color: black;"
        "   padding-right: 40px;"  // Reserve space for eye button
        "}"
        "QLineEdit:focus {"
        "   border: 2px solid royalblue;"
        "}"
        );

    // Create eye button
    ClickableLabel *eyeBtn = new ClickableLabel(frame);
    QPixmap coverImage(":/imgs/eye.png");
    QPixmap uncoverImage(":/imgs/eye_closed.png");

    if (coverImage.isNull() || uncoverImage.isNull()) {
        qDebug() << "Error: Failed to load eye icons";
        return frame;
    }

    eyeBtn->setPixmap(coverImage.scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    eyeBtn->setFixedSize(30, 30);
    eyeBtn->setCursor(Qt::PointingHandCursor);
    eyeBtn->setStyleSheet("background-color: white");

    QHBoxLayout *layout = new QHBoxLayout(frame);
    layout->setContentsMargins(0, 0, 22, 0);
    layout->setSpacing(0);
    layout->addWidget(pwLineEdit);
    layout->addWidget(eyeBtn);

    frame->setLayout(layout);

    // Toggle password visibility
    bool isCovering = true;
    QObject::connect(eyeBtn, &ClickableLabel::clicked, [=]() mutable {
        isCovering = !isCovering;
        pwLineEdit->setEchoMode(isCovering ? QLineEdit::Password : QLineEdit::Normal);
        eyeBtn->setPixmap((isCovering ? coverImage : uncoverImage).scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    });

    return frame;
}






SignUp::~SignUp()
{
    delete ui;
}



void SignUp::saveUserData(const QString &username, const QString &password) {
    // Open the database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("users.db"); // Store locally in a SQLite database

    if (!db.open()) {
        QMessageBox::critical(this, "Database Error", "Failed to open the database.");
        return;
    }

    // Create users table if it doesn't exist
    QSqlQuery query;
    if (!query.exec("CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT UNIQUE, password TEXT, stay_logged_in INTEGER DEFAULT 0)")) {
        qDebug() << "Table creation error: " << query.lastError().text();
        QMessageBox::critical(this, "Error", "Failed to create users table.");
        db.close();
        return;
    }

    // Prepare the INSERT statement
    query.prepare("INSERT INTO users (username, password) VALUES (:username, :password)");
    query.bindValue(":username", username);
    query.bindValue(":password", password);  // Ideally, hash password before storing

    // Start a transaction for safety
    db.transaction();

    if (!query.exec()) {
        qDebug() << "Insert error: " << query.lastError().text();
        QMessageBox::critical(this, "Error", "Username already exists or database error.");
        db.rollback();  // Rollback in case of failure
    } else {
        QMessageBox::information(this, "Success", "Account created successfully!");
        this->close();

        // Open the login window (MainWindow)
        if (!mwin) {
            mwin = new MainWindow();
            mwin->setAttribute(Qt::WA_DeleteOnClose);

            connect(mwin, &QDialog::destroyed, this, [this]() {
                mwin = nullptr;
            });
        }

        mwin->show();
        mwin->raise();
        mwin->activateWindow();

        // Commit transaction if everything goes fine
        db.commit();
    }

    db.close();
}

