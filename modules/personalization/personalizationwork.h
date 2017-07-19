#ifndef PERSONALIZATIONWORK_H
#define PERSONALIZATIONWORK_H

#include "personalizationmodel.h"
#include <QObject>
#include <QDebug>
#include <QStringList>
#include <QMap>
#include <QString>
#include <QJsonObject>
#include <com_deepin_daemon_apperance.h>
#include <com_deepin_wm_switcher.h>

using com::deepin::daemon::Appearance;
using com::deepin::wm_switcher;

namespace dcc
{
namespace personalization
{
class ThemeModel;
class PersonalizationWork : public QObject
{
    Q_OBJECT
public:
    PersonalizationWork(PersonalizationModel *model, QObject *parent = 0);
    void active();
    void deactive();
    void onGetList();

public slots:
    void setDefault(const QJsonObject &value);
    void setFontSize(const int value);
    void switchWM();

private slots:
    void FontSizeChanged(const double value) const;
    void onStandardFontFinished(QDBusPendingCallWatcher *w);
    void onMonoFontFinished(QDBusPendingCallWatcher *w);
    void onGetThemeFinished(QDBusPendingCallWatcher *w);
    void onGetPicFinished(QDBusPendingCallWatcher *w);
    void onRefreshedChanged(const QString &type);
    void onToggleWM();
    void onGetCurrentWMFinished(QDBusPendingCallWatcher *w);

private:
    int sizeToSliderValue(const double value) const;
    float sliderValueToSize(const int value) const;
    QList<QJsonObject> converToList(const QString &type, QJsonArray &array);
    void addList(ThemeModel *model, const QString &type, QJsonArray &array);

private:
    PersonalizationModel *m_model;
    Appearance           *m_dbus;
    wm_switcher *m_wmSwitcher;
};
}
}


#endif // PERSONALIZATIONWORK_H
