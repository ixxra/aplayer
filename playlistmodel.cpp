#include "playlistmodel.h"
#include <QMediaPlaylist>
#include <QFileInfo>
#include <QDebug>
#include <QMimeDatabase>
#include <QMimeData>
#include <QFileInfo>
#include <QApplication>
#include <QStyle>


PlaylistModel::PlaylistModel(QObject *parent) :
    QAbstractItemModel(parent), m_playlist(0), m_current(-1)
{
}


int PlaylistModel::rowCount(const QModelIndex &parent) const
{
    return m_playlist && !parent.isValid() ? m_playlist->mediaCount() : 0;
}

int PlaylistModel::columnCount(const QModelIndex &parent) const
{
    return !parent.isValid() ? 1 : 0;
}

bool PlaylistModel::insertRows(int row, int count, const QModelIndex &parent)
{
    qDebug() << "insertrows";
    Q_UNUSED(parent);

    if (!m_playlist)
        return false;

    beginInsertItems(row, row + count - 1);
    for (int k = 0; k < count; k++){
        m_playlist->insertMedia(row, QMediaContent());
        ++row;
    }
    endInsertRows();

    return true;
}

bool PlaylistModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if (count < 0 || row < 0 || row >= rowCount()){
        return false;
    }
    return m_playlist->removeMedia(row, row + count - 1);
}

QModelIndex PlaylistModel::index(int row, int column, const QModelIndex &parent) const
{
    return m_playlist && !parent.isValid()
                && row >= 0 && row < m_playlist->mediaCount()
                && column == 0
            ? createIndex(row, column)
            : QModelIndex();
}

QModelIndex PlaylistModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);

    return QModelIndex();
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid() && role == Qt::DisplayRole) {
        QVariant value = m_data[index];
        if (!value.isValid() && index.column() == 0) {
            QUrl location = m_playlist->media(index.row()).canonicalUrl();
            return QFileInfo(location.path()).fileName();
        }

        return value;

    } else if (index.isValid() && role == Qt::DecorationRole && index.row() == m_playlist->currentIndex()) {
        return QApplication::style()->standardIcon(QStyle::SP_MediaVolume);
    }
    return QVariant();
}

QMediaPlaylist *PlaylistModel::playlist() const
{
    return m_playlist;
}

void PlaylistModel::setPlaylist(QMediaPlaylist *playlist)
{
    if (m_playlist) {
            disconnect(m_playlist, SIGNAL(mediaAboutToBeInserted(int,int)), this, SLOT(beginInsertItems(int,int)));
            disconnect(m_playlist, SIGNAL(mediaInserted(int,int)), this, SLOT(endInsertItems()));
            disconnect(m_playlist, SIGNAL(mediaAboutToBeRemoved(int,int)), this, SLOT(beginRemoveItems(int,int)));
            disconnect(m_playlist, SIGNAL(mediaRemoved(int,int)), this, SLOT(endRemoveItems()));
            disconnect(m_playlist, SIGNAL(mediaChanged(int,int)), this, SLOT(changeItems(int,int)));
            disconnect(m_playlist, SIGNAL(currentIndexChanged(int)), this, SLOT(on_current_changed(int)));
    }

    beginResetModel();
    m_playlist = playlist;

    if (m_playlist) {
            connect(m_playlist, SIGNAL(mediaAboutToBeInserted(int,int)), this, SLOT(beginInsertItems(int,int)));
            connect(m_playlist, SIGNAL(mediaInserted(int,int)), this, SLOT(endInsertItems()));
            connect(m_playlist, SIGNAL(mediaAboutToBeRemoved(int,int)), this, SLOT(beginRemoveItems(int,int)));
            connect(m_playlist, SIGNAL(mediaRemoved(int,int)), this, SLOT(endRemoveItems()));
            connect(m_playlist, SIGNAL(mediaChanged(int,int)), this, SLOT(changeItems(int,int)));
            connect(m_playlist, SIGNAL(currentIndexChanged(int)), this, SLOT(on_current_changed(int)));
    }

    endResetModel();
}

bool PlaylistModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    qDebug() << "setData";
    Q_UNUSED(role);
    m_data[index] = value;
    emit dataChanged(index, index);
    return true;
}


void PlaylistModel::beginInsertItems(int start, int end)
{
    qDebug() << "beginInsert";
    m_data.clear();
    beginInsertRows(QModelIndex(), start, end);
}

void PlaylistModel::endInsertItems()
{
    qDebug() << "endInsert";
    endInsertRows();
}

void PlaylistModel::beginRemoveItems(int start, int end)
{
    qDebug() << "beginRemove";
    m_data.clear();
    beginRemoveRows(QModelIndex(), start, end);
}

void PlaylistModel::endRemoveItems()
{
    qDebug() << "endRemove";
    endRemoveRows();
}

void PlaylistModel::changeItems(int start, int end)
{
    qDebug() << "changeItems";
    m_data.clear();
    emit dataChanged(index(start, 0), index(end, 0));
}

void PlaylistModel::on_current_changed(int current)
{
    if (m_current != -1) {
        emit dataChanged(index(m_current, 0), index(m_current, 0), QVector<int>(Qt::DecorationRole));
    }

    m_current = current;
    emit dataChanged(index(m_current, 0), index(m_current, 0), QVector<int>(Qt::DecorationRole));
}

/************************* Drag and Drop *******************************/



QStringList PlaylistModel::mimeTypes() const
{
    return QStringList (QLatin1String("text/uri-list"));
}


Qt::DropActions PlaylistModel::supportedDropActions() const
{
    return Qt::CopyAction;
}


bool PlaylistModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    Q_UNUSED(column);
    Q_UNUSED(parent);

    if (action != Qt::CopyAction)
        return false;

    if (!data->hasUrls())
        return false;

    if (!m_playlist)
        return false;


    QList<QMediaContent> media;
    foreach(QUrl u, data->urls()){
        if (u.isLocalFile()){
            QFileInfo f(u.toLocalFile());
            if (f.isFile()){
                qDebug() << "Appending local file" << f.fileName();
                media << u;
            } else if (f.isDir()) {
                qDebug() << "Skipping dir" << f.fileName();
                return false;
            } else {
                qDebug() << "Skipping file" << f.fileName();
                return false;
            }
        } else {
            qDebug() << "Appending non local resource" << u;
            media << u;
        }
    }

    if (row == -1) {
        m_playlist->addMedia(media);
    } else {
        m_playlist->insertMedia(row + 1, media);
    }

    return true;
}

Qt::ItemFlags PlaylistModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = Qt::ItemNeverHasChildren | QAbstractItemModel::flags(index);


    if (index.isValid())
       return defaultFlags;
    else
       return Qt::ItemIsDropEnabled | defaultFlags;
}
