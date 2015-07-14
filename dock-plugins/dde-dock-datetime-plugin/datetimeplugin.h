#ifndef DATETIMEPLUGIN_H
#define DATETIMEPLUGIN_H

#include <dock/dockconstants.h>
#include <dock/dockplugininterface.h>
#include <dock/dockpluginproxyinterface.h>

#include <clockpixmap.h>

class QLabel;
class QTimer;
class DateTimePlugin : public QObject, public DockPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID DockPluginInterface_iid FILE "dde-dock-datetime-plugin.json")
    Q_INTERFACES(DockPluginInterface)

public:
    DateTimePlugin();
    ~DateTimePlugin();

    void init(DockPluginProxyInterface *proxy) Q_DECL_OVERRIDE;

    QString name() Q_DECL_OVERRIDE;

    QStringList uuids() Q_DECL_OVERRIDE;
    QString getTitle(QString uuid) Q_DECL_OVERRIDE;
    QWidget * getItem(QString uuid) Q_DECL_OVERRIDE;
    QWidget * getApplet(QString uuid) Q_DECL_OVERRIDE;
    void changeMode(Dock::DockMode newMode, Dock::DockMode oldMode) Q_DECL_OVERRIDE;

private:
    QString m_uuid = "datetime";
    DockPluginProxyInterface * m_proxy;

    QLabel * m_item = NULL;
    QTimer * m_timer = NULL;
    ClockPixmap m_clockPixmap;
    Dock::DockMode m_mode;

    void setMode(Dock::DockMode mode);

private slots:
    void updateTime();
};

#endif // DATETIMEPLUGIN_H
