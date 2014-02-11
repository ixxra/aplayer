#ifndef PLAYLISTWIDGET_H
#define PLAYLISTWIDGET_H

#include <QWidget>

namespace Ui {
class PlaylistWidget;
}

class QMediaPlaylist;
class QAbstractItemView;

class PlaylistWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlaylistWidget(QWidget *parent = 0);
    ~PlaylistWidget();

    void setPlaylist(QMediaPlaylist* playlist);
    QAbstractItemView* view();

private:
    Ui::PlaylistWidget *ui;
};

#endif // PLAYLISTWIDGET_H
