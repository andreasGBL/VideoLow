#include "include/videolowwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    VideoLowWindow w;
    w.show();
    return a.exec();
}
