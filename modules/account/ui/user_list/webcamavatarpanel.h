/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef WEBCAMAVATARPANEL_H
#define WEBCAMAVATARPANEL_H

#include <QWidget>
#include <QCamera>
#include <QCameraInfo>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include "generaladdbutton.h"
#include "generalremovebutton.h"
#include "dslider.h"
#include "dcameraview.h"
#include "dtextbutton.h"
#include "dconstants.h"

DWIDGET_USE_NAMESPACE

class WebcamAvatarPanel : public QWidget
{
    Q_OBJECT
public:
    explicit WebcamAvatarPanel(QWidget *parent = 0);
    void turnOnCamera();
    void turnOffCamera();

signals:
    void selectedAvatar(const QString &path);

private slots:
    void onCameraError(QCamera::Error);

private:
    void initCameraLayout();
    void initCameraView();
    void initScaleSlider();
    void initControlButton();
    void initStackWidget();

    void onSnapshot();
    void onConfirm();
    void onSliderValueChanged(int value);
private:
    QStackedWidget *m_stackWidget = NULL;
    QVBoxLayout *m_layout = NULL;
    QCamera *m_camera = NULL;
    DCameraView *m_cameraView = NULL;
    DTextButton *m_snapshotButton;
    bool m_captureState = false;
};

#endif // WEBCAMAVATARPANEL_H
