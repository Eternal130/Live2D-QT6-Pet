#include <QtWidgets/QApplication>
#include "GLCore.h"
#include "ElaApplication.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GLCore w(720, 480);
    eApp->init();
    w.show();
    return a.exec();
}
