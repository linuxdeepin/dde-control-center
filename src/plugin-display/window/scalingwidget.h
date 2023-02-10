//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later


#ifndef SCALINGWIDGET_H
#define SCALINGWIDGET_H

#include "interface/namespace.h"
#include "widgets/dccslider.h"
#include "widgets/switchwidget.h"
#include "widgets/titledslideritem.h"
#include "widgets/titlelabel.h"

#include <DTipLabel>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE

class Resolution;

namespace DCC_NAMESPACE {

class Monitor;
class DisplayModel;

class ScalingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ScalingWidget(QWidget *parent = nullptr);
    ~ScalingWidget();

public:
    void setModel(DisplayModel *model);

Q_SIGNALS:
    void requestUiScaleChange(const double scale);

private Q_SLOTS:
    void onResolutionChanged();

private:
    void addSlider();
    QStringList getScaleList(const Resolution &r);
    int convertToSlider(const double value);

private:
    DisplayModel *m_displayModel;
    QVBoxLayout *m_centralLayout;
    TitleLabel *m_title;
    QWidget *m_tipWidget;
    DTK_WIDGET_NAMESPACE::DTipLabel *m_tipLabel;
    TitledSliderItem *m_slider;
    QStringList m_scaleList;
};
}

#endif // SCALINGWIDGET_H
