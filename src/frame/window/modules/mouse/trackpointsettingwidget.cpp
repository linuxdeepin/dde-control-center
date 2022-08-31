// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "trackpointsettingwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/dccslider.h"
#include "widgets/titledslideritem.h"
#include "modules/mouse/mousemodel.h"

#include <QVBoxLayout>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::mouse;
using namespace dcc::mouse;
using namespace dcc::widgets;

TrackPointSettingWidget::TrackPointSettingWidget(QWidget *parent) : dcc::ContentWidget(parent)
{
    m_trackPointSettingsGrp = new SettingsGroup;
    //~ contents_path /mouse/TrackPoint
    //~ child_page TrackPoint
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

    TranslucentFrame *tFrame = new TranslucentFrame;
    tFrame->setLayout(m_contentLayout);
    layout()->setMargin(0);
    setContent(tFrame);
    connect(m_trackMoveSlider->slider(), &DCCSlider::valueChanged, this, &TrackPointSettingWidget::requestSetTrackPointMotionAcceleration);
}

void TrackPointSettingWidget::setModel(dcc::mouse::MouseModel *const model)
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
