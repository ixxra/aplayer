#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "playlistwidget.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QAction* aRemove = ui->actionRemove_Playlist_Selection;

    connect(aRemove, SIGNAL(triggered()), ui->player->playlist(), SLOT(removeSelection()));

}

MainWindow::~MainWindow()
{
    delete ui;
}
