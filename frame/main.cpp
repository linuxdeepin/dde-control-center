#include <QtWidgets>

#include "frame.h"
#include "interfaces.h"

int main(int argv, char *args[])
{
    QApplication app(argv, args);

    Frame frame;
    frame.show();

    return app.exec();
}
