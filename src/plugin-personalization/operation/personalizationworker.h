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
    virtual void active();
    void deactive();
    void onGetList();
    void refreshTheme();
    void refreshFont();

public Q_SLOTS:
    void switchWM();
    void windowSwitchWM(bool value);
    void movedWindowSwitchWM(bool value);
    void setDiabledCompactToTitleHeight();
    void setScrollBarPolicy(int policy);
    void goDownloadTheme();

    virtual void setDefaultByType(const QString &type, const QString &value);
    virtual void setDefault(const QJsonObject &value);
    virtual void setFontSize(const int value);
    virtual void setOpacity(int opcaity);
    virtual void setWindowEffect(int value);
    virtual void setMiniEffect(int effect);
    virtual void setActiveColor(const QString &hexColor);
    virtual void setWindowRadius(int radius);
    virtual void setCompactDisplay(bool value);
    virtual void setTitleBarHeight(int value);
    virtual void setGlobalTheme(const QString &themeId); // need implementation
    virtual void setAppearanceTheme(const QString &id);
    virtual void setIconTheme(const QString &id);
    virtual void setCursorTheme(const QString &id);
    virtual void setBackgroundForMonitor(const QString &screenName, const QString &url, bool isDark);
    virtual void setLockBackForMonitor(const QString &screenName, const QString &url, bool isDark);

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

protected:
    virtual void onWallpaperUrlsChanged();

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

protected:
    PersonalizationModel *m_model;

private:
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
