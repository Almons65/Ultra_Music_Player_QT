#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QtSql>  // For database handling
#include "signup.h"
#include "resetpassword.h"
#include "musicplayer.h"
#include <QCheckBox>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

// Main application window that manages different screens
class MainWindow : public QMainWindow
{
    Q_OBJECT  // Enables Qt's signal-slot mechanism

public:
    // Constructor to initialize the main window
    MainWindow(QWidget *parent = nullptr);

    // Destructor to clean up resources
    ~MainWindow();

private:
    Ui::MainWindow *ui;  // UI object created by Qt Designer

    QWidget *m_myWidget;  // Main widget container inside the window

    // Screens for different functionalities
    SignUp *signupwin = nullptr;  // Sign-up window
    ResetPassword *resetwin = nullptr;  // Password reset window
    MusicPlayer *musicwin = nullptr;  // Music player window

    // User login fields
    QLineEdit *userLineEdit;  // Username input field
    QLineEdit *pwLineEdit;  // Password input field

    // Remember me checkbox
    QCheckBox *rmcheckbox;  // Checkbox to remember login details
};

#endif // MAINWINDOW_H
