// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef PERSONALIZATIONWORK_H
#define PERSONALIZATIONWORK_H

#include "personalizationmodel.h"
#include <QObject>
#include <QDebug>
#include <QStringList>
#include <QMap>
#include <QString>
#include <QJsonObject>
#include <QGSettings>
#include <com_deepin_daemon_appearance.h>
#include <com_deepin_wmswitcher.h>
#include <com_deepin_wm.h>
#include <org_kde_kwin_effects.h>

using Appearance = com::deepin::daemon::Appearance;
using WMSwitcher = com::deepin::WMSwitcher;
using WM = com::deepin::wm;
using Effects=org::kde::kwin::Effects;

namespace dcc
{
namespace personalization
{
class ThemeModel;
class PersonalizationWork : public QObject
{
    Q_OBJECT
public:
    PersonalizationWork(PersonalizationModel *model, QObject *parent = nullptr);
    void active();
    void deactive();
    void onGetList();
    void refreshTheme();
    void refreshFont();
    void refreshWMState();

public Q_SLOTS:
    void setDefault(const QJsonObject &value);
    void setFontSize(const int value);
    void switchWM();
    void windowSwitchWM(bool value);
    void movedWindowSwitchWM(bool value);
    void setOpacity(int opcaity);
    void setMiniEffect(int effect);
    void setActiveColor(const QString &hexColor);
    void setWindowRadius(int radius);
    void handlePropertiesChanged(QDBusMessage msg);
    void refreshEffectModule();
    void setScrollBarPolicy(int policy);

private Q_SLOTS:
    void FontSizeChanged(const double value) const;
    void onGetFontFinished(QDBusPendingCallWatcher *w);
    void onGetThemeFinished(QDBusPendingCallWatcher *w);
    void onGetPicFinished(QDBusPendingCallWatcher *w);
    void onGetActiveColorFinished(QDBusPendingCallWatcher *w);
    void onRefreshedChanged(const QString &type);
    void onToggleWM(const QString &wm);
    void setMoveWindow(bool state);
    void onGetCurrentWMFinished(QDBusPendingCallWatcher *w);
    void setFontList(FontModel* model, const QString &type, const QString &list);
    void onCompositingAllowSwitch(bool value);
    void onWindowWM(bool value);

private:
    int sizeToSliderValue(const double value) const;
    double sliderValueToSize(const int value) const;
    double sliderValutToOpacity(const int value) const;
    QList<QJsonObject> converToList(const QString &type, const QJsonArray &array);
    void addList(ThemeModel *model, const QString &type, const QJsonArray &array);
    void refreshMoveWindowState();
    void refreshThemeByType(const QString &type);
    void refreshFontByType(const QString &type);
    void refreshOpacity(double opacity);
    void refreshActiveColor(const QString &color);
    bool allowSwitchWM();

    template<typename T>
    T toSliderValue(std::vector<T> list, T value);

private:
    PersonalizationModel *m_model;
    Appearance           *m_dbus;
    WMSwitcher *m_wmSwitcher;
    WM *m_wm;
    Effects *m_effects;
    QMap<QString, ThemeModel*> m_themeModels;
    QMap<QString, FontModel*> m_fontModels;
    QGSettings *m_setting;
    bool m_isWayland;
};
}
}


#endif // PERSONALIZATIONWORK_H
