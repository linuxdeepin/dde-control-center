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
    explicit MiracastDeviceModel(const QDBusObjectPath &linkpath, QObject *parent = 0);
    const QList<MiracastItem*> sinkList() const;
    MiracastItem *itemByPath(const QString &path);
    const QDBusObjectPath linkPath() const;

signals:
    void addItem(MiracastItem *item);
    void removeItem(MiracastItem *item);

public slots:
    void onSinkAdded(const SinkInfo &sinkinfo);
    void onSinkRemoved(const SinkInfo &sinkinfo);
    void onSinkConnect(const QDBusObjectPath &sinkPath, bool connected);

private:
    QList<MiracastItem*> m_sinkList;
    const QDBusObjectPath m_linkPath;
};
}
}

#endif // MIRACASTDEVICEMODEL_H
