#include "playlistwidget.h"
#include "ui_playlistwidget.h"


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

void PlaylistWidget::removeSelection()
{
    QAbstractItemModel* model = ui->playlistView->model();
    QModelIndexList selection = ui->playlistView->selectionModel()->selectedRows();
    qSort(selection);

    QModelIndexList::const_iterator idx = selection.constEnd();
    while (idx != selection.constBegin()){
        --idx;
        model->removeRow(idx->row());
    }
}
