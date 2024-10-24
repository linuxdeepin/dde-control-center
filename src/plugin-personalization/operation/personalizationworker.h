//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef PERSONALIZATIONWORKER_H
#define PERSONALIZATIONWORKER_H

#include "operation/wallpaperworker.h"
#include "personalizationmodel.h"

#include <QObject>
#include <QDebug>
#include <QStringList>
#include <QMap>
#include <QString>
#include <QJsonObject>

#include <DConfig>

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
    void movedWindowSwitchWM(bool value);
    void setOpacity(int opcaity);
    void setWindowEffect(int value);
    void setMiniEffect(int effect);
    void setActiveColor(const QString &hexColor);
    void setWindowRadius(int radius);
    void setCompactDisplay(bool value);
    void setScrollBarPolicy(int policy);
    void setTitleBarHeight(int value);
    void setDiabledCompactToTitleHeight();
    void setGlobalTheme(const QString &themeId);
    void setAppearanceTheme(const QString &id);
    void setIconTheme(const QString &id);
    void setCursorTheme(const QString &id);

    // wallpaper
    void setBackgroundForMonitor(const QString &screenName, const QString &url);
    QString getBackgroundForMonitor(const QString &screenName);

signals:
    void personalizationChanged(const QString &propertyName, const QString &value);

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
    void onCompactDisplayChanged(int value);
    void onWindowEffectChanged(int value);
    void onWallpaperUrlsChanged(const QString &value);

private:
    double sliderValutToOpacity(const int value) const;
    QList<QJsonObject> converToList(const QString &type, const QJsonArray &array);
    void addList(ThemeModel *model, const QString &type, const QJsonArray &array);
    void refreshWMState();
    void refreshThemeByType(const QString &type);
    void refreshFontByType(const QString &type);
    void refreshOpacity(double opacity);
    void refreshActiveColor(const QString &color);
    bool allowSwitchWM();
    void onKWinTitleBarConfigChanged(const QString &key);
    void onKWinCompositingConfigChanged(const QString &key);
    void onPersonalizationConfigChanged(const QString &key);

    template<typename T>
    T toSliderValue(std::vector<T> list, T value);

private:
    PersonalizationModel *m_model;
    PersonalizationDBusProxy *m_personalizationDBusProxy;
    WallpaperWorker *m_wallpaperWorker;
    Dtk::Core::DConfig *m_kwinTitleBarConfig;
    Dtk::Core::DConfig *m_kwinCompositingConfig;
    Dtk::Core::DConfig *m_personalizationConfig;

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
