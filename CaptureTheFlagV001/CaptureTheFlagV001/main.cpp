#include "CaptureTheFlagV001.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CaptureTheFlagV001 w;
    w.show();
    return a.exec();
}
