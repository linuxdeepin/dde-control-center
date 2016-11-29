#include "updatework.h"

UpdateWork::UpdateWork(QObject *parent) : QObject(parent)
{
    m_updateInter = new UpdateInter("com.deepin.lastore",
                                    "/com/deepin/lastore",
                                    QDBusConnection::systemBus(), this);

    m_jobInter = new JobInter("com.deepin.lastore",
                              "/com/deepin/lastore",
                              QDBusConnection::systemBus(), this);

    m_managerInter = new ManagerInter("com.deepin.lastore",
                                      "/com/deepin/lastore",
                                      QDBusConnection::systemBus(), this);

    connect(m_updateInter, SIGNAL(MirrorSourceChanged(QString)), this, SIGNAL(mirrorSourceChanged(QString)));
    connect(m_updateInter, SIGNAL(AutoCheckUpdatesChanged(bool)), this, SIGNAL(autoCheckUpdatesChanged(bool)));
}

MirrorInfoList UpdateWork::mirrorInfos() const
{
    return m_updateInter->ListMirrorSources(QLocale().name()).value();
}

QString UpdateWork::defaultMirror() const
{
    return m_updateInter->mirrorSource();
}

void UpdateWork::setMirrorSource(const QString &src)
{
    m_updateInter->SetMirrorSource(src);
}

bool UpdateWork::autoUpdate() const
{
    return m_updateInter->autoCheckUpdates();
}

void UpdateWork::setAutoUpdate(bool autoUpdate)
{
    m_updateInter->SetAutoCheckUpdates(autoUpdate);
}
