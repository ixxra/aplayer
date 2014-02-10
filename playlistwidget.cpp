#include "playlistwidget.h"
#include "ui_playlistwidget.h"
#include <QMediaPlaylist>
#include "playlistmodel.h"

PlaylistWidget::PlaylistWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlaylistWidget)
{
    ui->setupUi(this);
    ui->playlistView->setDragDropMode(QAbstractItemView::DropOnly);

    PlaylistModel* model = new PlaylistModel(this);
    QMediaPlaylist* playlist = new QMediaPlaylist(this);
    model->setPlaylist(playlist);

    ui->playlistView->setModel(model);
}

PlaylistWidget::~PlaylistWidget()
{
    delete ui;
}
