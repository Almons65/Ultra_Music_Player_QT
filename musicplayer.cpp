#include "musicplayer.h"
#include "ui_musicplayer.h"
#include "mainwindow.h"
#include <QPushButton>
#include <QColor>
#include <QPixmap>
#include <QFont>
#include <QLineEdit>
#include <QLabel>
#include <QIcon>
#include <QSlider>
#include <QEvent>
#include <QDebug>
#include <QStackedWidget>
#include <QAction>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>
#include <QDir>
#include <QFileInfoList>
#include <QListWidget>
#include <QFileInfo>
#include <QMessageBox>


MusicPlayer::MusicPlayer(const QString &username, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MusicPlayer)
    , player(new QMediaPlayer(this))
    , audioOutput(new QAudioOutput(this))
    , songListWidget(new QListWidget(this))
    , currentUsername(username)
{
    ui->setupUi(this);
    this->setWindowTitle("Music Player");
    this->setFixedSize(1020, 1000);

    QIcon windowIcon(":/imgs/logo.png");
    setWindowIcon(windowIcon);

    QLabel *userLabel = new QLabel(username, this);
    QFont font1("Times New Roman", 18, QFont::Bold);
    userLabel->move(80, 30);
    userLabel->setFont(font1);

    this->setStyleSheet("background-color: rgb(173, 216, 230);");

    QLabel *iconimageLabel = new QLabel(this);
    QPixmap pixmap(":/imgs/user_icon.png");
    iconimageLabel->setPixmap(pixmap.scaled(60, 60, Qt::KeepAspectRatio));
    iconimageLabel->move(10, 10);

    ClickableLabel *exit = new ClickableLabel(this);
    exit->setText("EXIT");
    exit->setStyleSheet(
        "QLabel { color: red; font-size: 24px; font-weight: bold; font-family: 'Times New Roman'; }"
        "QLabel:hover { color: darkred; }"
        );
    exit->setCursor(Qt::PointingHandCursor);
    exit->move(955, 20);
    exit->setFixedSize(200, 40);

    connect(exit, &ClickableLabel::clicked, this, [this]() {
        QString username = this->currentUsername;        // Check if the database is already open, if not, open it
        QSqlDatabase db = QSqlDatabase::database("qt_sql_default_connection");
        if (!db.isOpen()) {
            db = QSqlDatabase::addDatabase("QSQLITE", "qt_sql_default_connection");
            db.setDatabaseName("users.db");
            if (!db.open()) {
                qDebug() << "Database open error: " << db.lastError().text();
                return;
            }
        }


        QSqlQuery query(db);
        query.prepare("SELECT stay_logged_in FROM users WHERE username = :username");
        query.bindValue(":username", username);

        qDebug() << "Querying for username: " << username;

        if (!query.exec()) {
            qDebug() << "Query execution failed: " << query.lastError().text();
            db.close();
            return;
        }

        if (query.next()) {
            bool stayLoggedIn = query.value(0).toBool();

            if (stayLoggedIn) {
                QMessageBox::StandardButton reply = QMessageBox::question(
                    this, "Exit",
                    "You're staying logged in. Exit anyway?",
                    QMessageBox::Yes|QMessageBox::No
                    );
                if (reply == QMessageBox::Yes) {
                    db.close();
                    this->close();
                }
            } else {
                db.close();
                this->close();
            }
        } else {
            db.close();
            this->close();
        }
    });


    ClickableLabel *logout = new ClickableLabel(this);
    logout->setText("LOG OUT");
    logout->setStyleSheet(
        "QLabel { color: red; font-size: 24px; font-weight: bold; font-family: 'Times New Roman'; }"
        "QLabel:hover { color: darkred; }"
        );
    logout->setCursor(Qt::PointingHandCursor);
    logout->move(18, 90);
    logout->setFixedSize(200, 40);
    connect(logout, &ClickableLabel::clicked, this, [this]() {
        QSqlDatabase db = QSqlDatabase::database("qt_sql_default_connection");
        if (!db.isOpen()) {
            db = QSqlDatabase::addDatabase("QSQLITE");
            db.setDatabaseName("users.db");
            db.open();
        }

        QSqlQuery query(db);
        query.prepare("UPDATE users SET stay_logged_in = 0 WHERE username = :user");
        query.bindValue(":user", currentUsername);
        query.exec();
        db.close();

        this->close();
        MainWindow *login = new MainWindow();
        login->show();
    });

    QLabel *songnameLabel = new QLabel("< No song selected >", this);
    songnameLabel->move(280, 110);
    songnameLabel->setAlignment(Qt::AlignCenter);
    songnameLabel->setFixedSize(450, 40);
    songnameLabel->setFont(font1);

    QLabel *statusLabel = new QLabel("< Status >", this);
    QFont font2("Times New Roman", 16, QFont::Bold);
    statusLabel->move(465, 170);
    statusLabel->setFont(font2);




    connect(player, &QMediaPlayer::playbackStateChanged, this, [=](QMediaPlayer::PlaybackState state) {
        switch (state) {
        case QMediaPlayer::PlayingState:
            statusLabel->setText("Playing");
            break;
        case QMediaPlayer::PausedState:
            statusLabel->setText("Paused");
            break;
        case QMediaPlayer::StoppedState:
            statusLabel->setText("Stopped");
            break;
        default:
            statusLabel->setText("< Status >");
            break;
        }
    });




    QLabel *currenttimeLabel = new QLabel("00:00", this);
    currenttimeLabel->move(125, 230);
    currenttimeLabel->setFont(font2);

    QLabel *durationtimeLabel = new QLabel("00:00", this);
    durationtimeLabel->move(850, 230);
    durationtimeLabel->setFont(font2);

    // **Fix: Make songSlider a member variable**
    songSlider = new QSlider(Qt::Horizontal, this);
    songSlider->move(190, 224);
    songSlider->setFixedSize(650, 40);
    songSlider->installEventFilter(this);



    connect(player, &QMediaPlayer::durationChanged, this, [=](qint64 duration) {
        durationtimeLabel->setText(formatTime(duration));
        songSlider->setRange(0, duration);  // Ensure the slider's range matches the song duration
    });

    connect(player, &QMediaPlayer::positionChanged, this, [=](qint64 position) {
        if (player->playbackState() == QMediaPlayer::PlayingState) {
            currenttimeLabel->setText(formatTime(position));
            songSlider->setValue(position);  // Use actual position, not percentage
        }
    });



    connect(player, &QMediaPlayer::mediaStatusChanged, this, [=](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia) {
            // Automatically go to the next song when the current song finishes
            qint64 lastPosition = 0;  // Reset the position for the next song

            // Check if songList is empty
            if (songList.isEmpty()) {
                qDebug() << "Song list is empty";
                return;
            }

            // Increment the currentSongIndex
            currentSongIndex++;
            if (currentSongIndex >= songList.size()) {
                currentSongIndex = 0;  // Loop back to the first song if we reach the end
            }

            // Get the next song
            QString nextSong = songList.at(currentSongIndex);
            QString nextSongName = QFileInfo(nextSong).baseName();
            songnameLabel->setText(nextSongName);  // Update the song name label

            player->setSource(QUrl::fromLocalFile(nextSong));  // Set the new song
            player->play();  // Start playing the next song
        }
    });




    QImage image1(":/imgs/prev.png");
    if (image1.isNull()) {
        qDebug() << "Error: Failed to load prev.png";
    } else {
        ClickableLabel *prevbtn = new ClickableLabel(this);
        QPixmap scaledPixmap = QPixmap::fromImage(image1).scaled(50, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        prevbtn->setPixmap(scaledPixmap);
        prevbtn->setFixedSize(scaledPixmap.size());
        prevbtn->setCursor(Qt::PointingHandCursor);
        prevbtn->move(365, 290);
        connect(prevbtn, &ClickableLabel::clicked, [=]() {
            qint64 lastPosition = 0;
            // Handle previous song logic
            if (songList.isEmpty()) {
                qDebug() << "Song list is empty";
                return;
            }

            // Decrement the song index safely
            currentSongIndex--;
            if (currentSongIndex < 0) {
                currentSongIndex = songList.size() - 1;  // Loop back to the last song if we reach the beginning
            }

            // Pause current song and reset player if needed
            lastPosition = player->position();  // Save current position of the song
            player->pause();  // Pause current song before switching

            // Start the previous song
            QString prevSong = songList.at(currentSongIndex);
            QString prevSongName = QFileInfo(prevSong).baseName();
            songnameLabel->setText(prevSongName);

            player->setSource(QUrl::fromLocalFile(prevSong));
            playbtn->setPixmap(QPixmap(":/imgs/pause.png"));
            player->play();  // Play previous song
        });

    }



    connect(songSlider, &QSlider::sliderMoved, this, [=](int position) {
        // Update the player position based on the slider
        player->setPosition(position);

        // If the player is paused, ensure it stays paused
        if (player->playbackState() == QMediaPlayer::PausedState) {
            player->play();
            playbtn->setPixmap(QPixmap(":/imgs/pause.png"));
        }
    });



    qint64 lastPosition = 0;

    // Inside your constructor or setup function
    QImage playImage(":/imgs/play.png");
    QImage pauseImage(":/imgs/pause.png");  // Add pause icon

    if (playImage.isNull() || pauseImage.isNull()) {
        qDebug() << "Error: Failed to load play.png or pause.png";
    } else {
        playbtn = new ClickableLabel(this);

        // Scale the play image to fit the button size
        QPixmap scaledPixmap = QPixmap::fromImage(playImage).scaled(60, 70, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        playbtn->setPixmap(scaledPixmap);
        playbtn->setFixedSize(scaledPixmap.size());
        playbtn->setCursor(Qt::PointingHandCursor);
        playbtn->move(430, 285);  // Set position of the button on the window

        // Connect the button click to toggle play/pause state and update the icon
        connect(playbtn, &ClickableLabel::clicked, [=]() mutable {
            isPlaying = !isPlaying;  // Toggle play/pause state

            // Ensure the song list is not empty and the current song index is valid
            if (songList.isEmpty()) {
                qDebug() << "Song list is empty";
                return;
            }

            if (currentSongIndex < 0 || currentSongIndex >= songList.size()) {
                qDebug() << "Invalid song index: " << currentSongIndex;
                return;
            }

            QString selectedSong = songList.at(currentSongIndex);  // Get the selected song
            QString songNameWithoutExtension = QFileInfo(selectedSong).baseName();  // Get file name without extension
            songnameLabel->setText(songNameWithoutExtension);  // Update the song name label

            if (isPlaying) {
                // If the player is already playing and we paused earlier, resume from last position
                if (!player->isPlaying()) {
                    player->setSource(QUrl::fromLocalFile(songList.at(currentSongIndex)));
                    audioOutput->setVolume(50.0 / 100.0);
                    player->setAudioOutput(audioOutput);

                    if (lastPosition > 0) {
                        player->setPosition(lastPosition);  // Resume from last position
                    }
                    player->play();
                    qDebug() << "Resumed playback from position: " << lastPosition;
                }
            } else {
                // Pause the song and store the current position
                lastPosition = player->position();  // Store position before pausing
                player->pause();
            }


            // Change the button icon based on the state
            QPixmap newPixmap = isPlaying ? QPixmap::fromImage(pauseImage) : QPixmap::fromImage(playImage);
            playbtn->setPixmap(newPixmap.scaled(60, 70, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        });
    }



    QImage image3(":/imgs/next.png");
    if (image3.isNull()) {
        qDebug() << "Error: Failed to load next.png";
    } else {
        ClickableLabel *nextbtn = new ClickableLabel(this);
        QPixmap scaledPixmap = QPixmap::fromImage(image3).scaled(50, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        nextbtn->setPixmap(scaledPixmap);
        nextbtn->setFixedSize(scaledPixmap.size());
        nextbtn->setCursor(Qt::PointingHandCursor);
        nextbtn->move(520, 290);
        connect(nextbtn, &ClickableLabel::clicked, [=]() {
            qint64 lastPosition = 0;
            // Handle next song logic
            if (songList.isEmpty()) {
                qDebug() << "Song list is empty";
                return;
            }

            // Increment the song index safely
            currentSongIndex++;
            if (currentSongIndex >= songList.size()) {
                currentSongIndex = 0;  // Loop back to the first song if we reach the end
            }

            // Pause current song and reset player if needed
            lastPosition = player->position();  // Save current position of the song
            player->pause();  // Pause current song before switching

            // Start the next song
            QString nextSong = songList.at(currentSongIndex);
            QString nextSongName = QFileInfo(nextSong).baseName();
            songnameLabel->setText(nextSongName);

            player->setSource(QUrl::fromLocalFile(nextSong));
            playbtn->setPixmap(QPixmap(":/imgs/pause.png"));
            player->play();  // Play next song
        });
    }

    QImage image4(":/imgs/shuffle.png");
    if (image4.isNull()) {
        qDebug() << "Error: Failed to load shuffle.png";
    } else {
        ClickableLabel *shufflebtn = new ClickableLabel(this);
        QPixmap scaledPixmap = QPixmap::fromImage(image4).scaled(80, 90, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        shufflebtn->setPixmap(scaledPixmap);
        shufflebtn->setFixedSize(scaledPixmap.size());
        shufflebtn->setCursor(Qt::PointingHandCursor);
        shufflebtn->move(600, 275);



        connect(shufflebtn, &ClickableLabel::clicked, this, [=]() {
            shuffleState = !shuffleState;  // Toggle shuffle state

            if (shuffleState) {
                // Shuffle the song list if shuffle is enabled
                std::random_shuffle(songList.begin(), songList.end());
                qDebug() << "Shuffling songs!";
            } else {
                qDebug() << "Shuffle turned off!";
            }


            // Clear the song list widget
            songListWidget->clear();

            // Iterate over the shuffled list and extract just the song names
            for (const auto& songPath : songList) {
                QFileInfo songInfo(songPath);  // Get file info
                QString songName = songInfo.completeBaseName();  // Extract just the song name (file name)

                // Add the song name (not the full path) to the list widget
                songListWidget->addItem(songName);
            }
        });


    }

    QImage image5(":/imgs/volume.png");
    if (image5.isNull()) {
        qDebug() << "Error: Failed to load volume.png";
    } else {
        ClickableLabel *volumeLabel = new ClickableLabel(this);
        QPixmap scaledPixmap = QPixmap::fromImage(image5).scaled(50, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        volumeLabel->setPixmap(scaledPixmap);
        volumeLabel->setFixedSize(scaledPixmap.size());
        volumeLabel->setCursor(Qt::PointingHandCursor);
        volumeLabel->move(360, 395);
    }

    volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->move(420, 400);
    volumeSlider->setFixedSize(255, 40);
    volumeSlider->installEventFilter(this);

    volumeSlider->setRange(0, 100);  // Range of the slider
    volumeSlider->setValue(50);

    connect(volumeSlider, &QSlider::valueChanged, [=](int value) {
        audioOutput->setVolume(value / 100.0);  // Convert the value to a range between 0.0 and 1.0
    });



    QLabel *outerPlaylist = new QLabel(this);
    outerPlaylist->setFixedSize(650, 440);
    outerPlaylist->move(190, 550);
    outerPlaylist->setStyleSheet("background-color: white; border-radius: 14px;"); // Blue screen with rounded edges


    QLabel *innerPlaylist = new QLabel(this);
    innerPlaylist->setFixedSize(570, 350);
    innerPlaylist->move(229, 565);
    innerPlaylist->setStyleSheet("background-color: royalblue; border-radius: 14px;"); // Blue screen with rounded edges


    QPushButton *loadbtn= new QPushButton("Load Songs", this);
    loadbtn->setFixedSize(160, 30);
    loadbtn->move(435, 937);
    loadbtn->setStyleSheet(
        "QPushButton {"
        "font-family: 'Times New Roman';"
        "background-color: royalblue; color: white;"
        "border-radius: 14px; font-size: 18px; font-weight: bold;"
        "}"
        "QPushButton:hover { background-color: #1a3790; }"
        );

    connect(loadbtn, &QPushButton::clicked, this, &MusicPlayer::loadSongs);

    // Create a list widget to display songs inside the innerPlaylist
    songListWidget = new QListWidget(innerPlaylist); // We set the parent to 'innerPlaylist'
    songListWidget->setFixedSize(570, 350); // Match innerPlaylist size
    songListWidget->move(0, 0); // Set position within innerPlaylist
    songListWidget->setStyleSheet("QListWidget { background-color: royalblue; color: white; font-size: 16px; border-radius: 12px; padding: 10px; }");

    QStackedWidget *searchWidget = new QStackedWidget(this);
    searchWidget->setFixedSize(200, 50);

    // Search Label
    searchLabel = new ClickableLabel(this);
    QPixmap searchPixmap(":/imgs/search.png");
    searchLabel->setPixmap(searchPixmap.scaled(30, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    searchLabel->setCursor(Qt::PointingHandCursor);

    // Create search input
    searchEdit = new SearchLineEdit(searchWidget, this);
    searchEdit->setPlaceholderText("Search songs...");
    searchEdit->setFixedSize(200, 40);
    searchEdit->setStyleSheet(
        "QLineEdit {"
        "   font-size: 14px;"
        "   font-family: 'Times New Roman';"
        "   color: black;"
        "   background: white;"
        "   border: 2px solid black;"  // Visible border
        "   border-radius: 12px;"       // Rounded corners
        "   padding: 3px;"             // Better spacing inside the field
        "}"
        "QLineEdit:focus {"
        "   border: 2px solid royalblue;"  // Highlight border when focused
        "}"
        );


    QAction *searchAction = new QAction(this);
    QPixmap searchIcon(":/imgs/search.png");
    searchAction->setIcon(QIcon(searchIcon.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation)));

    // Add the icon inside the line edit
    searchEdit->addAction(searchAction, QLineEdit::LeadingPosition);

    // Add both to stacked widget
    searchWidget->addWidget(searchLabel);
    searchWidget->addWidget(searchEdit);
    searchWidget->setCurrentWidget(searchLabel);
    searchWidget->setFixedSize(searchLabel->size());
    searchWidget->setFixedSize(searchEdit->size());
    searchWidget->move(640, 480);

    // Connect search label click to show input
    connect(searchLabel, &ClickableLabel::clicked, [=]() {
        searchWidget->setCurrentWidget(searchEdit);
        searchEdit->setFocus();
    });

    connect(searchEdit, &QLineEdit::textChanged, this, [=](const QString &searchText) {
        songListWidget->clear();  // Clear the current list in the widget

        // Loop through all songs in the songList and add only matching ones
        for (const QString &song : songList) {
            QFileInfo fileInfo(song);  // Get file information to extract the base name (file name without path/extension)
            QString songName = fileInfo.baseName();  // Get only the file name without the extension

            // Check if the song name contains the search text (case insensitive)
            if (songName.contains(searchText, Qt::CaseInsensitive)) {
                songListWidget->addItem(songName);  // Add only the song name to the widget
            }
        }
    });



    connect(songListWidget, &QListWidget::itemClicked, [=](QListWidgetItem *item) {
        if (!item) {
            qDebug() << "Item is nullptr!";
            return;
        }

        QString songName = item->text();  // Get the selected song name
        qDebug() << "Selected song: " << songName;

        // Find the full path of the song
        QString songPath = findSongPath(songName);

        if (songPath.isEmpty()) {
            qDebug() << "Song path not found for: " << songName;
            return;
        }

        qDebug() << "Song path found: " << songPath;

        // Check if player is initialized
        if (!player) {
            qDebug() << "Player is not initialized!";
            return;
        }

        // Save current song position if a song is already playing
        qint64 lastPosition = 0;
        if (player->playbackState() == QMediaPlayer::PlayingState) {
            lastPosition = player->position();  // Save current position of the song
            player->pause();  // Pause current song before switching
        }

        // Set the new song and play it
        player->setSource(QUrl::fromLocalFile(songPath));
        player->play();
        qDebug() << "Player source set and playback started for: " << songName;

        // Update the song name label with the selected song
        songnameLabel->setText(songName);

        // Update the current song index and path
        currentSongPath = songPath;
        currentSongIndex = songList.indexOf(songPath);  // Update the current song index

        // If the song was paused, we resume it from the saved position
        if (lastPosition > 0) {
            player->setPosition(lastPosition);  // Resume from the previous position
            qDebug() << "Resumed song from position: " << lastPosition;
        }
    });


}

int currentSongIndex = -1;
QList<QString> songList;


QString MusicPlayer::findSongPath(const QString &songName) {
    // Search for the song in the song list and return its full path if found
    for (const QString &songPath : songList) {
        QFileInfo fileInfo(songPath);
        if (fileInfo.completeBaseName() == songName) {
            return songPath;  // Return the full path if the song matches
        }
    }
    return QString();  // Return an empty string if not found
}


MusicPlayer::~MusicPlayer()
{
    delete ui;
}

QString MusicPlayer::formatTime(qint64 time) {
    int seconds = time / 1000;
    int minutes = seconds / 60;
    seconds = seconds % 60;
    return QString::asprintf("%02d:%02d", minutes, seconds);
}

bool MusicPlayer::eventFilter(QObject *obj, QEvent *event) {
    if (obj == songSlider) {
        if (event->type() == QEvent::Enter) {
            songSlider->setStyleSheet(R"(
                QSlider::groove:horizontal { background: #535353; height: 8px; border-radius: 4px; }
                QSlider::handle:horizontal { background: white; width: 16px; height: 16px; border-radius: 8px; margin: -4px 0; }
                QSlider::add-page:horizontal { background: #535353; border-radius: 4px; }
                QSlider::sub-page:horizontal { background: aqua; border-radius: 4px; }
            )");
        } else if (event->type() == QEvent::Leave) {
            songSlider->setStyleSheet(R"(
                QSlider::groove:horizontal { background: #535353; height: 8px; border-radius: 4px; }
                QSlider::handle:horizontal { background: transparent; width: 16px; height: 16px; border-radius: 8px; margin: -4px 0; }
                QSlider::add-page:horizontal { background: #535353; border-radius: 4px; }
                QSlider::sub-page:horizontal { background: white; border-radius: 4px; }
            )");
        }
    }

    if (obj == volumeSlider) {
        if (event->type() == QEvent::Enter) {
            volumeSlider->setStyleSheet(R"(
                QSlider::groove:horizontal { background: #535353; height: 8px; border-radius: 4px; }
                QSlider::handle:horizontal { background: white; width: 16px; height: 16px; border-radius: 8px; margin: -4px 0; }
                QSlider::add-page:horizontal { background: #535353; border-radius: 4px; }
                QSlider::sub-page:horizontal { background: aqua; border-radius: 4px; }
            )");
        } else if (event->type() == QEvent::Leave) {
            volumeSlider->setStyleSheet(R"(
                QSlider::groove:horizontal { background: #535353; height: 8px; border-radius: 4px; }
                QSlider::handle:horizontal { background: transparent; width: 16px; height: 16px; border-radius: 8px; margin: -4px 0; }
                QSlider::add-page:horizontal { background: #535353; border-radius: 4px; }
                QSlider::sub-page:horizontal { background: white; border-radius: 4px; }
            )");
        }
    }



    return QDialog::eventFilter(obj, event);
}



void MusicPlayer::loadSongs() {
    QString directoryPath = QFileDialog::getExistingDirectory(this, "Select Music Directory", QDir::homePath());

    if (!directoryPath.isEmpty()) {
        QDir musicDir(directoryPath);
        QStringList filters;
        filters << "*.mp3" << "*.wav";  // Add other formats you want to support
        musicDir.setNameFilters(filters);

        QFileInfoList files = musicDir.entryInfoList();

        songList.clear();  // Clear the list before loading new songs

        for (const QFileInfo &fileInfo : files) {
            if (fileInfo.isFile()) {
                songList.append(fileInfo.absoluteFilePath());  // Add the full file path to the list
            }
        }

        QFont font("Times New Roman", 12);
        songListWidget->setFont(font);

        songListWidget->clear();  // Clear the list widget

        // Add only the song names without the file extension
        for (const QString &song : songList) {
            QFileInfo songInfo(song);
            QString songNameWithoutExtension = songInfo.completeBaseName();  // Get the name without the extension
            songListWidget->addItem(songNameWithoutExtension);  // Add the name without the extension
        }

        if (!songList.isEmpty()) {
            songListWidget->setCurrentRow(0);  // Set the first song as selected
        }
    }
}



void MusicPlayer::playSong(int index) {
    QString songPath = songList[index];
    player->setSource(QUrl::fromLocalFile(songPath));
    player->play();
    songnameLabel->setText(songList[index]);
}



void MusicPlayer::playSongByIndex(int index) {
    if (index < 0 || index >= songList.size()) return;

    QString songPath = songList.at(index);
    if (QFile::exists(songPath)) {
        player->setSource(QUrl::fromLocalFile(songPath));
        player->play();
    } else {
        qDebug() << "File does not exist:" << songPath;
    }
}



void MusicPlayer::setUsername(const QString &username) {
    currentUsername = username;
    userLabel->setText(username);
}


