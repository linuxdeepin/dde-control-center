#ifndef COMPOSITECOMPONENT_H
#define COMPOSITECOMPONENT_H

#include <QObject>
#include <QLabel>

#include "plugincomponentinterface.h"

class CompositeComponent : public QObject, public PluginComponentInterface
{
    Q_OBJECT
public:
    CompositeComponent(QObject *parent = 0);

    QString getId() Q_DECL_OVERRIDE;
    QString getName() Q_DECL_OVERRIDE;
    QString getTitle() Q_DECL_OVERRIDE;
    QString getCommand() Q_DECL_OVERRIDE;
    QWidget * getItem() Q_DECL_OVERRIDE;
    void retainItem() Q_DECL_OVERRIDE;
    QWidget * getApplet() Q_DECL_OVERRIDE;

    QString getMenuContent() Q_DECL_OVERRIDE;
    void invokeMenuItem(QString itemId, bool checked) Q_DECL_OVERRIDE;

private:
    QLabel * m_item;
};

#endif // COMPOSITECOMPONENT_H
