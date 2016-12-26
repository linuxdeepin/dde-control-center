#ifndef DEFAPPWORKER_H
#define DEFAPPWORKER_H

#include <com_deepin_api_manager.h>
#include <com_deepin_api_media.h>
#include <QObject>
using com::deepin::api::Manager;
using com::deepin::api::Media;
namespace dcc
{
namespace defapp
{
class DefAppModel;
class Category;
class DefAppWorker : public QObject
{
    Q_OBJECT
public:
    explicit DefAppWorker(DefAppModel *m_defAppModel, QObject *parent = 0);
    void active();
    void deactive();

public slots:
    void onSetDefaultApp(const QString &category, const QJsonObject &item);
    void onGetDefaultApp();
    void onGetListApps();
    void onResetTriggered();
    void onAutoOpenChanged(const bool state);
    void onAddUserApp(const QString &category, const QString &item);
    void onDelUserApp(const QString &mine, const QJsonObject &item);

private slots:
    void getListAppFinished(QDBusPendingCallWatcher *w);
    void getUserAppFinished(QDBusPendingCallWatcher *w);
    void getDefaultAppFinished(QDBusPendingCallWatcher *w);
    void saveListApp(const QString &mime, const QJsonArray &json);
    void saveUserApp(const QString &mime, const QJsonArray &json);
    void saveDefaultApp(const QString &mime, const QString &app);
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
    Category* getCategory(const QString &mime) const;
};
}
}

#endif // DEFAPPWORKER_H
