#ifndef SYSTEMINFOINTERACTION_H
#define SYSTEMINFOINTERACTION_H

#include "systeminfomodel.h"
#include "systeminfowork.h"

#include <QObject>

using namespace DCC_NAMESPACE;

class SystemInfoInteraction : public QObject
{
    Q_OBJECT
public:
    explicit SystemInfoInteraction(QObject *parent = nullptr);

    Q_INVOKABLE SystemInfoWork *systemInfoWork() const;
    void setSystemInfoWork(SystemInfoWork *newSystemInfoWork);

    Q_INVOKABLE SystemInfoModel *systemInfoMode() const;
    void setSystemInfoMode(SystemInfoModel *newSystemInfoMode);

signals:


private:
    SystemInfoWork* m_systemInfoWork;
    SystemInfoModel* m_systemInfoMode;
};

#endif // SYSTEMINFOINTERACTION_H
