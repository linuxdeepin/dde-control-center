/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PERSONALIZATIONWORK_H
#define PERSONALIZATIONWORK_H

#include "personalizationmodel.h"
#include <QObject>
#include <QDebug>
#include <QStringList>
#include <QMap>
#include <QString>
#include <QJsonObject>
#include <com_deepin_daemon_appearance.h>
#include <com_deepin_wmswitcher.h>

using Appearance = com::deepin::daemon::Appearance;
using WMSwitcher = com::deepin::WMSwitcher;

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
    void refreshTheme();
    void refreshFont();

public Q_SLOTS:
    void setDefault(const QJsonObject &value);
    void setFontSize(const int value);
    void switchWM();
    void setOpacity(int opcaity);

private Q_SLOTS:
    void FontSizeChanged(const double value) const;
    void onGetFontFinished(QDBusPendingCallWatcher *w);
    void onGetThemeFinished(QDBusPendingCallWatcher *w);
    void onGetPicFinished(QDBusPendingCallWatcher *w);
    void onRefreshedChanged(const QString &type);
    void onToggleWM(const QString &wm);
    void onGetCurrentWMFinished(QDBusPendingCallWatcher *w);
    void setFontList(FontModel* model, const QString &type, const QString &list);

private:
    int sizeToSliderValue(const double value) const;
    float sliderValueToSize(const int value) const;
    double sliderValutToOpacity(const int value) const;
    QList<QJsonObject> converToList(const QString &type, const QJsonArray &array);
    void addList(ThemeModel *model, const QString &type, const QJsonArray &array);
    void refreshWMState();
    void refreshThemeByType(const QString &type);
    void refreshFontByType(const QString &type);
    void refreshOpacity(double opacity);

    template<typename T>
    T toSliderValue(std::vector<T> list, T value);

private:
    PersonalizationModel *m_model;
    Appearance           *m_dbus;
    WMSwitcher *m_wmSwitcher;
    QMap<QString, ThemeModel*> m_themeModels;
    QMap<QString, FontModel*> m_fontModels;
};
}
}


#endif // PERSONALIZATIONWORK_H
