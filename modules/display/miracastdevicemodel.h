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

    const QList<MiracastItem*> sinkList() const;
    MiracastItem *itemByPath(const QString &path);
    const LinkInfo linkInfo() const;

signals:
    void addItem(MiracastItem *item);
    void removeItem(MiracastItem *item);
    void linkManageChanged(const bool state);

public slots:
    void onSinkAdded(const SinkInfo &sinkinfo);
    void onSinkRemoved(const SinkInfo &sinkinfo);
    void onSinkConnect(const QDBusObjectPath &sinkPath, bool connected);
    void onLinkManageChanged(const bool state);
    void clear();

private:
    QList<MiracastItem*> m_sinkList;
    LinkInfo m_linkInfo;
};
}
}

#endif // MIRACASTDEVICEMODEL_H
