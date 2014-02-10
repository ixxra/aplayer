#ifndef FILEMANAGERWIDGET_H
#define FILEMANAGERWIDGET_H

#include <QWidget>

namespace Ui {
class FileManagerWidget;
}

class QFileSystemModel;


class FileManagerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileManagerWidget(QWidget *parent = 0);
    ~FileManagerWidget();

private:
    Ui::FileManagerWidget *ui;
    QFileSystemModel* model;

private slots:
    void onEntryActivated(const QModelIndex& index);
    void goUp();
    void goHome();
};

#endif // FILEMANAGERWIDGET_H
