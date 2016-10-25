#ifndef DEFAPPWORKER_H
#define DEFAPPWORKER_H

#include "defappmodel.h"

#include <com_deepin_api_manager.h>
#include <com_deepin_api_media.h>
#include <QObject>
#include <QStringList>

using com::deepin::api::Manager;
using com::deepin::api::Media;

class DefAppWorker : public QObject
{
    Q_OBJECT
public:
    explicit DefAppWorker(DefAppModel *m_defAppModel, QObject *parent = 0);

    enum DefaultAppsCategory {
        Browser, Mail, Text, Music, Video, Picture, Terminal,
        CD_Audio, DVD_Video, MusicPlayer, Camera, Software,
    };
    QMap<QString, DefaultAppsCategory> m_stringToCategory;
    QMultiMap<QString,QStringList> m_appsList;
    QMultiMap<QString,QString> m_defapp;
    const QString getTypeByCategory(const DefAppWorker::DefaultAppsCategory &category);
    const QStringList getTypeListByCategory(const DefAppWorker::DefaultAppsCategory &category);

public slots:
    void onSetDefaultAppChanged(QString name,QString category);
    void onGetDefaultAppChanged();
    void onGetListAppsChanged();
    void onResetChanged();

private:
    DefAppModel *m_defAppModel;
    Manager     *m_dbusManager;
    Media       *m_dbusMedia;
};

#endif // DEFAPPWORKER_H
