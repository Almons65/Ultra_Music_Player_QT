#include "mainwindow.h"
#include <QApplication>
#include "musicplayer.h"
#include <QSqlDatabase>
#include <QSqlQuery>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Initialize SQLite database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("users.db");  // Database file storing user information

    // Check if the database opens successfully
    if (db.open()) {
        QSqlQuery query;

        // Retrieve the username of the user who chose to stay logged in
        query.prepare("SELECT username FROM users WHERE stay_logged_in = 1 LIMIT 1");

        // If a user is found, open the music player directly
        if (query.exec() && query.next()) {
            QString username = query.value(0).toString();
            db.close();  // Close the database connection after retrieving data

            // Launch the Music Player with the logged-in user's name
            MusicPlayer *player = new MusicPlayer(username);
            player->show();
            return a.exec();  // Start event loop
        }
        db.close();  // Close the database if no user is found
    }

    // If no user is logged in, show the login screen (MainWindow)
    MainWindow w;
    w.show();
    return a.exec();  // Start event loop
}
