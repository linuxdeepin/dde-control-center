#ifndef MIRACASTMODEL_H
#define MIRACASTMODEL_H

#include "miracastworker.h"

#include <QObject>

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

signals:
    void linkAdded(const LinkInfo &link) const;
    void linkRemoved(const QDBusObjectPath &path) const;
    void peerAdded(const SinkInfo &peer) const;
    void peerRemoved(const SinkInfo &peer) const;
    void requestLinkScanning(const QDBusObjectPath &path, const bool scanning) const;
    void linkInfoChanged() const;
    void sinkConnectedChanged(const QDBusObjectPath &sinkPath, const bool connected) const;

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

    LinkInfo &linkByPath(const QDBusObjectPath &path);

private:
    QList<LinkInfo> m_links;
};

#endif // MIRACASTMODEL_H
