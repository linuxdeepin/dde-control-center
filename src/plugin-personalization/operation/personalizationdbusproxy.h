//SPDX-FileCopyrightText: 2018 - 2026 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef PERSONALIZATIONDBUSPROXY_H
#define PERSONALIZATIONDBUSPROXY_H

#include <QObject>
#include <DDBusInterface>
#include <QDBusPendingReply>

class QDBusMessage;

class PersonalizationDBusProxy : public QObject
{
    Q_OBJECT
public:
    explicit PersonalizationDBusProxy(QObject *parent = nullptr);

    // Appearance
    Q_PROPERTY(QString Background READ background WRITE setBackground NOTIFY BackgroundChanged)
    QString background();
    void setBackground(const QString &value);
    Q_PROPERTY(QString CursorTheme READ cursorTheme WRITE setCursorTheme NOTIFY CursorThemeChanged)
    QString cursorTheme();
    void setCursorTheme(const QString &value);
    Q_PROPERTY(QString GlobalTheme READ globalTheme WRITE setGlobalTheme NOTIFY GlobalThemeChanged)
    QString globalTheme();
    void setGlobalTheme(const QString &value);
    Q_PROPERTY(double FontSize READ fontSize WRITE setFontSize NOTIFY FontSizeChanged)
    double fontSize();
    void setFontSize(double value);
    Q_PROPERTY(QString GtkTheme READ gtkTheme WRITE setGtkTheme NOTIFY GtkThemeChanged)
    QString gtkTheme();
    void setGtkTheme(const QString &value);
    Q_PROPERTY(QString IconTheme READ iconTheme WRITE setIconTheme NOTIFY IconThemeChanged)
    QString iconTheme();
    void setIconTheme(const QString &value);
    Q_PROPERTY(QString MonospaceFont READ monospaceFont WRITE setMonospaceFont NOTIFY MonospaceFontChanged)
    QString monospaceFont();
    void setMonospaceFont(const QString &value);
    Q_PROPERTY(double Opacity READ opacity WRITE setOpacity NOTIFY OpacityChanged)
    double opacity();
    void setOpacity(double value);
    Q_PROPERTY(QString QtActiveColor READ qtActiveColor WRITE setQtActiveColor NOTIFY QtActiveColorChanged)
    QString qtActiveColor();
    void setQtActiveColor(const QString &value);
    Q_PROPERTY(QString StandardFont READ standardFont WRITE setStandardFont NOTIFY StandardFontChanged)
    QString standardFont();
    void setStandardFont(const QString &value);

    // wallpaperSlideshow
    Q_PROPERTY(QString WallpaperSlideShow WRITE setWallpaperSlideShow READ wallpaperSlideShow NOTIFY WallpaperSlideShowChanged)
    QString wallpaperSlideShow();
    QString wallpaperSlideShow(const QString &monitorName);
    void setWallpaperSlideShow(const QString &monitorName, const QString &sliderShow);
    void setWallpaperSlideShow(const QString &wallpaperSlideShow);

    Q_PROPERTY(int WindowRadius READ windowRadius WRITE setWindowRadius NOTIFY WindowRadiusChanged)
    int windowRadius();
    void setWindowRadius(int value);
    // SystemPersonalization
    // WM
    Q_PROPERTY(bool compositingAllowSwitch READ compositingAllowSwitch NOTIFY compositingAllowSwitchChanged)
    bool compositingAllowSwitch();
    Q_PROPERTY(bool compositingEnabled READ compositingEnabled WRITE setCompositingEnabled NOTIFY compositingEnabledChanged)
    bool compositingEnabled();
    void setCompositingEnabled(bool value);
    Q_PROPERTY(bool compositingPossible READ compositingPossible NOTIFY compositingPossibleChanged)
    bool compositingPossible();
    Q_PROPERTY(int cursorSize READ cursorSize WRITE setCursorSize NOTIFY cursorSizeChanged)
    int cursorSize();
    void setCursorSize(int value);
    //    Q_PROPERTY(QString cursorTheme READ cursorTheme WRITE setCursorTheme NOTIFY CursorThemeChanged)
    //    QString cursorTheme();
    //    void setCursorTheme(const QString &value);
    Q_PROPERTY(bool zoneEnabled READ zoneEnabled WRITE setZoneEnabled NOTIFY ZoneEnabledChanged)
    bool zoneEnabled();
    void setZoneEnabled(bool value);

    Q_PROPERTY(int DTKSizeMode READ getDTKSizeMode WRITE setDTKSizeMode NOTIFY DTKSizeModeChanged)
    int getDTKSizeMode();
    void setDTKSizeMode(int value);

    Q_PROPERTY(int scrollBarPolicy READ getScrollBarPolicy WRITE setScrollBarPolicy NOTIFY scrollBarPolicyChanged)
    int getScrollBarPolicy();
    void setScrollBarPolicy(int value);

    void SetCurrentWorkspaceBackgroundForMonitor(const QString &url, const QString &screenName);
    QString getCurrentWorkSpaceBackgroundForMonitor(const QString &screenName);

    void SetGreeterBackground(const QString &url);
    
    // daemon
    QString saveCustomWallpaper(const QString &userName, const QString &url);
    void deleteCustomWallpaper(const QString &userName, const QString &url);
    QStringList getCustomWallpaper(const QString &userName);

    // screenSaver
    QStringList getAllscreensaver();

    void setCurrentScreenSaver(const QString &value);
    QString getCurrentScreenSaver();

    void setLockScreenAtAwake(bool value);
    bool getLockScreenAtAwake();

    void preview(const QString &name, bool stayOn = true);

    void setLinePowerScreenSaverTimeout(int value);
    void setBatteryScreenSaverTimeout(int value);

    int getLinePowerScreenSaverTimeout();
    int getBatteryScreenSaverTimeout();

    void requestScreenSaverConfig(const QString& name);

    QStringList ConfigurableItems();

    QString GetScreenSaverCover(const QString &name);

    // power
    bool OnBattery();

signals:
    // Appearance
    void Changed(const QString &in0, const QString &in1);
    void Refreshed(const QString &in0);
    // begin property changed signals
    void BackgroundChanged(const QString &value) const;
    void CursorThemeChanged(const QString &value) const;
    void FontSizeChanged(double value) const;
    void GtkThemeChanged(const QString &value) const;
    void IconThemeChanged(const QString &value) const;
    void GlobalThemeChanged(const QString &value) const;
    void MonospaceFontChanged(const QString &value) const;
    void OpacityChanged(double value) const;
    void QtActiveColorChanged(const QString &value) const;
    void StandardFontChanged(const QString &value) const;
    void WallpaperSlideShowChanged(const QString &value) const;
    void WindowRadiusChanged(int value) const;
    // WMSwitcher
    void WMChanged(const QString &wm);
    // WM
    // begin property changed signals
    void compositingAllowSwitchChanged(bool value) const;
    void compositingEnabledChanged(bool value) const;
    void compositingPossibleChanged(bool value) const;
    void cursorSizeChanged(int value) const;
    //    void CursorThemeChanged(const QString & value) const;
    void ZoneEnabledChanged(bool value) const;
    void DTKSizeModeChanged(int value) const;
    void scrollBarPolicyChanged(int value) const;
    void WallpaperURlsChanged(QString) const;
    // screenSaver
    void allscreensaverChanged(const QStringList &value);
    void currentScreenSaverChanged(const QString &value);
    void lockScreenAtAwakeChanged(bool value);
    void linePowerScreenSaverTimeoutChanged(int value);
    void batteryScreenSaverTimeoutChanged(int value);
    // daemon
    void WallpaperChanged(const QString &value, uint mode, const QStringList &urls);

    // power
    void OnBatteryChanged(bool value);

public slots:
    // Appearance
    QString List(const QString &ty);
    bool List(const QString &ty, QObject *receiver, const char *member, const char *errorSlot);
    void Set(const QString &ty, const QString &value);
    QString Show(const QString &ty, const QStringList &names);
    bool Show(const QString &ty, const QStringList &names, QObject *receiver, const char *member);
    QString Thumbnail(const QString &ty, const QString &name);
    bool Thumbnail(const QString &ty, const QString &name, QObject *receiver, const char *member, const char *errorSlot);
    QString activeColors();
    void setActiveColors(const QString &activeColors);
    // Effects
    bool loadEffect(const QString &name);
    void unloadEffect(const QString &name);
    bool isEffectLoaded(const QString &name);
    bool isEffectLoaded(const QString &name, QObject *receiver, const char *member);
    bool isEffectSupported(const QString &name);

private:
    Dtk::Core::DDBusInterface *m_AppearanceInter = nullptr;
    Dtk::Core::DDBusInterface *m_WMInter = nullptr;
    Dtk::Core::DDBusInterface *m_EffectsInter = nullptr;
    Dtk::Core::DDBusInterface *m_DaemonInter = nullptr;
    Dtk::Core::DDBusInterface *m_screenSaverInter = nullptr;
    Dtk::Core::DDBusInterface *m_wallpaperSlideshowInter = nullptr;
    Dtk::Core::DDBusInterface *m_powerInter = nullptr;
};

#endif // PERSONALIZATIONDBUSPROXY_H
