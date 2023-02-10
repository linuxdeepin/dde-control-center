//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef PERSONALIZATIONDBUSPROXY_H
#define PERSONALIZATIONDBUSPROXY_H

#include <QObject>
class QDBusInterface;
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
    Q_PROPERTY(QString WallpaperSlideShow READ wallpaperSlideShow WRITE setWallpaperSlideShow NOTIFY WallpaperSlideShowChanged)
    QString wallpaperSlideShow();
    void setWallpaperSlideShow(const QString &value);
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

public slots:
    // Appearance
    QString List(const QString &ty);
    bool List(const QString &ty, QObject *receiver, const char *member, const char *errorSlot);
    void Set(const QString &ty, const QString &value);
    QString Show(const QString &ty, const QStringList &names);
    bool Show(const QString &ty, const QStringList &names, QObject *receiver, const char *member);
    QString Thumbnail(const QString &ty, const QString &name);
    bool Thumbnail(const QString &ty, const QString &name, QObject *receiver, const char *member, const char *errorSlot);
    // WMSwitcher
    bool AllowSwitch();
    QString CurrentWM();
    bool CurrentWM(QObject *receiver, const char *member);
    void RequestSwitchWM();
    // Effects
    bool loadEffect(const QString &name);
    void unloadEffect(const QString &name);
    bool isEffectLoaded(const QString &name);
    bool isEffectLoaded(const QString &name, QObject *receiver, const char *member);

private slots:
    void onPropertiesChanged(const QDBusMessage &message);

private:
    QDBusInterface *m_AppearanceInter;
    QDBusInterface *m_WMSwitcherInter;
    QDBusInterface *m_WMInter;
    QDBusInterface *m_EffectsInter;
};

#endif // PERSONALIZATIONDBUSPROXY_H
