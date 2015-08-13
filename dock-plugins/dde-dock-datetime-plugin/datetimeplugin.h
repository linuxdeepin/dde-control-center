#ifndef DATETIMEPLUGIN_H
#define DATETIMEPLUGIN_H

#include <dde-dock/dockconstants.h>
#include <dde-dock/dockplugininterface.h>
#include <dde-dock/dockpluginproxyinterface.h>

#include <clockpixmap.h>
#include <libdui/dcalendar.h>

DUI_USE_NAMESPACE

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

    QString getPluginName() Q_DECL_OVERRIDE;

    QStringList ids() Q_DECL_OVERRIDE;
    QString getName(QString id) Q_DECL_OVERRIDE;
    QString getTitle(QString id) Q_DECL_OVERRIDE;
    QString getCommand(QString id) Q_DECL_OVERRIDE;
    bool canDisable(QString id) Q_DECL_OVERRIDE;
    bool isDisabled(QString id) Q_DECL_OVERRIDE;
    void setDisabled(QString id, bool disabled) Q_DECL_OVERRIDE;
    QWidget * getItem(QString id) Q_DECL_OVERRIDE;
    QWidget * getApplet(QString id) Q_DECL_OVERRIDE;
    void changeMode(Dock::DockMode newMode, Dock::DockMode oldMode) Q_DECL_OVERRIDE;

    QString getMenuContent(QString id) Q_DECL_OVERRIDE;
    void invokeMenuItem(QString id, QString itemId, bool checked) Q_DECL_OVERRIDE;

private:
    QString m_id = "id_datetime";
    DockPluginProxyInterface * m_proxy;

    DCalendar * m_calendar;
    QLabel * m_item = NULL;
    QTimer * m_timer = NULL;
    ClockPixmap m_clockPixmap;
    Dock::DockMode m_mode;
    bool m_showWeek;
    bool m_showDate;

private:
    void initCalendar();
    void setMode(Dock::DockMode mode);
    QJsonObject createMenuItem(QString itemId,
                               QString itemName,
                               bool checkable = false,
                               bool checked = false);

private slots:
    void updateTime();
};

#endif // DATETIMEPLUGIN_H
