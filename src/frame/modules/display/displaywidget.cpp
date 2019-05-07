/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "displaywidget.h"
#include "widgets/basiclistdelegate.h"
#include "widgets/dccslider.h"
#include "displaycontrolpage.h"
#include "displaymodel.h"
#include "displayworker.h"
#include "widgets/editablenextpagewidget.h"
#include "widgets/nextpagewidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/titledslideritem.h"
#include "widgets/translucentframe.h"
#include <cmath>
#include <functional>

using namespace dcc::widgets;
using namespace dcc::display;

DisplayWidget::DisplayWidget()
    : ModuleWidget()
    , m_displayControlPage(new DisplayControlPage)
    , m_resolution(new NextPageWidget)
    , m_brightnessSettings(new NextPageWidget)
    , m_scalingSettings(new NextPageWidget)
    , m_scaleWidget(new TitledSliderItem(tr("Display Scaling")))
#ifndef DCC_DISABLE_ROTATE
    , m_rotate(new QPushButton)
#endif
    , m_customConfigButton(new QPushButton)
{
    setObjectName("Display");
#ifndef DCC_DISABLE_ROTATE
    m_rotate->setText(tr("Rotate"));
#endif
    m_customConfigButton->setText(tr("Custom Settings"));
    m_resolution->setTitle(tr("Resolution"));
    m_brightnessSettings->setTitle(tr("Brightness"));
    m_scalingSettings->setTitle(tr("Scaling Settings"));

    QStringList scaleList;
    scaleList << "1.0"
              << "1.25"
              << "1.5"
              << "1.75"
              << "2.0"
              << "2.25"
              << "2.5"
              << "2.75"
              << "3.0";

    DCCSlider *slider = m_scaleWidget->slider();
    slider->setRange(1, 9);
    slider->setType(DCCSlider::Vernier);
    slider->setTickPosition(QSlider::TicksBelow);
    slider->setTickInterval(1);
    slider->setPageStep(1);
    slider->blockSignals(true);
    slider->blockSignals(false);
    m_scaleWidget->setAnnotations(scaleList);

    m_displayControlPageGrp = new SettingsGroup;
    m_displayControlPageGrp->appendItem(m_displayControlPage);

    SettingsGroup *scaleGrp = new SettingsGroup;
    scaleGrp->appendItem(m_scaleWidget);
    scaleGrp->appendItem(m_scalingSettings);
    m_scalingSettings->hide();

    m_resolutionsGrp = new SettingsGroup;
    m_resolutionsGrp->appendItem(m_resolution);

#ifndef DCC_DISABLE_MIRACAST
    m_miracastGrp = new SettingsGroup;
#endif

    SettingsGroup *brightnessGrp = new SettingsGroup;
    brightnessGrp->appendItem(m_brightnessSettings);

    m_centralLayout->addWidget(m_displayControlPageGrp);
    m_centralLayout->addWidget(scaleGrp);
    m_centralLayout->addWidget(m_resolutionsGrp);
#ifndef DCC_DISABLE_MIRACAST
    m_centralLayout->addWidget(m_miracastGrp);
#endif
    m_centralLayout->addWidget(brightnessGrp);
#ifndef DCC_DISABLE_ROTATE
    m_centralLayout->addWidget(m_rotate);
#endif
    m_centralLayout->addWidget(m_customConfigButton);

    setTitle(tr("Display"));

    connect(m_brightnessSettings, &NextPageWidget::clicked, this,
            &DisplayWidget::showBrightnessPage);
    connect(m_scalingSettings, &NextPageWidget::clicked, this, &DisplayWidget::showScalingPage);
    connect(m_resolution, &NextPageWidget::clicked, this, &DisplayWidget::showResolutionPage);
#ifndef DCC_DISABLE_ROTATE
    connect(m_rotate, &QPushButton::clicked, this, &DisplayWidget::requestRotate);
#endif

    connect(m_customConfigButton, &QPushButton::clicked, this, [=] {
        setIndividualScalingEnabled(true);

        // save/record current mode/config state in order to restore later
        Q_EMIT requestRecordCurrentState();

        if (m_model->displayMode() == CUSTOM_MODE && m_model->config() == m_model->DDE_Display_Config) {
            Q_EMIT requestModifyConfig();
        } else {
            Q_EMIT requestNewConfig(m_model->DDE_Display_Config);
        }
    });
    connect(slider, &DCCSlider::valueChanged, this, [=](const int value) {
        Q_EMIT requestUiScaleChanged(convertToScale(value));

        m_scaleWidget->setValueLiteral(QString::number(convertToScale(value)));
    });

    connect(m_displayControlPage, &DisplayControlPage::requestDuplicateMode, this,
            &DisplayWidget::requestDuplicateMode);
    connect(m_displayControlPage, &DisplayControlPage::requestExtendMode, this,
            &DisplayWidget::requestExtendMode);
    connect(m_displayControlPage, &DisplayControlPage::requestOnlyMonitor, this,
            &DisplayWidget::requestOnlyMonitor);

    connect(m_displayControlPage, &DisplayControlPage::requestDuplicateMode, this,
            std::bind(&DisplayWidget::setIndividualScalingEnabled, this, false));
    connect(m_displayControlPage, &DisplayControlPage::requestOnlyMonitor, this,
            std::bind(&DisplayWidget::setIndividualScalingEnabled, this, false));
    connect(m_displayControlPage, &DisplayControlPage::requestExtendMode, this,
            std::bind(&DisplayWidget::setIndividualScalingEnabled, this, true));
}

void DisplayWidget::setModel(DisplayModel *model)
{
    m_model = model;

    connect(m_model, &DisplayModel::monitorListChanged, this, &DisplayWidget::onMonitorListChanged);
    connect(m_model, &DisplayModel::configListChanged, this, &DisplayWidget::onMonitorListChanged);
    connect(m_model, &DisplayModel::screenHeightChanged, this, &DisplayWidget::onScreenSizeChanged,
            Qt::QueuedConnection);
    connect(m_model, &DisplayModel::screenWidthChanged, this, &DisplayWidget::onScreenSizeChanged,
            Qt::QueuedConnection);
    connect(m_model, &DisplayModel::configCreated, this, &DisplayWidget::requestModifyConfig,
            Qt::QueuedConnection);
    connect(m_model, &DisplayModel::uiScaleChanged, this, &DisplayWidget::onUiScaleChanged);

    m_displayControlPage->setModel(model);

    onMonitorListChanged();
    onScreenSizeChanged();
    onUiScaleChanged(m_model->uiScale());
}

#ifndef DCC_DISABLE_MIRACAST
void DisplayWidget::setMiracastModel(MiracastModel *miracastModel)
{
    connect(miracastModel, &MiracastModel::linkAdded, this, &DisplayWidget::onMiracastLinkAdded);
    connect(miracastModel, &MiracastModel::linkRemoved, this,
            &DisplayWidget::onMiracastLinkRemoved);

    m_miracastGrp->setVisible(!m_miracastList.isEmpty());

    for (const LinkInfo &link : miracastModel->links()) onMiracastLinkAdded(link);
}
#endif

void DisplayWidget::onMonitorListChanged() const
{
    const auto mons = m_model->monitorList();

    if (mons.size() <= 1) {
        m_customConfigButton->hide();

        m_displayControlPageGrp->hide();
        m_displayControlPage->hide();

        m_resolutionsGrp->show();
        m_resolution->show();
        setIndividualScalingEnabled(false);
#ifndef DCC_DISABLE_ROTATE
        m_rotate->show();
#endif
    }
    else {

        m_displayControlPage->show();
        m_displayControlPageGrp->show();

        m_customConfigButton->show();

        m_resolutionsGrp->hide();
        m_resolution->hide();
        setIndividualScalingEnabled(m_model->allowEnableMultiScaleRatio());
#ifndef DCC_DISABLE_ROTATE
        m_rotate->hide();
#endif
    }
}

void DisplayWidget::onScreenSizeChanged() const
{
    const QString resolution =
        QString("%1×%2").arg(m_model->screenWidth()).arg(m_model->screenHeight());
    m_resolution->setValue(resolution);
}

#ifndef DCC_DISABLE_MIRACAST
void DisplayWidget::onMiracastLinkAdded(const LinkInfo &link)
{
    if (m_miracastList.contains(link.m_dbusPath)) return;

    NextPageWidget *miracast = new NextPageWidget;
    miracast->setTitle(tr("Wireless Screen Projection"));
    m_miracastGrp->appendItem(miracast);
    m_miracastList.insert(link.m_dbusPath, miracast);
    connect(miracast, &NextPageWidget::clicked, this,
            [=] { Q_EMIT requestMiracastConfigPage(link.m_dbusPath); });

    m_miracastGrp->setVisible(!m_miracastList.isEmpty());
}

void DisplayWidget::onMiracastLinkRemoved(const QDBusObjectPath &path)
{
    NextPageWidget *button = m_miracastList[path];
    if (button) {
        m_miracastGrp->removeItem(button);
        m_miracastList.remove(path);
        button->deleteLater();
    }

    m_miracastGrp->setVisible(!m_miracastList.isEmpty());
}
#endif

void DisplayWidget::onUiScaleChanged(const double scale)
{
    DCCSlider *slider = m_scaleWidget->slider();

    slider->blockSignals(true);
    slider->setValue(convertToSlider(scale));
    slider->blockSignals(false);

    m_scaleWidget->setValueLiteral(QString::number(scale));
}

void DisplayWidget::setIndividualScalingEnabled(bool enabled) const
{
    if(enabled && m_model->allowEnableMultiScaleRatio()) {
        m_scaleWidget->hide();
        m_scalingSettings->show();
    }
    else {
        m_scaleWidget->show();
        m_scalingSettings->hide();
    }
}


int DisplayWidget::convertToSlider(const float value)
{
    //remove base scale (100), then convert to 1-based value
    //with a stepping of 25
    return (int)round(value * 100 - 100) / 25 + 1;
}

float DisplayWidget::convertToScale(const int value)
{
    return 1.0 + (value - 1) * 0.25;
}
