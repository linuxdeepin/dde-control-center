#ifndef MIRACASTDEVICEMODEL_H
#define MIRACASTDEVICEMODEL_H

#include "miracastitem.h"
#include <types/sinkinfolist.h>
#include "types/linkinfolist.h"
#include <QObject>

namespace dcc{
namespace display {
class MiracastDeviceModel : public QObject
{
    Q_OBJECT
public:
    explicit MiracastDeviceModel(const LinkInfo &linkinfo, QObject *parent = 0);
    ~MiracastDeviceModel();

    const QList<SinkInfo> sinkList() const;
    const LinkInfo linkInfo() const;
    bool isConnected() const;

signals:
    void addSink(const SinkInfo &info);
    void removeSink(const SinkInfo &info);
    void linkManageChanged(const bool state);
    void connectStateChanged(const QDBusObjectPath &sinkPath, const bool state);

public slots:
    void onSinkAdded(const SinkInfo &sinkinfo);
    void onSinkRemoved(const SinkInfo &sinkinfo);
    void onSinkConnect(const QDBusObjectPath &sinkPath, bool connected);
    void onLinkManageChanged(const bool state);

private:
    QList<SinkInfo> m_sinkList;
    LinkInfo m_linkInfo;
};
}
}

#endif // MIRACASTDEVICEMODEL_H
