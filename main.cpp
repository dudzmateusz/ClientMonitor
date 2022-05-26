#include "firstapp.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    firstApp w;
    w.show();
    return a.exec();
}
