#include "mainwindow.h"
#include <QSystemTrayIcon>
#include <QApplication>
#include <QStyle>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QSystemTrayIcon tray(a.style()->standardIcon(QStyle::SP_MediaPlay), &w);
    tray.show();
    return a.exec();
}
