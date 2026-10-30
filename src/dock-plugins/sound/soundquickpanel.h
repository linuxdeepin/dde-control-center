// Copyright (C) 2022 ~ 2022 Deepin Technology Co., Ltd.
// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SOUNDWIDGET_H
#define SOUNDWIDGET_H

#include "soundcontroller.h"

#include <DBlurEffectWidget>
#include <QWidget>

class QDBusMessage;
class SliderContainer;
class QLabel;

DWIDGET_USE_NAMESPACE

class SoundQuickPanel : public QWidget
{
    Q_OBJECT

public:
    explicit SoundQuickPanel(QWidget *parent = nullptr);
    ~SoundQuickPanel() override;

Q_SIGNALS:
    void rightIconClick();

protected:
    void initUi();
    void initConnection();

private:
    QString leftIcon();
    QIcon rightIcon();
    void convertThemePixmap(QPixmap &pixmap);
    int soundVolume() const;

private Q_SLOTS:
    void refreshWidget();

private:
    SliderContainer *m_sliderContainer;
};

#endif // VOLUMEWIDGET_H
