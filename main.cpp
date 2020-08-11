#include "Photos_Copy.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Photos_Copy w;
    w.show();
    return a.exec();
}
