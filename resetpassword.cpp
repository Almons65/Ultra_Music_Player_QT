#include "resetpassword.h"
#include "ui_resetpassword.h"
#include "mainwindow.h"
#include "clickablelabel.h"
#include <QPalette>
#include <QVBoxLayout>
#include <QPushButton>
#include <QColor>
#include <QPixmap>
#include <QFont>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include <QIcon>
#include <QFrame>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

ResetPassword::ResetPassword(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ResetPassword)
{
    ui->setupUi(this);
    this->setWindowTitle("Reset Password");
    this->setFixedSize(840, 780);

    QIcon windowIcon(":/imgs/logo.png");  // Make sure to use the correct path
    setWindowIcon(windowIcon);

    this->setStyleSheet("background-color: rgb(144, 224, 239);");


    QLabel *imageLabel = new QLabel(this);
    QPixmap pixmap(":/imgs/logo.png");
    imageLabel->setPixmap(pixmap.scaled(250, 250, Qt::KeepAspectRatio)); // Adjust image size
    imageLabel->move(285, -30); //


    QLabel *userLabel = new QLabel("Username", this);
    QLabel *pwLabel = new QLabel("New Password", this);
    QLabel *cfpwLabel = new QLabel("Confirm Password", this);

    // Set font size and bold
    QFont font2("Times New Roman", 16, QFont::Bold);
    userLabel->setFont(font2);
    userLabel->setFixedSize(300, 60);
    userLabel->move(238, 320);
    userLabel->setStyleSheet("color: black;");

    QLineEdit *userLineEdit = new QLineEdit(this);
    QFont font3("Times New Roman", 12);
    userLineEdit->setPlaceholderText("Username");
    userLineEdit->setFont(font3);
    userLineEdit->setFixedSize(360, 40);
    userLineEdit->move(238, 370);

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
        "   border: 2px solid aqua;"  // Highlight border when focused
        "}"
        );

    pwLabel->setFont(font2);
    pwLabel->setFixedSize(300, 60);
    pwLabel->move(238, 420);
    pwLabel->setStyleSheet("color: black;");

    pwFrame = createPasswordField(this, pwLineEdit);
    pwFrame->move(238, 470);

    cfpwFrame = createPasswordField(this, cfpwLineEdit);
    cfpwFrame->move(238, 570);


    pwLineEdit->setFont(font3);
    pwLineEdit->setPlaceholderText("Password");
    pwLineEdit->setFixedSize(360, 40);
    pwLineEdit->move(238, 470);

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
        "   border: 2px solid aqua;"  // Highlight border when focused
        "}"
        );


    cfpwLabel->setFont(font2);
    cfpwLineEdit->setPlaceholderText("Confirm password");
    cfpwLabel->setFixedSize(300, 60);
    cfpwLabel->move(238, 520);
    cfpwLabel->setStyleSheet("color: black;");



    cfpwLineEdit->setFont(font3);
    cfpwLineEdit->setFixedSize(360, 40);
    cfpwLineEdit->move(238, 570);

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
        "   border: 2px solid aqua;"  // Highlight border when focused
        "}"
        );



    QPushButton *loginpb = new QPushButton("Set Password", this);  // Uppercase for Spotify style
    loginpb->setFixedSize(360, 50);
    loginpb->move(238, 675);

    loginpb->setStyleSheet(
        "QPushButton {"
        "   font-size: 20px;"
        "   font-weight: bold;"         // Bold text
        "   font-family: 'Times New Roman';" // Spotify uses a sans-serif font // Make text uppercase like Spotify
        "   color: black;"              // White text
        "   background-color: teal;" // Spotify Green
        "   border: none;"              // No border
        "   border-radius: 25px;"       // Fully rounded edges
        "   padding: 10px 20px;"        // Better padding
        "}"
        "QPushButton:hover {"
        "   background-color: aqua;" // Slightly darker green on hover
        "}"
        "QPushButton:pressed {"
        "   background-color: green;" // Even darker green when pressed
        "}"
        );


    connect(loginpb, &QPushButton::clicked, this, [this, userLineEdit]() {
        // Get the entered username, password, and confirmation password
        QString username = userLineEdit->text().trimmed();  // Trim the username input
        QString newPassword = this->pwLineEdit->text();      // Correct variable name: newPassword
        QString confirmPassword = this->cfpwLineEdit->text();

        // Validate if the passwords match
        if (newPassword != confirmPassword) {
            QMessageBox::warning(this, "Password Mismatch", "The passwords do not match.");
            return;
        }

        // Validate the new password using the regex pattern
        if (!validatePassword(newPassword)) {
            QMessageBox::warning(this, "Invalid Password", "The password does not meet the required criteria.");
            return;
        }

        // Check if the password meets the strong criteria (uppercase, number, special character)
        QRegularExpression passwordRegex("^(?=.*[A-Z])(?=.*\\d)(?=.*[@$!%*?&])[A-Za-z\\d@$!%*?&]{8,}$");

        if (!passwordRegex.match(newPassword).hasMatch()) {
            QMessageBox::warning(this, "Weak Password",
                                 "Password must be at least 8 characters long, contain at least one uppercase letter, one number, and one special character.");
            return;
        }

        // Remove any existing database connection
        if (QSqlDatabase::contains("qt_sql_default_connection")) {
            QSqlDatabase::removeDatabase("qt_sql_default_connection");
        }

        // Open the database connection
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("users.db");

        if (!db.open()) {
            QMessageBox::critical(this, "Database Error", "Failed to open the database.");
            return;
        }

        // Begin the transaction
        if (!db.transaction()) {
            QMessageBox::critical(this, "Transaction Error", "Failed to start the transaction.");
            db.close();
            return;
        }

        // Prepare the query to check if the username exists in the database
        QSqlQuery query;
        query.prepare("SELECT * FROM users WHERE username = ?");
        query.addBindValue(username);

        if (!query.exec()) {
            QMessageBox::critical(this, "Query Error", "Failed to execute the query.");
            db.rollback();  // Rollback if the query failed
            db.close();
            return;
        }

        if (!query.next()) {
            // If the username does not exist
            QMessageBox::warning(this, "Username Not Found", "No user found with this username.");
            db.rollback();  // Rollback if the user was not found
            db.close();
            return;
        }

        // If the username exists, update the password
        query.prepare("UPDATE users SET password = ? WHERE username = ?");
        query.addBindValue(newPassword);  // The new password
        query.addBindValue(username);     // The username

        if (!query.exec()) {
            QMessageBox::critical(this, "Query Error", "Failed to update the password.");
            db.rollback();  // Rollback if the update failed
        } else {
            QMessageBox::information(this, "Success", "Password has been successfully reset.");

            // Commit the transaction if everything is fine
            db.commit();

            this->close();
            MainWindow *login = new MainWindow();
            login->show();
        }

        db.close();
    });



    ClickableLabel *backlog = new ClickableLabel(this);
    backlog->setText("Back to Log In");
    backlog->setStyleSheet(
        "QLabel {"
        "   color: blue;"
        "   font-size: 24px;"
        "   font-weight: bold;"         // Set font size
        "   font-family: 'Times New Roman';"  // Set font family
        "}"
        "QLabel:hover { color: darkblue; }"
        );
    backlog->setCursor(Qt::PointingHandCursor);
    backlog->move(10, 0);
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




    QLabel *titleLabel = new QLabel("Reset Password", this);

    // Set font size and bold
    QFont font("Times New Roman", 36, QFont::Bold);
    titleLabel->setFont(font);
    titleLabel->setFixedSize(500, 60);
    titleLabel->move(167, 210);
    titleLabel->setStyleSheet("color: black;");

    // Center align text
    titleLabel->setAlignment(Qt::AlignCenter);


    // Scale the image to fit the fixed siz

}

QFrame* ResetPassword::createPasswordField(QWidget *parent, QLineEdit* &pwLineEdit) {
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


ResetPassword::~ResetPassword()
{
    delete ui;
}


// resetpassword.cpp

bool ResetPassword::validatePassword(const QString &password) {
    // Minimum 8 characters, at least: 1 uppercase, 1 lowercase, 1 digit, 1 special char
    QRegularExpression regex("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[^a-zA-Z\\d]).{8,}$");
    QRegularExpressionMatch match = regex.match(password);
    return match.hasMatch();
}


