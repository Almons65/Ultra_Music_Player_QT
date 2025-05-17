#ifndef RESETPASSWORD_H
#define RESETPASSWORD_H

#include <QDialog>
#include <QFrame>
#include <QtSql>  // For database operations
#include <QCryptographicHash>  // For encrypting passwords
#include <QLineEdit>

class MainWindow;  // Forward declaration to avoid circular dependency

namespace Ui {
class ResetPassword;
}

// ResetPassword dialog allows users to reset their account password
class ResetPassword : public QDialog
{
    Q_OBJECT  // Enables Qt's signal-slot system

public:
    // Constructor: Initializes the ResetPassword dialog
    explicit ResetPassword(QWidget *parent = nullptr);

    // Destructor: Cleans up resources
    ~ResetPassword();

private:
    Ui::ResetPassword *ui;  // UI object created by Qt Designer

    MainWindow *mwin = nullptr;  // Pointer to the main window

    // Input fields for user details
    QLineEdit *userLineEdit;  // Username input field
    QLineEdit *pwLineEdit;  // New password input field
    QLineEdit *cfpwLineEdit;  // Confirm new password input field

    // Frames to visually group password fields
    QFrame *pwFrame;
    QFrame *cfpwFrame;

    // Creates a password field with a frame
    QFrame* createPasswordField(QWidget *parent, QLineEdit* &pwLineEdit);

    // Validates if the password meets security requirements
    bool validatePassword(const QString &password);
};

#endif // RESETPASSWORD_H
