#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "playlistwidget.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QAction* aRemove = ui->actionRemove_Playlist_Selection;
    QAction* aClear = ui->actionClear_Playlist;

    connect(aRemove, SIGNAL(triggered()), ui->player->playlist(), SLOT(removeSelection()));
    connect(aClear, SIGNAL(triggered()), ui->player->playlist(), SLOT(clear()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
