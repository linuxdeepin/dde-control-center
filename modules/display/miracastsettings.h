#ifndef MIRACASTSETTINGS_H
#define MIRACASTSETTINGS_H

#include "contentwidget.h"
#include "switchwidget.h"
#include "settingsgroup.h"
#include "miracastnodevicewidget.h"
#include "miracastdevicemodel.h"
#include "miracastitem.h"
#include "miracastmodel.h"
#include "../widgets/labels/normallabel.h"
#include "QVBoxLayout"

using namespace dcc::widgets;

namespace dcc {

namespace display {

class MiracastPage : public ContentWidget
{
    Q_OBJECT
public:
    explicit MiracastPage(const QString &title, QWidget *parent = 0);
    void setModel(MiracastDeviceModel *model);

signals:
    void requestDeviceEnable(const QDBusObjectPath &path, const bool enable);
    void requestDeviceConnect(const QDBusObjectPath &path, const QRect &rect);
    void requestDeviceDisConnect(const QDBusObjectPath &path);
    void requestDeviceRefreshed(const QDBusObjectPath &path, const bool enable);

private slots:
    void onItemAdded(const SinkInfo &info);
    void onItemRemoved(const SinkInfo &info);
    void onDeviceEnableChanged(const bool enable);
    void onRefreshed();
    void onDeviceStateChanged(const bool state);
    void onDeviceManaged(const bool managed);
    void onRefreshTimeout();
    void onSinkConnectChanged(const QDBusObjectPath &sinkPath, const bool state);

private:
    MiracastItem *itemByPath(const QString &path);

private:
    QVBoxLayout *m_mainLayout;
    MiracastNoDeviceWidget *m_nodevice;
    SettingsGroup *m_deviceGrp;
    MiracastDeviceModel *m_model;
    SwitchWidget *m_deviceSwBtn;
    NormalLabel *m_tip;
    QTimer *m_refreshTimer;
    DImageButton *m_refreshBtn;
    QMap<QString, MiracastItem*> m_deviceItemList;
};

}
}

#endif // MIRACASTSETTINGS_H
