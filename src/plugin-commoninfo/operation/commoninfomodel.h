//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "grubanimationmodel.h"
#include "grubmenulistmodel.h"

#include <QtQml/qqml.h>
#include <QObject>
#include <QPixmap>

class CommonInfoModel : public QObject
{
    Q_OBJECT

    QML_NAMED_ELEMENT(CommonInfoModel)
    QML_SINGLETON

    Q_PROPERTY(bool bootDelay READ bootDelay NOTIFY bootDelayChanged FINAL)
    Q_PROPERTY(bool themeEnabled READ themeEnabled NOTIFY themeEnabledChanged FINAL)
    Q_PROPERTY(bool grubEditAuthEnabled READ grubEditAuthEnabled NOTIFY grubEditAuthEnabledChanged FINAL)
    Q_PROPERTY(int debugLogCurrentIndex READ debugLogCurrentIndex NOTIFY debugLogCurrentIndexChanged FINAL)
    Q_PROPERTY(bool developerModeState READ developerModeState NOTIFY developerModeStateChanged FINAL)
    Q_PROPERTY(bool isLogin READ isLogin NOTIFY isLoginChenged FINAL)
    Q_PROPERTY(bool isActivate READ isActivate NOTIFY LicenseStateChanged FINAL)
    Q_PROPERTY(QString grubThemePath READ grubThemePath NOTIFY grubThemePathChanged FINAL)
    Q_PROPERTY(bool needShowModalDialog READ needShowModalDialog NOTIFY needShowModalDialogChanged FINAL)
    Q_PROPERTY(bool isDeveloperMode READ isDeveloperMode NOTIFY isDeveloperModeChanged FINAL)
    Q_PROPERTY(bool readOnlyProtectionEnabled READ readOnlyProtectionEnabled NOTIFY readOnlyProtectionEnabledChanged FINAL)


public:
    explicit CommonInfoModel(QObject *parent = nullptr);

    void setEntryLists(const QStringList &list);
    inline QStringList entryLists() const { return m_entryLists;}
    inline QString defaultEntry() const { return m_defaultEntry;}
    bool bootDelay() const;
    bool themeEnabled() const { return m_themeEnabled; }
    inline bool isShowGrubEditAuth() { return m_isShowGrubEditAuth; }
    bool grubEditAuthEnabled() { return m_grubEditAuthEnabled; }
    inline bool updating() const { return m_updating; }
    QPixmap background() const;
    void setBackground(const QPixmap &background);
    bool ueProgram() const; // for user experience program
    bool developerModeState() const;
    inline bool isLogin() const { return m_isLogin; }
    inline bool isActivate() const { return m_activation; }
    void setActivation(bool value);
    inline int plymouthScale() const { return m_plymouthscale; }
    inline QString plymouthTheme() const { return m_plymouththeme; }

    Q_INVOKABLE GrubAnimationModel *grubAnimationModel();
    Q_INVOKABLE GrubMenuListModel *grubMenuListModel();
    Q_INVOKABLE bool isCommunitySystem() const;

    int debugLogCurrentIndex() const;
    void setDebugLogCurrentIndex(int newDebugLogCurrentIndex);

    QString grubThemePath() const;
    void setGrubThemePath(const QString &newGrubThemePath);

    bool needShowModalDialog() const;
    void setNeedShowModalDialog(bool newNeedShowModalDialog);


    bool isDeveloperMode() const;
    bool readOnlyProtectionEnabled() const;

Q_SIGNALS:
    void bootDelayChanged(const bool enabled) const;
    void themeEnabledChanged(const bool enabled) const;
    void grubEditAuthEnabledChanged(const bool enabled) const;
    void entryListsChanged(const QStringList &list);
    void defaultEntryChanged(const QString &entry);
    void updatingChanged(const bool &updating);
    void backgroundChanged(const QPixmap &pixmap);
    void ueProgramChanged(const bool enable) const; // for user experience program
    void developerModeStateChanged(const bool enable) const;
    void isLoginChenged(bool log) const;
    void LicenseStateChanged(bool state);
    void plymouthScaleChanged(int scale);
    void plymouthThemeChanged(const QString &themeName);

    void GrubAnimationModelChanged();

    void debugLogCurrentIndexChanged();

    void grubThemePathChanged();

    void needShowModalDialogChanged();

    void isDeveloperModeChanged();
    void readOnlyProtectionEnabledChanged();

public Q_SLOTS:
    void setBootDelay(bool bootDelay);
    void setThemeEnabled(const bool enabled);
    void setShowGrubEditAuth(const bool enabled);
    void setGrubEditAuthEnabled(const bool enabled);
    void setDefaultEntry(const QString &entry);
    void setUpdating(bool updating);
    void setUeProgram(const bool ueProgram); // for user experience program
    void setDeveloperModeState(const bool state);
    void setIsLogin(const bool log);
    void setPlymouthScale(int scale);
    void setPlymouthTheme(const QString &themeName);
    void setIsDeveloperMode(bool newIsDeveloperMode);
    void setReadOnlyProtectionEnabled(bool enabled);


private:
    bool m_bootDelay;
    bool m_themeEnabled;
    bool m_isShowGrubEditAuth = false;
    bool m_grubEditAuthEnabled = false;
    bool m_updating;
    QStringList m_entryLists;
    QString m_defaultEntry;
    QPixmap m_background;
    bool m_joinUeProgram;   // for user experience program
    bool m_developerModeState{false}; // for developer mode state
    bool m_isLogin{false};
    bool m_activation;
    int m_plymouthscale;
    QString m_plymouththeme;
    QString m_grubThemePath;

    GrubAnimationModel* m_GrubAnimationModel;
    GrubMenuListModel* m_GrubMenuListModel;

    int m_debugLogCurrentIndex;
    bool m_needShowModalDialog;

    bool m_isDeveloperMode;
    bool m_readOnlyProtectionEnabled{false};

};
