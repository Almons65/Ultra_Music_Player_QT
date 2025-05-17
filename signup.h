#ifndef SIGNUP_H
#define SIGNUP_H

#include <QDialog>
#include <QFrame>
#include <QtSql>  // For database interactions
#include <QCryptographicHash>  // For password encryption
#include <QLineEdit>

class MainWindow;  // Forward declaration to avoid circular dependency

namespace Ui {
class SignUp;
}

// SignUp dialog for user registration
class SignUp : public QDialog
{
    Q_OBJECT  // Enables Qt's signal-slot system

public:
    // Constructor: Initializes the SignUp dialog
    explicit SignUp(QWidget *parent = nullptr);

    // Destructor: Cleans up resources
    ~SignUp();

private:
    Ui::SignUp *ui;  // UI object created by Qt Designer

    MainWindow *mwin = nullptr;  // Pointer to the main window

    // Password input fields
    QLineEdit *pwLineEdit;  // Password entry field
    QLineEdit *cfpwLineEdit;  // Confirm password entry field

    // Frames to group password fields visually
    QFrame *pwFrame;
    QFrame *cfpwFrame;

    // Creates a password field with a frame
    QFrame* createPasswordField(QWidget *parent, QLineEdit* &pwLineEdit);

    // Saves user credentials to the database
    void saveUserData(const QString &username, const QString &password);
};

#endif // SIGNUP_H
