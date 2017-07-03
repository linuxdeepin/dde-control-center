#ifndef MIRACASTMODEL_H
#define MIRACASTMODEL_H

#include "miracastworker.h"
#include "miracastdevicemodel.h"
#include <QObject>

namespace dcc {
namespace display {
class MiracastModel : public QObject
{
    Q_OBJECT

    friend class MiracastWorker;

public:
    enum EventType
    {
        LinkManaged = 1,
        LinkUnmanaged,
        SinkConnected,
        SinkConnectFailed,
        SinkDisconnected,
    };

    explicit MiracastModel(QObject *parent = 0);

    const QList<LinkInfo> links() const { return m_links; }

    MiracastDeviceModel* deviceModelByPath(const QString &path);
    LinkInfo &linkByPath(const QDBusObjectPath &path);

signals:
    void linkAdded(const LinkInfo &link) const;
    void linkRemoved(const QDBusObjectPath &path) const;
    void requestLinkScanning(const QDBusObjectPath &path, const bool scanning) const;
    void linkEnableChanged(const QDBusObjectPath &path, const bool enable) const;
    void sinkConnected() const;

private:
    void addSink(const SinkInfo &peer);
    void addLink(const LinkInfo &link);
    void removeLink(const QDBusObjectPath &path);
    void removeSink(const QString &peerInfo);
    void setLinks(const QList<LinkInfo> &links);
    void setSinks(const QList<SinkInfo> &sinks);
    void onPathAdded(const QDBusObjectPath &path, const QString &info);
    void onPathRemoved(const QDBusObjectPath &path, const QString &info);
    void onMiracastEvent(const uchar type, const QDBusObjectPath &path);
    void scanAllLinks();
    void sinkStateChanged(const QDBusObjectPath &path, bool state);

private:
    QList<LinkInfo> m_links;
    QList<SinkInfo> m_sinks;
    QMap<QString, MiracastDeviceModel*> m_deviceModelList;
};
}
}

#endif // MIRACASTMODEL_H
