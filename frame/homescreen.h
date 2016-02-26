/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef HOMESCREEN_H
#define HOMESCREEN_H

#include "modulemetadata.h"
#include "pluginsmanager.h"
#include "useravatar.h"

#include <QFrame>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QSettings>
#include <QScrollArea>

class QLabel;
class QGridLayout;
class HomeScreen : public QFrame
{
    Q_OBJECT
public:
    explicit HomeScreen(QWidget *parent = 0);
    ~HomeScreen();

    void hide();
    void show();

signals:
    void moduleSelected(const QString pluginId) const;
    void showAniFinished();
    void powerBtnClicked();

public slots:
    void powerButtonClicked();

private slots:
    void loadUserAvatar();
    void insertPlugin(const int position, const ModuleMetaData &meta);
    void removePlugin(const ModuleMetaData &meta);
    void relayoutPlugins();

private:
    UserAvatar *m_userAvatar;
    QGridLayout *m_grid;

    QGraphicsOpacityEffect *m_opacityEffect;
    QPropertyAnimation *m_topAni;
    QPropertyAnimation *m_ctrShowAni;
    QPropertyAnimation *m_ctrHideAni;
    QPropertyAnimation *m_botAni;

    QWidget *m_topWidget;
    QScrollArea *m_centerArea;
    QWidget *m_bottomWidget;

    PluginsManager *m_pluginsManager;
    QSettings *m_settings;
};

class QEvent;
class QMouseEvent;
class ModuleButton : public QFrame
{
    Q_OBJECT
public:
    ModuleButton(const ModuleMetaData &metaData, QWidget *parent = 0);

    const QString pluginId() const {return m_pluginId;}
    void enterEvent(QEvent *event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

signals:
    void clicked(const QString pluginId) const;

private:
    enum State { Normal, Hover };

private:
    void setState(State state);

private:
    QString m_pluginId;

    QLabel *m_icon;
    QLabel *m_text;
};

#endif // HOMESCREEN_H
