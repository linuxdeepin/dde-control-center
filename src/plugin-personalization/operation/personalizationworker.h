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
    void setDiabledCompactToTitleHeight();
    void setScrollBarPolicy(int policy);
    void setCompactDisplay(bool value);
    void goDownloadTheme();

    // 设置给Appearance分别在深色和浅色下的活动色
    void setActiveColors(const QString &activeColors);

    virtual void setDefaultByType(const QString &type, const QString &value);
    virtual void setDefault(const QJsonObject &value);
    virtual void setFontSize(const int pixelSize);
    virtual void setOpacity(int opcaity);
    virtual void setWindowEffect(int value);
    virtual void setMiniEffect(int effect);
    virtual void setMovedWindowOpacity(bool value);
    virtual void setActiveColor(const QString &hexColor);
    virtual void setWindowRadius(int radius);
    virtual void setTitleBarHeight(int value);
    virtual void setGlobalTheme(const QString &themeId);
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
    void onRefreshedChanged(const QString &type);
    void setFontList(FontModel *model, const QString &type, const QString &list);
    void onWindowRadiusChanged(int value);
    void onCompactDisplayChanged(int value);
    void onWindowEffectChanged(int value);
    void onScreensChanged();

protected:
    virtual void onWallpaperUrlsChanged();

private:
    double sliderValutToOpacity(const int value) const;
    QList<QJsonObject> converToList(const QString &type, const QJsonArray &array);
    void addList(ThemeModel *model, const QString &type, const QJsonArray &array);
    void refreshThemeByType(const QString &type);
    void refreshFontByType(const QString &type);
    void refreshOpacity(double opacity);
    void refreshActiveColor(const QString &color);
    void onPersonalizationConfigChanged(const QString &key);
    void onDTKConfigChanged(const QString &key);

    template<typename T>
    T toSliderValue(std::vector<T> list, T value);

protected:
    PersonalizationModel *m_model;
    PersonalizationDBusProxy *m_personalizationDBusProxy;

private:
    WallpaperWorker *m_wallpaperWorker;
    Dtk::Core::DConfig *m_personalizationConfig;
    Dtk::Core::DConfig *m_dtkConfig;

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
