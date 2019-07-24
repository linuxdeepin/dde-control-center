/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     lq <longqi_cm@deepin.com>
 *
 * Maintainer: lq <longqi_cm@deepin.com>
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

#include "displaymodule.h"
#include "widgets/contentwidget.h"
#include "displaywidget.h"
#include "resolutiondetailpage.h"
#include "brightnesspage.h"
#include "rotatedialog.h"
#include "scalingpage.h"
#include "multiscreensettingpage.h"
#include "customsettingdialog.h"

#include "widgets/timeoutdialog.h"
#include "modules/display/displaymodel.h"
#include "modules/display/displayworker.h"
#include "modules/display/recognizedialog.h"

#include <QApplication>

using namespace dcc::display;
using namespace DCC_NAMESPACE::display;

DisplayModule::DisplayModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent),
      ModuleInterface(frame),
      m_displayModel(nullptr),
      m_displayWorker(nullptr)
{

}

DisplayModule::~DisplayModule()
{
    m_displayModel->deleteLater();
    m_displayWorker->deleteLater();
}


void DisplayModule::initialize()
{
    m_displayModel = new DisplayModel;
    m_displayWorker = new DisplayWorker(m_displayModel);

    m_displayModel->moveToThread(qApp->thread());
    m_displayWorker->moveToThread(qApp->thread());
}

void DisplayModule::reset()
{

}

void DisplayModule::contentPopped(QWidget *const w)
{
    w->deleteLater();
}

const QString DisplayModule::name() const
{
    return QStringLiteral("display");
}

QWidget *DisplayModule::moduleWidget()
{
    DisplayWidget* displayWidget = new DisplayWidget;
    displayWidget->setModel(m_displayModel);

    connect(displayWidget, &DisplayWidget::requestShowScalingPage, this, &DisplayModule::showScalingPage);
    connect(displayWidget, &DisplayWidget::requestShowResolutionPage, this, &DisplayModule::showResolutionDetailPage);
    connect(displayWidget, &DisplayWidget::requestShowBrightnessPage, this, &DisplayModule::showBrightnessPage);
    connect(displayWidget, &DisplayWidget::requestRotate, [ = ] { showRotate(nullptr); });
    connect(displayWidget, &DisplayWidget::requestShowMultiScreenPage, this, &DisplayModule::showMultiScreenSettingPage);
    connect(displayWidget, &DisplayWidget::requestShowCustomConfigPage, this, &DisplayModule::showCustomSettingDialog);
    connect(displayWidget, &DisplayWidget::requsetCreateConfig, m_displayWorker, &DisplayWorker::createConfig);
    connect(displayWidget, &DisplayWidget::requsetRecord, m_displayWorker, &DisplayWorker::record);

    return displayWidget;
}

void DisplayModule::showBrightnessPage()
{
    m_displayWorker->updateNightModeStatus();

    BrightnessPage *page = new BrightnessPage;

    page->setMode(m_displayModel);

    connect(page, &BrightnessPage::requestSetMonitorBrightness, m_displayWorker, &DisplayWorker::setMonitorBrightness);
    connect(page, &BrightnessPage::requestAmbientLightAdjustBrightness, m_displayWorker, &DisplayWorker::setAmbientLightAdjustBrightness);
    connect(page, &BrightnessPage::requestSetNightMode, m_displayWorker, &DisplayWorker::setNightMode);

    m_frameProxy->pushWidget(this, page);
}

void DisplayModule::showResolutionDetailPage()
{
    ResolutionDetailPage *page = new ResolutionDetailPage;
    page->setModel(m_displayModel);

    connect(page, &ResolutionDetailPage::requestSetResolution, this, &DisplayModule::onDetailPageRequestSetResolution);
    m_frameProxy->pushWidget(this, page);
}

void DisplayModule::showScalingPage()
{
    ScalingPage *page = new ScalingPage;

    page->setModel(m_displayModel);
    connect(page, &ScalingPage::requestUiScaleChange, m_displayWorker, &DisplayWorker::setUiScale);
    connect(page, &ScalingPage::requestIndividualScaling, m_displayWorker, &DisplayWorker::setIndividualScaling);

    m_frameProxy->pushWidget(this, page);
}


void DisplayModule::showMultiScreenSettingPage()
{
    MultiScreenSettingPage *page = new MultiScreenSettingPage();

    page->setModel(m_displayModel);

    connect(page, &MultiScreenSettingPage::requestDuplicateMode, m_displayWorker,
            &DisplayWorker::duplicateMode);
    connect(page, &MultiScreenSettingPage::requestExtendMode, m_displayWorker,
            &DisplayWorker::extendMode);
    connect(page, &MultiScreenSettingPage::requestOnlyMonitor, m_displayWorker,
            &DisplayWorker::onlyMonitor);

    m_frameProxy->pushWidget(this, page);
}

void DisplayModule::showCustomSettingDialog()
{
    auto displayMode = m_displayModel->displayMode();

    Q_ASSERT(displayMode == CUSTOM_MODE);

    for (auto mon : m_displayModel->monitorList())
        m_displayWorker->setMonitorEnable(mon, true);

    CustomSettingDialog *dlg = new CustomSettingDialog();

    connect(dlg, &CustomSettingDialog::requestShowRotateDialog, this, &DisplayModule::showRotate);
    connect(dlg, &CustomSettingDialog::requestSetResolution, this, &DisplayModule::onCustomPageRequestSetResolution);
    connect(dlg, &CustomSettingDialog::requestMerge, m_displayWorker, &DisplayWorker::mergeScreens);
    connect(dlg, &CustomSettingDialog::requestSplit, m_displayWorker, &DisplayWorker::splitScreens);
    connect(dlg, &CustomSettingDialog::requestSetMonitorPosition, m_displayWorker, &DisplayWorker::setMonitorPosition);
    connect(dlg, &CustomSettingDialog::requestRecognize, this, &DisplayModule::showRecognize);

    dlg->setModel(m_displayModel);
    dlg->exec();
    dlg->deleteLater();
}

void DisplayModule::onDetailPageRequestSetResolution(Monitor *mon, const int mode)
{
    m_displayWorker->setMonitorResolution(mon, mode);

    if (showTimeoutDialog(mon) == QDialog::Accepted) {
        m_displayWorker->saveChanges();
        return;
    }

    m_displayWorker->restore();
}

void DisplayModule::onCustomPageRequestSetResolution(Monitor *mon, const int mode)
{
    m_displayWorker->setMonitorResolution(mon, mode);

    if (m_displayModel->isMerge()) {
        return;
    }

    if (showTimeoutDialog(mon) != QDialog::Accepted) {
        m_displayWorker->restore();
    }
}

int DisplayModule::showTimeoutDialog(Monitor *mon)
{
    TimeoutDialog *timeoutDialog = new TimeoutDialog(15);

    qreal radio = qApp->devicePixelRatio();
    connect(mon, &Monitor::geometryChanged, timeoutDialog, [ = ] {
        if (timeoutDialog)
        {
            timeoutDialog->moveToCenterByRect(QRect(mon->x(), mon->y(), mon->w() / radio, mon->h() / radio));
        }
    }, Qt::QueuedConnection);
    connect(timeoutDialog, &TimeoutDialog::closed, timeoutDialog, &TimeoutDialog::deleteLater);

    return timeoutDialog->exec();
}

void DisplayModule::showRotate(Monitor *mon)
{
    RotateDialog *dialog = new RotateDialog(mon);

    dialog->setModel(m_displayModel);

    connect(dialog, &RotateDialog::requestRotate, m_displayWorker, &DisplayWorker::setMonitorRotate);
    connect(dialog, &RotateDialog::requestRotateAll, m_displayWorker, &DisplayWorker::setMonitorRotateAll);

    if (QDialog::DialogCode::Accepted == dialog->exec()) {
        // if monitor list size > 1 means the config file will be saved by MonitorSettingDialog

//        if (m_displayModel->monitorList().size() == 1)
//            m_displayWorker->saveChanges();
    } else {
        m_displayWorker->restore();
    }

    dialog->deleteLater();

}

void DisplayModule::showRecognize()
{
    RecognizeDialog dialog(m_displayModel);
    dialog.exec();
}
