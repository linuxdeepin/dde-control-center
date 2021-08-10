#ifndef DCCPLUGINTESTWIDGET_H
#define DCCPLUGINTESTWIDGET_H

#include <QWidget>

#include <interface/frameproxyinterface.h>

class QPushButton;
class DCCNetworkModule;

using namespace DCC_NAMESPACE;

class DccPluginTestWidget : public QWidget, public FrameProxyInterface
{
    Q_OBJECT

public:
    explicit DccPluginTestWidget(QWidget *parent = nullptr);
    ~DccPluginTestWidget();

    void pushWidget(ModuleInterface *const inter, QWidget *const w, PushType type = Normal);

    void popWidget(ModuleInterface *const inter);

    void setModuleVisible(ModuleInterface *const inter, const bool visible);

    void showModulePage(const QString &module, const QString &page, bool animation);

    void setModuleSubscriptVisible(const QString &module, bool bIsDisplay);

    void setRemoveableDeviceStatus(QString type, bool state);
    bool getRemoveableDeviceStatus(QString type) const;

    void setSearchPath(ModuleInterface *const inter) const;

private:
    QPushButton *m_button;
    QWidget *m_topWidget;
    QWidget *m_bottomWidget;

    DCCNetworkModule *m_pModule;
};

#endif // DCCPLUGINTESTWIDGET_H
