/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include "playerwidget.h"
#include "ui_playerwidget.h"

#include "playercontrols.h"
#include "playlistmodel.h"
#include "playlistwidget.h"

#include <QMediaService>
#include <QMediaPlaylist>
#include <QMediaMetaData>
#include <QtWidgets>

#include <QDebug>

#include <taglib/fileref.h>



PlayerWidget::PlayerWidget(QWidget *parent) :
    QWidget(parent),
    coverLabel(0),
    slider(0),
    ui(new Ui::PlayerWidget)
{
    //ui->setupUi(this);

    player = new QMediaPlayer(this);
    // owned by PlaylistModel
    m_playlist = new QMediaPlaylist();

    connect(m_playlist, SIGNAL(mediaInserted(int,int)), SLOT(updateMetadata(int,int)));

    player->setPlaylist(m_playlist);

    connect(player, SIGNAL(durationChanged(qint64)), SLOT(durationChanged(qint64)));
    connect(player, SIGNAL(positionChanged(qint64)), SLOT(positionChanged(qint64)));
    connect(player, SIGNAL(metaDataChanged()), SLOT(metaDataChanged()));
    connect(m_playlist, SIGNAL(currentIndexChanged(int)), SLOT(playlistPositionChanged(int)));
    connect(player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
            this, SLOT(statusChanged(QMediaPlayer::MediaStatus)));
    connect(player, SIGNAL(bufferStatusChanged(int)), this, SLOT(bufferingProgress(int)));
    connect(player, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(displayErrorMessage()));

    playlistModel = new PlaylistModel(this);
    playlistModel->setPlaylist(m_playlist);

    playlistWidget = new PlaylistWidget(this);
    playlistView = playlistWidget->view();

    playlistView->setModel(playlistModel);
    playlistView->setCurrentIndex(playlistModel->index(m_playlist->currentIndex(), 0));



    connect(playlistView, SIGNAL(activated(QModelIndex)), this, SLOT(jump(QModelIndex)));

    slider = new QSlider(Qt::Horizontal, this);
    slider->setRange(0, player->duration() / 1000);

    labelDuration = new QLabel(this);
    connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(seek(int)));


    QPushButton *openButton = new QPushButton(tr("Open"), this);

    connect(openButton, SIGNAL(clicked()), this, SLOT(open()));

    PlayerControls *controls = new PlayerControls(this);
    controls->setState(player->state());
    controls->setVolume(player->volume());
    controls->setMuted(controls->isMuted());

    connect(controls, SIGNAL(play()), player, SLOT(play()));
    connect(controls, SIGNAL(pause()), player, SLOT(pause()));
    connect(controls, SIGNAL(stop()), player, SLOT(stop()));
    connect(controls, SIGNAL(next()), m_playlist, SLOT(next()));
    connect(controls, SIGNAL(previous()), this, SLOT(previousClicked()));
    connect(controls, SIGNAL(changeVolume(int)), player, SLOT(setVolume(int)));
    connect(controls, SIGNAL(changeMuting(bool)), player, SLOT(setMuted(bool)));
    //connect(controls, SIGNAL(changeRate(qreal)), player, SLOT(setPlaybackRate(qreal)));

    connect(player, SIGNAL(stateChanged(QMediaPlayer::State)),
            controls, SLOT(setState(QMediaPlayer::State)));
    connect(player, SIGNAL(volumeChanged(int)), controls, SLOT(setVolume(int)));
    connect(player, SIGNAL(mutedChanged(bool)), controls, SLOT(setMuted(bool)));


    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setMargin(0);
    controlLayout->addWidget(openButton);
    controlLayout->addStretch(1);
    controlLayout->addWidget(controls);
    controlLayout->addStretch(1);

    QBoxLayout *layout = new QVBoxLayout;
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(slider);
    hLayout->addWidget(labelDuration);
    layout->addWidget(playlistWidget);
    layout->addLayout(hLayout);
    layout->addLayout(controlLayout);

    setLayout(layout);

    if (!player->isAvailable()) {
        QMessageBox::warning(this, tr("Service not available"),
                             tr("The QMediaPlayer object does not have a valid service.\n"\
                                "Please check the media service plugins are installed."));

        controls->setEnabled(false);
        playlistView->setEnabled(false);
        openButton->setEnabled(false);

    }

    metaDataChanged();

    QStringList arguments = qApp->arguments();
    arguments.removeAt(0);
    addToPlaylist(arguments);

}



PlayerWidget::~PlayerWidget()
{
    delete ui;
}

QMediaPlayer* PlayerWidget::inner_player()
{
    return player;
}

void PlayerWidget::open()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open Files"));
    addToPlaylist(fileNames);
}

void PlayerWidget::addToPlaylist(const QStringList& fileNames)
{
    foreach (QString const &argument, fileNames) {
        QFileInfo fileInfo(argument);
        if (fileInfo.exists()) {
            QUrl url = QUrl::fromLocalFile(fileInfo.absoluteFilePath());
            if (fileInfo.suffix().toLower() == QLatin1String("m3u")) {
                m_playlist->load(url);
            } else
                m_playlist->addMedia(url);
        } else {
            QUrl url(argument);
            if (url.isValid()) {
                m_playlist->addMedia(url);
            }
        }
    }
}

PlaylistWidget* PlayerWidget::playlist()
{
    return playlistWidget;
}

void PlayerWidget::durationChanged(qint64 duration)
{
    this->duration = duration/1000;
    slider->setMaximum(duration / 1000);
}

void PlayerWidget::positionChanged(qint64 progress)
{
    if (!slider->isSliderDown()) {
        slider->setValue(progress / 1000);
    }
    updateDurationInfo(progress / 1000);
}

void PlayerWidget::metaDataChanged()
{
    if (player->isMetaDataAvailable()) {
        setTrackInfo(QString("%1 - %2")
                .arg(player->metaData(QMediaMetaData::AlbumArtist).toString())
                .arg(player->metaData(QMediaMetaData::Title).toString()));

        if (coverLabel) {
            QUrl url = player->metaData(QMediaMetaData::CoverArtUrlLarge).value<QUrl>();

            coverLabel->setPixmap(!url.isEmpty()
                    ? QPixmap(url.toString())
                    : QPixmap());
        }
    }
}

void PlayerWidget::previousClicked()
{
    // Go to previous track if we are within the first 5 seconds of playback
    // Otherwise, seek to the beginning.
    if(player->position() <= 5000)
        m_playlist->previous();
    else
        player->setPosition(0);
}

void PlayerWidget::jump(const QModelIndex &index)
{
    if (index.isValid()) {
        m_playlist->setCurrentIndex(index.row());
        player->play();
    }
}

void PlayerWidget::playlistPositionChanged(int currentItem)
{
    playlistView->setCurrentIndex(playlistModel->index(currentItem, 0));
}

void PlayerWidget::seek(int seconds)
{
    player->setPosition(seconds * 1000);
}

void PlayerWidget::statusChanged(QMediaPlayer::MediaStatus status)
{
    handleCursor(status);

    // handle status message
    switch (status) {
    case QMediaPlayer::UnknownMediaStatus:
    case QMediaPlayer::NoMedia:
    case QMediaPlayer::LoadedMedia:
    case QMediaPlayer::BufferingMedia:
    case QMediaPlayer::BufferedMedia:
        setStatusInfo(QString());
        break;
    case QMediaPlayer::LoadingMedia:
        setStatusInfo(tr("Loading..."));
        break;
    case QMediaPlayer::StalledMedia:
        setStatusInfo(tr("Media Stalled"));
        break;
    case QMediaPlayer::EndOfMedia:
        QApplication::alert(this);
        break;
    case QMediaPlayer::InvalidMedia:
        displayErrorMessage();
        break;
    }
}

void PlayerWidget::handleCursor(QMediaPlayer::MediaStatus status)
{
#ifndef QT_NO_CURSOR
    if (status == QMediaPlayer::LoadingMedia ||
        status == QMediaPlayer::BufferingMedia ||
        status == QMediaPlayer::StalledMedia)
        setCursor(QCursor(Qt::BusyCursor));
    else
        unsetCursor();
#endif
}

void PlayerWidget::bufferingProgress(int progress)
{
    setStatusInfo(tr("Buffering %4%").arg(progress));
}

void PlayerWidget::setTrackInfo(const QString &info)
{
    trackInfo = info;
    if (!statusInfo.isEmpty())
        setWindowTitle(QString("%1 | %2").arg(trackInfo).arg(statusInfo));
    else
        setWindowTitle(trackInfo);
}

void PlayerWidget::setStatusInfo(const QString &info)
{
    statusInfo = info;
    if (!statusInfo.isEmpty())
        setWindowTitle(QString("%1 | %2").arg(trackInfo).arg(statusInfo));
    else
        setWindowTitle(trackInfo);
}

void PlayerWidget::displayErrorMessage()
{
    setStatusInfo(player->errorString());
}

void PlayerWidget::updateDurationInfo(qint64 currentInfo)
{
    QString tStr;
    if (currentInfo || duration) {
        QTime currentTime((currentInfo/3600)%60, (currentInfo/60)%60, currentInfo%60, (currentInfo*1000)%1000);
        QTime totalTime((duration/3600)%60, (duration/60)%60, duration%60, (duration*1000)%1000);
        QString format = "mm:ss";
        if (duration > 3600)
            format = "hh:mm:ss";
        tStr = currentTime.toString(format) + " / " + totalTime.toString(format);
    }
    labelDuration->setText(tStr);
}

void PlayerWidget::updateMetadata(int start, int end)
{
    for (int i = start; i <= end; i++){
        QUrl u = m_playlist->media(i).canonicalUrl();
        if (u.isLocalFile()){
            TagLib::FileRef f = TagLib::FileRef(u.toLocalFile().toLocal8Bit().data());
            QString d(f.tag()->title().toCString());
            QModelIndex idx = playlistModel->index(i, 0);
            if (!d.isEmpty())
                playlistModel->setData(idx, d);

        }
    }
}


