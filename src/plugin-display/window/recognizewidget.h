//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later


#ifndef RECOGNIZEWIDGET_H
#define RECOGNIZEWIDGET_H

#include "interface/namespace.h"

#include <DBlurEffectWidget>

class QPainter;
class QPainterPath;

DWIDGET_USE_NAMESPACE

namespace DCC_NAMESPACE {

class Monitor;
class DisplayModel;
class RecognizeWidget : public DBlurEffectWidget
{
    Q_OBJECT

public:
    explicit RecognizeWidget(Monitor *monitor, QString name, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *) override;
    void timerEvent(QTimerEvent *event) override;

private Q_SLOTS:
    void onScreenRectChanged();

private:
    Monitor *m_monitor;
    QRect m_rect;
    QString m_text;
};
}

#endif // RECOGNIZEWIDGET_H
