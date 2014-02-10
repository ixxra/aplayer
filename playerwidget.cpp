#include "playerwidget.h"
#include "ui_playerwidget.h"

PlayerWidget::PlayerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayerWidget)
{
    ui->setupUi(this);

    QStyle* st = style();
    ui->prevButton->setIcon(st->standardIcon(QStyle::SP_MediaSkipBackward));
    ui->playButton->setIcon(st->standardIcon(QStyle::SP_MediaPlay));
    ui->nextButton->setIcon(st->standardIcon(QStyle::SP_MediaSkipForward));
    ui->muteButton->setIcon(st->standardIcon(QStyle::SP_MediaVolume));
}

PlayerWidget::~PlayerWidget()
{
    delete ui;
}
