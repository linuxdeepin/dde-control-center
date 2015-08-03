#include "frame.h"
#include "interfaces.h"

#include <libdui/dapplication.h>
#include <libdui/dthememanager.h>

DUI_USE_NAMESPACE

int main(int argv, char *args[])
{
    DApplication app(argv, args);

    Frame frame;
    frame.show();

    DThemeManager *manager = DThemeManager::instance();
    manager->setTheme("dark");

    return app.exec();
}
