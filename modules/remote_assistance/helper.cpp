#include "helper.h"
#include <QFile>

#include <libdui/dthememanager.h>

DUI_USE_NAMESPACE

QString readStyleSheet(const QString &widgetName)
{
    return DThemeManager::instance()->getQssForWidget(widgetName);
}

QString getThemeImage(const QString &iconName)
{
    return QString{":/%1/images/%2"}.arg(DThemeManager::instance()->theme(), iconName);
}
