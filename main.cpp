#include "mdplayer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    mdplayer w;
    w.show();

    return a.exec();
}
