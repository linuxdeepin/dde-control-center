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

#ifndef PERSONALIZATIONWORKER_H
#define PERSONALIZATIONWORKER_H

#include "personalizationmodel.h"

#include <QDebug>
#include <QJsonObject>
#include <QMap>
#include <QObject>
#include <QString>
#include <QStringList>

class PersonalizationDBusProxy;
class ThemeModel;

class PersonalizationWorker : public QObject
{
    Q_OBJECT
public:
    PersonalizationWorker(PersonalizationModel *model, QObject *parent = nullptr);
    void active();
    void deactive();
    void onGetList();
    void refreshTheme();
    void refreshFont();

public Q_SLOTS:
    void setDefaultByType(const QString &type, const QString &value);
    void setDefault(const QJsonObject &value);
    void setFontSize(const int value);
    void switchWM();
    void windowSwitchWM(bool value);
    void setOpacity(int opcaity);
    void setMiniEffect(int effect);
    void setActiveColor(const QString &hexColor);
    void setWindowRadius(int radius);

private Q_SLOTS:
    void FontSizeChanged(const double value) const;
    void onGetFontFinished(const QString &category, const QString &json);
    void onGetThemeFinished(const QString &category, const QString &json);
    void onGetPicFinished(const QString &category, const QString &id, const QString &json);
    //    void onGetActiveColorFinished(QDBusPendingCallWatcher *w);
    void onRefreshedChanged(const QString &type);
    void onToggleWM(const QString &wm);
    void setFontList(FontModel *model, const QString &type, const QString &list);
    void onCompositingAllowSwitch(bool value);
    void onWindowWM(bool value);
    void onMiniEffectChanged(bool value);
    void onWindowRadiusChanged(int value);

private:
    int sizeToSliderValue(const double value) const;
    double sliderValueToSize(const int value) const;
    double sliderValutToOpacity(const int value) const;
    QList<QJsonObject> converToList(const QString &type, const QJsonArray &array);
    void addList(ThemeModel *model, const QString &type, const QJsonArray &array);
    void refreshWMState();
    void refreshThemeByType(const QString &type);
    void refreshFontByType(const QString &type);
    void refreshOpacity(double opacity);
    void refreshActiveColor(const QString &color);
    bool allowSwitchWM();

    template<typename T>
    T toSliderValue(std::vector<T> list, T value);

private:
    PersonalizationModel *m_model;
    PersonalizationDBusProxy *m_personalizationDBusProxy;

    QMap<QString, ThemeModel *> m_themeModels;
    QMap<QString, FontModel *> m_fontModels;
    friend class PersonalizationWatcher;
};

class QDBusError;

class PersonalizationWatcher : public QObject
{
    Q_OBJECT
public:
    PersonalizationWatcher(PersonalizationWorker *work);

private Q_SLOTS:
    void onShow(const QString &json);
    void onList(const QString &json);
    void onGetFont(const QString &json);
    void onThumbnail(const QString &json);
    void errorSlot(const QDBusError &err);

private:
    PersonalizationWorker *m_work;
};

#endif // PERSONALIZATIONWORKER_H
