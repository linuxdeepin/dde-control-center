#ifndef DEFAPPWORKER_H
#define DEFAPPWORKER_H

#include "defappmodel.h"

#include <com_deepin_api_manager.h>
#include <com_deepin_api_media.h>
#include <QObject>
#include <QStringList>
#include <QList>

using com::deepin::api::Manager;
using com::deepin::api::Media;

class DefAppWorker : public QObject
{
    Q_OBJECT
public:
    explicit DefAppWorker(DefAppModel *m_defAppModel, QObject *parent = 0);
    void active();
    void deactive();

public slots:
    void onSetDefaultAppChanged(const QString &name, const QString &category);
    void onGetDefaultAppChanged();
    void onGetListAppsChanged();
    void onResetTriggered();
    void onAutoOpenChanged(const bool state);

private slots:
    void getListAppFinished(QDBusPendingCallWatcher *w);
    void getDefaultAppFinished(QDBusPendingCallWatcher *w);
    void serviceStartFinished();

private:
    DefAppModel *m_defAppModel;
    Manager     *m_dbusManager;
    Media       *m_dbusMedia;

    enum DefaultAppsCategory {
        Browser, Mail, Text, Music, Video, Picture, Terminal,
        CD_Audio, DVD_Video, MusicPlayer, Camera, Software,
    };
    QMap<QString, DefaultAppsCategory> m_stringToCategory;

private:
    const QString getTypeByCategory(const DefAppWorker::DefaultAppsCategory &category);
    const QStringList getTypeListByCategory(const DefAppWorker::DefaultAppsCategory &category);
    bool isMediaApps(const DefaultAppsCategory &category) const;
};

#endif // DEFAPPWORKER_H
