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

}

PlaylistWidget::~PlaylistWidget()
{
    delete ui;
}

QAbstractItemView* PlaylistWidget::view()
{
    return ui->playlistView;
}
