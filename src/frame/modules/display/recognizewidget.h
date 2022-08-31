// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef RECOGNIZEWIDGET_H
#define RECOGNIZEWIDGET_H

#include <DBlurEffectWidget>

DWIDGET_USE_NAMESPACE

namespace dcc {

namespace display {

class Monitor;
class DisplayModel;
class RecognizeWidget : public DBlurEffectWidget
{
    Q_OBJECT

public:
    explicit RecognizeWidget(Monitor *monitor, QString name, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *);
    void timerEvent(QTimerEvent *event) override;

private Q_SLOTS:
    void onScreenRectChanged();

private:
    Monitor *m_monitor;
    QRect m_rect;
    QString m_text;
};

} // namespace display

} // namespace dcc

#endif // RECOGNIZEWIDGET_H
