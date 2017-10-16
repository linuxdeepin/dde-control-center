/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
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
#include "translucentframe.h"
#include "nextpagewidget.h"
#include "editablenextpagewidget.h"
#include "settingsgroup.h"
#include "displaymodel.h"
#include "displayworker.h"
#include "basiclistdelegate.h"
#include "titledslideritem.h"
#include "dccslider.h"

using namespace dcc::widgets;
using namespace dcc::display;

DisplayWidget::DisplayWidget()
    : ModuleWidget(),

      m_resolution(new NextPageWidget),
      m_brightnessSettings(new NextPageWidget),
      m_customSettingsGrp(new SettingsGroup),
      m_scaleWidget(new TitledSliderItem(tr("Display scaling"))),
#ifndef DCC_DISABLE_ROTATE
      m_rotate(new QPushButton),
#endif
      m_createConfig(new QPushButton),

      m_configListRefershTimer(new QTimer(this))
{
    setObjectName("Display");
#ifndef DCC_DISABLE_ROTATE
    m_rotate->setText(tr("Rotate"));
#endif
    m_createConfig->setText(tr("New custom settings"));

    m_resolution->setTitle(tr("Resolution"));

    m_brightnessSettings->setTitle(tr("Brightness"));

    QStringList scaleList;
    scaleList << "1.0" << "1.25" << "1.5" << "1.75" << "2.0";

    DCCSlider *slider = m_scaleWidget->slider();
    slider->setRange(1,5);
    slider->setType(DCCSlider::Vernier);
    slider->setTickPosition(QSlider::TicksBelow);
    slider->setTickInterval(1);
    slider->setPageStep(1);
    slider->blockSignals(true);
    slider->blockSignals(false);
    m_scaleWidget->setAnnotations(scaleList);

    SettingsGroup *scaleGrp = new SettingsGroup;
    scaleGrp->appendItem(m_scaleWidget);

    m_resolutionsGrp = new SettingsGroup;
    m_resolutionsGrp->appendItem(m_resolution);

#ifndef DCC_DISABLE_MIRACAST
    m_miracastGrp = new SettingsGroup;
#endif

    m_customSettingsGrp = new SettingsGroup;

    SettingsGroup *brightnessGrp = new SettingsGroup;
    brightnessGrp->appendItem(m_brightnessSettings);

    m_centralLayout->addWidget(scaleGrp);
    m_centralLayout->addWidget(m_resolutionsGrp);
#ifndef DCC_DISABLE_MIRACAST
    m_centralLayout->addWidget(m_miracastGrp);
#endif
    m_centralLayout->addWidget(brightnessGrp);
#ifndef DCC_DISABLE_ROTATE
    m_centralLayout->addWidget(m_rotate);
#endif
    m_centralLayout->addWidget(m_customSettingsGrp);
    m_centralLayout->addWidget(m_createConfig);

    m_configListRefershTimer->setSingleShot(true);
    m_configListRefershTimer->setInterval(100);

    setTitle(tr("Display"));

    connect(m_brightnessSettings, &NextPageWidget::clicked, this, &DisplayWidget::showBrightnessPage);
    connect(m_resolution, &NextPageWidget::clicked, this, &DisplayWidget::showResolutionPage);
#ifndef DCC_DISABLE_ROTATE
    connect(m_rotate, &QPushButton::clicked, this, &DisplayWidget::requestRotate);
#endif
    connect(m_createConfig, &QPushButton::clicked, this, &DisplayWidget::requestNewConfig);
    connect(m_configListRefershTimer, &QTimer::timeout, this, &DisplayWidget::onConfigListChanged);
    connect(slider, &DCCSlider::valueChanged, this, [=] (const int value) {
        emit requestUiScaleChanged(converToScale(value));

        m_scaleWidget->setValueLiteral(QString::number(converToScale(value)));
    });
}

void DisplayWidget::setModel(DisplayModel *model)
{
    m_model = model;

    connect(m_model, &DisplayModel::monitorListChanged, this, &DisplayWidget::onScreenListChanged);
    connect(m_model, &DisplayModel::configListChanged, this, &DisplayWidget::onScreenListChanged);
    connect(m_model, &DisplayModel::configListChanged, m_configListRefershTimer, static_cast<void (QTimer::*)()>(&QTimer::start));
    connect(m_model, &DisplayModel::currentConfigChanged, m_configListRefershTimer, static_cast<void (QTimer::*)()>(&QTimer::start));
    connect(m_model, &DisplayModel::displayModeChanged, m_configListRefershTimer, static_cast<void (QTimer::*)()>(&QTimer::start));
    connect(m_model, &DisplayModel::screenHeightChanged, this, &DisplayWidget::onScreenSizeChanged, Qt::QueuedConnection);
    connect(m_model, &DisplayModel::screenWidthChanged, this, &DisplayWidget::onScreenSizeChanged, Qt::QueuedConnection);
    connect(m_model, &DisplayModel::firstConfigCreated, this, &DisplayWidget::onFirstConfigCreated, Qt::QueuedConnection);
    connect(m_model, &DisplayModel::uiScaleChanged, this, &DisplayWidget::onUiScaleChanged);

    onScreenListChanged();
    onScreenSizeChanged();
    m_configListRefershTimer->start();
    onUiScaleChanged(m_model->uiScale());
}

#ifndef DCC_DISABLE_MIRACAST
void DisplayWidget::setMiracastModel(MiracastModel *miracastModel)
{
    connect(miracastModel, &MiracastModel::linkAdded, this, &DisplayWidget::onMiracastLinkAdded);
    connect(miracastModel, &MiracastModel::linkRemoved, this, &DisplayWidget::onMiracastLinkRemoved);

    m_miracastGrp->setVisible(!m_miracastList.isEmpty());

    for (const LinkInfo &link : miracastModel->links())
        onMiracastLinkAdded(link);
}
#endif

void DisplayWidget::onScreenListChanged() const
{
    const auto mons = m_model->monitorList();

    if (mons.size() <= 1)
    {
        m_createConfig->hide();
        m_customSettingsGrp->hide();

        m_resolutionsGrp->show();
        m_resolution->show();
#ifndef DCC_DISABLE_ROTATE
        m_rotate->show();
#endif
    } else {
        m_createConfig->show();
        m_customSettingsGrp->setVisible(!m_model->configList().isEmpty());

        m_resolutionsGrp->hide();
        m_resolution->hide();
#ifndef DCC_DISABLE_ROTATE
        m_rotate->hide();
#endif
    }
}

void DisplayWidget::onScreenSizeChanged() const
{
    const QString resolution = QString("%1×%2").arg(m_model->screenWidth()).arg(m_model->screenHeight());
    m_resolution->setValue(resolution);
}

void DisplayWidget::onConfigListChanged()
{
    m_customSettingsGrp->clear();
    for (auto *w : m_customSettings)
        QTimer::singleShot(1, w, &NextPageWidget::deleteLater);
    m_customSettings.clear();

    const auto mode = m_model->displayMode();
    const auto current = m_model->config();
    const auto configList = m_model->configList();

    for (const auto &config : configList)
    {
        EditableNextPageWidget *w = new EditableNextPageWidget;
        w->setTitle(config);
        if (mode == CUSTOM_MODE && config == current)
            w->setIcon(loadPixmap(":/widgets/themes/dark/icons/select.png"));

        connect(w, &EditableNextPageWidget::textChanged, this, &DisplayWidget::requestModifyConfigName);
        connect(w, &EditableNextPageWidget::acceptNextPage, this, [=] { emit requestConfigPage(config); });
        connect(w, &EditableNextPageWidget::selected, this, [=] { emit requestSwitchConfig(config); });

        m_customSettingsGrp->appendItem(w);
    }
}

void DisplayWidget::onFirstConfigCreated(const QString &config)
{
    emit requestConfigPage(config);
    emit requestModifyConfig(config);
}

#ifndef DCC_DISABLE_MIRACAST
void DisplayWidget::onMiracastLinkAdded(const LinkInfo &link)
{
    if (m_miracastList.contains(link.m_dbusPath))
        return;

    NextPageWidget *miracast = new NextPageWidget;
    miracast->setTitle(tr("Wireless Screen Projection"));
    m_miracastGrp->appendItem(miracast);
    m_miracastList.insert(link.m_dbusPath, miracast);
    connect(miracast, &NextPageWidget::clicked, this, [=]{
        emit requestMiracastConfigPage(link.m_dbusPath);
    });

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
    slider->setValue(converToSlider(scale));
    slider->blockSignals(false);

    m_scaleWidget->setValueLiteral(QString::number(scale));
}

int DisplayWidget::converToSlider(const float value)
{
    if (value <= 1.0)
        return 1;
    else if (value <= 1.25)
        return 2;
    else if (value <= 1.5)
        return 3;
    else if (value <= 1.75)
        return 4;
    else if (value > 1.75)
        return 5;
    else
        return 1;
}

float DisplayWidget::converToScale(const int value)
{
    switch (value) {
    case 1:
        return 1.0;
        break;
    case 2:
        return 1.25;
        break;
    case 3:
        return 1.5;
        break;
    case 4:
        return 1.75;
        break;
    case 5:
        return 2.0;
        break;
    default:
        return 1.0;
        break;
    }
}

