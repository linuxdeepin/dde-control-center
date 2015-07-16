#ifndef BLUETOOTHCOMPONENT_H
#define BLUETOOTHCOMPONENT_H

#include <QObject>

#include "plugincomponentinterface.h"

class QLabel;
class QFrame;
class SwitchButton;
class BluetoothComponent : public QObject, public PluginComponentInterface
{
    Q_OBJECT
public:
    BluetoothComponent(QObject *parent = 0);
    ~BluetoothComponent();

    QString getUUID() Q_DECL_OVERRIDE;

    QString getTitle() Q_DECL_OVERRIDE;
    QWidget * getItem() Q_DECL_OVERRIDE;
    QWidget * getApplet() Q_DECL_OVERRIDE;

    QString getMenuContent() Q_DECL_OVERRIDE;
    void invokeMenuItem(QString itemId, bool checked) Q_DECL_OVERRIDE;

private:
    QLabel * m_item;
    QLabel * m_label;
    SwitchButton * m_switchButton;
    QFrame * m_appletFrame;
};

#endif // BLUETOOTHCOMPONENT_H
