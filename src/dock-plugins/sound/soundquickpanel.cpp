// Copyright (C) 2022 ~ 2022 Deepin Technology Co., Ltd.
// SPDX-FileCopyrightText: 2018 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "soundquickpanel.h"
#include "slidercontainer.h"
#include "soundmodel.h"

#include <DGuiApplicationHelper>

#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDBusPendingCall>
#include <QDBusPendingReply>
#include <QDebug>
#include <QEvent>
#include <QHBoxLayout>
#include <QIcon>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QMetaMethod>
#include <QPainter>
#include <QPixmap>

DGUI_USE_NAMESPACE

#define ICON_SIZE 18
#define BACKSIZE 36

SoundQuickPanel::SoundQuickPanel(QWidget* parent)
    : QWidget(parent)
    , m_sliderContainer(new SliderContainer(this))
{
    initUi();
    initConnection();
}

SoundQuickPanel::~SoundQuickPanel()
{
}

void SoundQuickPanel::initUi()
{
    m_sliderContainer->updateSliderValue(soundVolume());
    m_sliderContainer->setRange(0, SoundModel::ref().maxVolumeUI());
    m_sliderContainer->setPageStep(2);
    SliderProxyStyle* proxy = new SliderProxyStyle;
    m_sliderContainer->setSliderProxyStyle(proxy);
    m_sliderContainer->rightIconWidget()->setStyleType(RightIconButton::StyleType::Background);
    m_sliderContainer->setSliderContentsMargin(QMargins(8, 0, 10, 0));

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(6, 0, 2, 0);
    mainLayout->addWidget(m_sliderContainer);

    refreshWidget();
}

void SoundQuickPanel::initConnection()
{
    connect(&SoundModel::ref(), &SoundModel::muteStateChanged, this, [this] {
        refreshWidget();
        m_sliderContainer->updateSliderValue(soundVolume());
    });
    connect(&SoundModel::ref(), &SoundModel::volumeChanged, this, [this](int value) {
        Q_UNUSED(value)
        m_sliderContainer->updateSliderValue(soundVolume());
        refreshWidget();
    });
    connect(&SoundModel::ref(), &SoundModel::maxVolumeUIChanged, this, [=](int maxValue) {
        m_sliderContainer->setRange(0, maxValue);
        m_sliderContainer->updateSliderValue(soundVolume());
        refreshWidget();
    });

    connect(m_sliderContainer, &SliderContainer::sliderValueChanged, this, [this](int value) {
        SoundController::ref().SetVolume(value * 0.01, true);
    });
    connect(&SoundModel::ref(), &SoundModel::activePortChanged, this, &SoundQuickPanel::refreshWidget);
    connect(&SoundModel::ref(), &SoundModel::cardsInfoChanged, this, &SoundQuickPanel::refreshWidget);
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &SoundQuickPanel::refreshWidget);
    connect(m_sliderContainer, &SliderContainer::iconClicked, this, [this](SliderContainer::IconPosition icon) {
        switch (icon) {
        case SliderContainer::IconPosition::LeftIcon: {
            if (SoundController::ref().existActiveOutputDevice())
                SoundController::ref().SetMute(!SoundModel::ref().isMute());
            break;
        }
        case SliderContainer::IconPosition::RightIcon: {
            // 弹出音量选择对话框
            Q_EMIT rightIconClick();
            break;
        }
        }
    });
    connect(m_sliderContainer, &SliderContainer::panelClicked, this, &SoundQuickPanel::rightIconClick);

    refreshWidget();
}

QString SoundQuickPanel::leftIcon()
{
    const double volume = SoundModel::ref().volume();
    const double maxVolume = SoundModel::ref().maxVolumeUI();
    const bool mute = !SoundController::ref().existActiveOutputDevice() ? true : SoundModel::ref().isMute();

    QString iconString;
    QString volumeString;
    if (mute)
        volumeString = "muted";
    else if (int(volume) == 0)
        volumeString = "off";
    else if (volume / maxVolume > 0.6)
        volumeString = "high";
    else if (volume / maxVolume > 0.3)
        volumeString = "medium";
    else
        volumeString = "low";

    return QString("audio-volume-%1-symbolic").arg(volumeString);
}

QIcon SoundQuickPanel::rightIcon()
{
    auto activePort = SoundModel::ref().activePort();
    if (activePort) {
        return QIcon::fromTheme(SoundCardPort::icon(activePort->portType()));
    }

    return QIcon::fromTheme("sound_other");
}

void SoundQuickPanel::convertThemePixmap(QPixmap& pixmap)
{
    // 图片是黑色的，如果当前主题为白色主题，则无需转换
    if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::ColorType::LightType)
        return;

    // 如果是黑色主题，则转换成白色图像
    QPainter painter(&pixmap);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(pixmap.rect(), Qt::white);
    painter.end();
}

void SoundQuickPanel::refreshWidget()
{
    const bool existValidPort = SoundController::ref().existActiveOutputDevice();
    m_sliderContainer->updateSliderValue(soundVolume());
    m_sliderContainer->setIcon(SliderContainer::IconPosition::LeftIcon, leftIcon(), 10);
    m_sliderContainer->setIcon(SliderContainer::IconPosition::RightIcon, rightIcon(), 12);
    m_sliderContainer->setSliderEnabled(existValidPort);
    m_sliderContainer->setButtonEnabled(SliderContainer::IconPosition::LeftIcon, existValidPort);
}

int SoundQuickPanel::soundVolume() const
{
    return SoundController::ref().existActiveOutputDevice() ? SoundModel::ref().volume() : 0;
}
