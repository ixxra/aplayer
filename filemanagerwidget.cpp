#include "filemanagerwidget.h"
#include "ui_filemanagerwidget.h"
#include <QFileSystemModel>


FileManagerWidget::FileManagerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileManagerWidget)
{
    ui->setupUi(this);
    ui->homeButton->setIcon(style()->standardIcon(QStyle::SP_DirHomeIcon));
    ui->upButton->setIcon(style()->standardIcon(QStyle::SP_ArrowUp));

    model = new QFileSystemModel(this);

    connect(ui->fileView, SIGNAL(activated(QModelIndex)), this, SLOT(onEntryActivated(QModelIndex)));
    connect(ui->upButton, SIGNAL(clicked()), this, SLOT(goUp()));
    connect(ui->homeButton, SIGNAL(clicked()), SLOT(goHome()));

    model->setRootPath(QDir::homePath());
    ui->fileView->setModel(model);

    for(int i = 1; i < model->columnCount(); i++){
        ui->fileView->setColumnHidden(i, true);
    }

    ui->fileView->setRootIndex(model->index(QDir::homePath()));
}

FileManagerWidget::~FileManagerWidget()
{
    delete ui;
}

void FileManagerWidget::onEntryActivated(const QModelIndex& index)
{
    if (model->isDir(index)){
        ui->fileView->setRootIndex(index);
        ui->pathLabel->setText(model->fileName(index));
    }
}

void FileManagerWidget::goUp()
{
    const QModelIndex idx = ui->fileView->rootIndex().parent();
    ui->fileView->setRootIndex(idx);
    ui->pathLabel->setText(model->fileName(idx));
}

void FileManagerWidget::goHome()
{
    ui->fileView->setRootIndex(model->index(QDir::homePath()));
    ui->pathLabel->setText("HOME");
}
