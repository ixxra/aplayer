#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "playlistwidget.h"
#include <QMediaMetaData>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QAction* aRemove = ui->actionRemove_Playlist_Selection;
    QAction* aClear = ui->actionClear_Playlist;

    connect(aRemove, SIGNAL(triggered()), ui->player->playlist(), SLOT(removeSelection()));
    connect(aClear, SIGNAL(triggered()), ui->player->playlist(), SLOT(clear()));

    connect(ui->player->inner_player(), SIGNAL(metaDataChanged(QString,QVariant)), SLOT(metadataChanged(QString,QVariant)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::metadataChanged(const QString &key, const QVariant &value)
{
    if (key == QMediaMetaData::Title){
        setWindowTitle(value.toString());
    }
}
