/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "homescreen.h"
#include "constants.h"

#include <QDebug>
#include <QGridLayout>
#include <QPixmap>
#include <QLabel>
#include <QEvent>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QProcess>
#include <QCoreApplication>
#include <QSettings>

#include <unistd.h>
#include <pwd.h>

#include <dseparatorhorizontal.h>
#include <dimagebutton.h>
#include <dboxwidget.h>

HomeScreen::HomeScreen(QWidget *parent) :
    QFrame(parent)
{
    Q_INIT_RESOURCE(widgets_theme_dark);
    Q_INIT_RESOURCE(widgets_theme_light);

    m_pluginsManager = PluginsManager::getInstance(this);

    struct passwd *pws;
    pws = getpwuid(getuid());

    const QString userName(pws->pw_name);
    m_settings = new QSettings("/var/lib/AccountsService/users/" + userName, QSettings::IniFormat, this);

    m_grid = new QGridLayout;
    m_grid->setContentsMargins(1, 0, 1, 0);
    m_grid->setSpacing(2);

    const QList<ModuleMetaData> pluginsList = m_pluginsManager->pluginsList();
    const int pluginsCount = pluginsList.count();
    for (int i(0); i != pluginsCount; ++i)
        insertPlugin(i, pluginsList.at(i));

    DVBoxWidget *centerBox = new DVBoxWidget;
    centerBox->layout()->addLayout(m_grid);

    m_centerArea = new QScrollArea(this);
    m_centerArea->setFrameStyle(QFrame::NoFrame);
    m_centerArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_centerArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_centerArea->setWidget(centerBox);
    m_centerArea->setStyleSheet("background-color:transparent;");

    QWidget *topOuterWidget = new QWidget(this);
    topOuterWidget->setFixedHeight(DCC::HomeScreen_TopWidgetHeight);
    topOuterWidget->setFixedWidth(DCC::ControlCenterWidth);

    m_userAvatar = new UserAvatar;
    m_userAvatar->setFixedSize(80, 80);
    m_userAvatar->setAvatarSize(UserAvatar::AvatarLargeSize);

    QLabel *topLabel = new QLabel;
    topLabel->setFixedHeight(30);
    topLabel->setAlignment(Qt::AlignCenter);
    topLabel->setObjectName("Username");
    topLabel->setText(userName);

    QVBoxLayout *topWidgetLayout = new QVBoxLayout;
    topWidgetLayout->addWidget(m_userAvatar);
    topWidgetLayout->setAlignment(m_userAvatar, Qt::AlignCenter);
    topWidgetLayout->addWidget(topLabel);
    topWidgetLayout->addStretch();
    topWidgetLayout->setSpacing(20);
    topWidgetLayout->setContentsMargins(0, 20, 0, 15);

    QWidget *topWidget = new QWidget;
    topWidget->setLayout(topWidgetLayout);
    topWidget->setObjectName("TopWidget");

    QVBoxLayout *topVBox = new QVBoxLayout;
    topVBox->addWidget(topWidget);
    topVBox->addWidget(new DSeparatorHorizontal);
    topVBox->setSpacing(0);
    topVBox->setMargin(0);

    m_topWidget = new QWidget(topOuterWidget);
    m_topWidget->setFixedSize(topOuterWidget->size());
    m_topWidget->setLayout(topVBox);

    DImageButton *bottomButton = new DImageButton(DCC::IconPath + "power-button-normal.png",
                                                  DCC::IconPath + "power-button-hover.png",
                                                  DCC::IconPath + "power-button-press.png");
    bottomButton->setAttribute(Qt::WA_TranslucentBackground);

    QLabel *bottomLabel = new QLabel(tr("Shutdown"));
    bottomLabel->setObjectName("PowerLabel");
    bottomLabel->setAlignment(Qt::AlignCenter);

    QVBoxLayout *bottomVLayout = new QVBoxLayout;
    bottomVLayout->addWidget(bottomButton);
    bottomVLayout->setAlignment(bottomButton, Qt::AlignHCenter);
    bottomVLayout->addWidget(bottomLabel);
    bottomVLayout->setSpacing(0);
    bottomVLayout->setContentsMargins(0, 0, 0, 5);

    QWidget *bottomOuterWidget = new QWidget(this);
    bottomOuterWidget->setFixedHeight(DCC::HomeScreen_BottomWidgetHeight);
    bottomOuterWidget->setFixedWidth(DCC::ControlCenterWidth);

    m_bottomWidget = new QWidget(bottomOuterWidget);
    m_bottomWidget->setObjectName("BottomWidget");
    m_bottomWidget->setLayout(bottomVLayout);
    m_bottomWidget->setFixedSize(bottomOuterWidget->size());

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(topOuterWidget);
    mainLayout->addWidget(m_centerArea);
    mainLayout->addSpacing(1);
    mainLayout->addWidget(bottomOuterWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    this->setLayout(mainLayout);

    m_opacityEffect = new QGraphicsOpacityEffect;
    m_opacityEffect->setOpacity(1.0);
    m_centerArea->setGraphicsEffect(m_opacityEffect);

    m_topAni = new QPropertyAnimation(m_topWidget, "geometry");
    m_topAni->setDuration(DCC::FrameAnimationDuration);

    m_ctrHideAni = new QPropertyAnimation(m_opacityEffect, "opacity");
    m_ctrHideAni->setDuration(DCC::FrameAnimationDuration);
    m_ctrHideAni->setEasingCurve(DCC::FrameHideCurve);

    m_ctrShowAni = new QPropertyAnimation(m_opacityEffect, "opacity");
    m_ctrShowAni->setDuration(DCC::FrameAnimationDuration);
    m_ctrShowAni->setEasingCurve(DCC::FrameShowCurve);

    m_botAni = new QPropertyAnimation(m_bottomWidget, "geometry");
    m_botAni->setDuration(DCC::FrameAnimationDuration);

    connect(bottomButton, &DImageButton::clicked, this, &HomeScreen::powerButtonClicked, Qt::DirectConnection);
    connect(m_userAvatar, &UserAvatar::clicked, [this] {emit moduleSelected("account");});
    connect(m_ctrHideAni, &QPropertyAnimation::finished, this, &QFrame::hide);
    connect(m_ctrShowAni, &QPropertyAnimation::finished, this, &HomeScreen::showAniFinished, Qt::QueuedConnection);
    connect(m_pluginsManager, &PluginsManager::pluginInserted, this, &HomeScreen::insertPlugin);
    connect(m_pluginsManager, &PluginsManager::pluginRemoved, this, &HomeScreen::removePlugin);

    loadUserAvatar();
}

HomeScreen::~HomeScreen()
{
    m_opacityEffect->deleteLater();
    m_topAni->deleteLater();
    m_ctrHideAni->deleteLater();
    m_ctrShowAni->deleteLater();
    m_botAni->deleteLater();
}

void HomeScreen::hide()
{
    m_topAni->stop();
    m_ctrHideAni->stop();
    m_ctrShowAni->stop();
    m_botAni->stop();

    m_ctrHideAni->setStartValue(1.0);
    m_ctrHideAni->setEndValue(0.0);

    m_botAni->setStartValue(QRect(0, 0, m_bottomWidget->width(), m_bottomWidget->height()));
    m_botAni->setEndValue(QRect(0, m_bottomWidget->height(), m_bottomWidget->width(), m_bottomWidget->height()));
    m_botAni->setEasingCurve(DCC::FrameHideCurve);

    m_topAni->setStartValue(QRect(0, 0, m_topWidget->width(), m_topWidget->height()));
    m_topAni->setEndValue(QRect(0, -m_topWidget->height(), m_topWidget->width(), m_topWidget->height()));
    m_topAni->setEasingCurve(DCC::FrameHideCurve);

    m_ctrHideAni->start();
    m_topAni->start();
    m_botAni->start();
}

void HomeScreen::show()
{
    m_topAni->stop();
    m_ctrHideAni->stop();
    m_ctrShowAni->stop();
    m_botAni->stop();

    m_ctrShowAni->setStartValue(0.0);
    m_ctrShowAni->setEndValue(1.0);

    m_botAni->setEndValue(QRect(0, 0, m_bottomWidget->width(), m_bottomWidget->height()));
    m_botAni->setStartValue(QRect(0, m_bottomWidget->height(), m_bottomWidget->width(), m_bottomWidget->height()));
    m_botAni->setEasingCurve(DCC::FrameShowCurve);

    m_topAni->setEndValue(QRect(0, 0, m_topWidget->width(), m_topWidget->height()));
    m_topAni->setStartValue(QRect(0, -m_topWidget->height(), m_topWidget->width(), m_topWidget->height()));
    m_topAni->setEasingCurve(DCC::FrameShowCurve);

    m_ctrShowAni->start();
    m_botAni->start();
    m_topAni->start();

    // reload user avatar
    loadUserAvatar();

    QFrame::show();
}

void HomeScreen::powerButtonClicked()
{
    emit powerBtnClicked();

    QProcess *proc = new QProcess;
    QObject::connect(proc, static_cast<void (QProcess::*)(int)>(&QProcess::finished), proc, &QProcess::deleteLater, Qt::QueuedConnection);
    proc->start("dde-shutdown");
}

void HomeScreen::loadUserAvatar()
{
    // clear QSettings cache,
    m_settings->sync();

    const QString &file = m_settings->value("User/Icon").toString();

    qDebug() << "avatar = " << file;
    for (const QString &k : m_settings->allKeys())
        qDebug() << k << " = " << m_settings->value(k).toString();

    if (!file.isEmpty())
        m_userAvatar->setIcon(file);
    else
        // use default icon
        m_userAvatar->setIcon("/var/lib/AccountsService/icons/default.png");
}

void HomeScreen::insertPlugin(const int position, const ModuleMetaData &meta)
{
    ModuleButton *button = new ModuleButton(meta, this);

    QList<QWidget *> plugins;
    QLayoutItem *plugin;
    while ((plugin = m_grid->takeAt(0)) != nullptr)
        plugins.append(plugin->widget());

    plugins.insert(position, button);

    const int count = plugins.count();
    for (int i(0); i != count; ++i)
        m_grid->addWidget(plugins.at(i), i / 3, i % 3);

    connect(button, &ModuleButton::clicked, this, &HomeScreen::moduleSelected);
}

void HomeScreen::removePlugin(const ModuleMetaData &meta)
{
    for (int r(0); r != m_grid->count(); ++r)
    {
        QLayoutItem *item = m_grid->itemAt(r);
        ModuleButton *btn = qobject_cast<ModuleButton *>(item->widget());
        if (!btn)
            continue;

        if (btn->pluginId() != meta.id)
            continue;

        m_grid->removeWidget(btn);
        delete btn;
        break;
    }

    relayoutPlugins();
}

void HomeScreen::relayoutPlugins()
{
    QList<QLayoutItem *> plugins;
    QLayoutItem *plugin;
    while ((plugin = m_grid->takeAt(0)) != nullptr)
        plugins.append(plugin);

    const int count = plugins.count();
    for (int i(0); i != count; ++i)
        m_grid->addItem(plugins.at(i), i / 3, i % 3);
}

// class ModuleButton
ModuleButton::ModuleButton(const ModuleMetaData &metaData, QWidget *parent) :
    QFrame(parent),
    m_pluginId(metaData.id)
{
    setFixedSize(118, 110);
    setMouseTracking(true);

    // text font
    QFont font(QFont().family());
    font.setPixelSize(12);

    m_icon = new QLabel(this);
    m_text = new QLabel(this);
    m_text->setFont(font);
    m_text->setText(QCoreApplication::translate("ModuleName", metaData.name.toLatin1()));
    m_text->setWordWrap(true);
    m_text->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    m_text->setFixedWidth(90);

    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addWidget(m_icon);
    vLayout->setAlignment(m_icon, Qt::AlignHCenter);
    vLayout->addWidget(m_text);
    vLayout->setAlignment(m_text, Qt::AlignHCenter);
    vLayout->setContentsMargins(0, 24, 0, 0);
    vLayout->setSpacing(6);

    this->setLayout(vLayout);

    setState(Normal);
}

void ModuleButton::enterEvent(QEvent *)
{
    setState(Hover);
}

void ModuleButton::leaveEvent(QEvent *)
{
    setState(Normal);
}

void ModuleButton::mousePressEvent(QMouseEvent *)
{
    emit clicked(m_pluginId);
}

void ModuleButton::setState(State state)
{
#ifndef QT_DEBUG
    QString moduleIconsDir("/usr/share/dde-control-center/modules/icons/32/%1_%2.svg");
#else
    QString moduleIconsDir("modules/icons/32/%1_%2.svg");
#endif

    switch (state) {
    case Normal:
        this->setStyleSheet("QFrame { background-color: transparent; border-radius: 3 }");
        m_icon->setPixmap(QPixmap(moduleIconsDir.arg(m_pluginId).arg("normal")));
        m_text->setStyleSheet(QString("QLabel { color: %1 }").arg(DCC::TextNormalColor.name()));
        break;
    case Hover:
        this->setStyleSheet(QString("QFrame { background-color: %1; border-radius: 3 }").arg(DCC::BgDarkColor.name()));
        m_icon->setPixmap(QPixmap(moduleIconsDir.arg(m_pluginId).arg("hover")));
        m_text->setStyleSheet(QString("QLabel { color: %1 }").arg(DCC::TextHoverColor.name()));
        break;
    default:
        break;
    }
}
