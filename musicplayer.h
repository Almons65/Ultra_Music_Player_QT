#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include <QDialog>
#include <QtSql>  // For database operations
#include <QPixmap>
#include <QEvent>
#include <QSlider>
#include <QLineEdit>
#include <QFocusEvent>
#include <QMediaPlayer>
#include <QMediaMetaData>
#include <QAudioOutput>
#include <QUrl>
#include "clickablelabel.h"  // Custom clickable label for buttons
#include <QStackedWidget>
#include <QDir>
#include <QFileInfoList>
#include <QFileDialog>
#include <QListWidget>
#include <QTimer>

class MainWindow;  // Forward declaration to avoid circular dependency

namespace Ui {
class MusicPlayer;
}

// Custom QLineEdit that switches views when focus is lost
class SearchLineEdit : public QLineEdit {
    Q_OBJECT

public:
    // Constructor: Takes a stacked widget to handle UI transitions
    explicit SearchLineEdit(QStackedWidget *widget, QWidget *parent = nullptr)
        : QLineEdit(parent), searchWidget(widget) {}

protected:
    // When focus is lost, switch to the default search label
    void focusOutEvent(QFocusEvent *event) override {
        searchWidget->setCurrentWidget(searchWidget->widget(0));
        QLineEdit::focusOutEvent(event);
    }

private:
    QStackedWidget *searchWidget;  // Widget stack that manages search UI transitions
};

// MusicPlayer class that handles audio playback and user interactions
class MusicPlayer : public QDialog
{
    Q_OBJECT  // Enables Qt's signal-slot system

public:
    // Constructor: Initializes the music player with an optional username
    explicit MusicPlayer(const QString &username = "", QWidget *parent = nullptr);

    // Destructor: Cleans up resources
    ~MusicPlayer();

protected:
    // Event filter for handling UI interactions
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::MusicPlayer *ui;  // UI object created by Qt Designer

    MainWindow *mwin = nullptr;  // Pointer to the main window
    QLabel *userLabel;  // Displays the username
    QString currentUsername;  // Stores the current user's name

    QListWidget *songListWidget;  // Widget for displaying song list
    QString currentSongPath;  // Tracks the path of the currently playing song

    // Playback state variables
    bool isPlayerPaused = false;
    bool isPlaying = false;
    bool shuffleState = false;
    bool isSongPlaying = false;
    bool isSongPaused = false;
    int currentSongIndex = 0;

    // UI elements
    ClickableLabel *playbtn = nullptr;  // Custom clickable play button
    QPixmap scaledPlayPixmap;  // Scaled pixmap for play button
    QMediaPlayer *player;  // Media player instance
    QAudioOutput *audioOutput;  // Audio output for media player
    QSlider *songSlider;  // Slider to track song progress
    QSlider *volumeSlider;  // Slider to control volume
    ClickableLabel *searchLabel;  // Search button
    QLineEdit *searchEdit;  // Search input field
    QStackedWidget *searchWidget;  // Widget to toggle search UI
    QList<QString> songList;  // List of song file paths

    // Metadata and labels
    QMediaMetaData metaData;
    QLabel *songnameLabel;  // Label to display song name
    QLabel *statusLabel;  // Label to display play/pause status
    QLabel *currenttimeLabel;  // Label to show current playback time
    QLabel *durationtimeLabel;  // Label to show total duration of song

    // Formats time (milliseconds) into a readable format (MM:SS)
    QString formatTime(qint64 milliseconds);

    // Loads songs from a directory
    void loadSongs();

    // Plays a song by index
    void playSongByIndex(int index);

    // Plays a selected song
    void playSong(int index);

    // Finds the file path of a song by name
    QString findSongPath(const QString &songName);

    // Sets the username displayed in the player
    void setUsername(const QString &username);
};

#endif // MUSICPLAYER_H
