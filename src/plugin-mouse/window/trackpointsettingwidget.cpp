//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "trackpointsettingwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/dccslider.h"
#include "widgets/titledslideritem.h"
#include "src/plugin-mouse/operation/mousemodel.h"

#include <QVBoxLayout>

using namespace DCC_NAMESPACE;
TrackPointSettingWidget::TrackPointSettingWidget(QWidget *parent) : QWidget(parent)
{
    m_trackPointSettingsGrp = new SettingsGroup;
    m_trackMoveSlider = new TitledSliderItem(tr("Pointer Speed"));
    QStringList trackPointlist;
    trackPointlist << tr("Slow") << "" << "" << "" << "" << "";
    trackPointlist << tr("Fast");
    DCCSlider *pointSlider = m_trackMoveSlider->slider();
    pointSlider->setType(DCCSlider::Vernier);
    pointSlider->setTickPosition(QSlider::TicksBelow);
    pointSlider->setRange(0, 6);
    pointSlider->setTickInterval(1);
    pointSlider->setPageStep(1);
    m_trackMoveSlider->setAnnotations(trackPointlist);
    m_trackPointSettingsGrp->appendItem(m_trackMoveSlider);

    m_contentLayout = new QVBoxLayout();
    m_contentLayout->setMargin(0);
    m_contentLayout->addWidget(m_trackPointSettingsGrp);
    m_contentLayout->addStretch();

    setLayout(m_contentLayout);
    setContentsMargins(0, 10, 0, 10);
    connect(m_trackMoveSlider->slider(), &DCCSlider::valueChanged, this, &TrackPointSettingWidget::requestSetTrackPointMotionAcceleration);
}

void TrackPointSettingWidget::setModel(MouseModel *const model)
{
    m_mouseModel = model;
    connect(m_mouseModel, &MouseModel::redPointMoveSpeedChanged, this, &TrackPointSettingWidget::onRedPointMoveSpeedChanged);
    onRedPointMoveSpeedChanged(m_mouseModel->redPointMoveSpeed());
}

void TrackPointSettingWidget::onRedPointMoveSpeedChanged(int speed)
{
    m_trackMoveSlider->slider()->blockSignals(true);
    m_trackMoveSlider->slider()->setValue(speed);
    m_trackMoveSlider->slider()->blockSignals(false);
}
