#ifndef PLUGINCOMPONENTINTERFACE_H
#define PLUGINCOMPONENTINTERFACE_H

#include <QString>

class QWidget;
class PluginComponentInterface
{
public:
    virtual QString getUUID() = 0;

    virtual QString getTitle() = 0;
    virtual QWidget * getItem() = 0;
    virtual QWidget * getApplet() = 0;

    virtual QString getMenuContent() = 0;
    virtual void invokeMenuItem(QString itemId, bool checked) = 0;
};

#endif //PLUGINCOMPONENTINTERFACE_H
