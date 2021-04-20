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
#include "displaywidget.h"
#include "resolutiondetailpage.h"
#include "brightnesspage.h"
#include "rotatedialog.h"
#include "scalingpage.h"
#include "multiscreensettingpage.h"
#include "multiscreendetailpage.h"
#include "refreshratepage.h"
#include "touchscreenpage.h"
#include "window/utils.h"

#include "widgets/timeoutdialog.h"
#include "modules/display/displaymodel.h"
#include "modules/display/displayworker.h"
#include "modules/display/recognizedialog.h"

#include <QApplication>
#include <DApplicationHelper>

using namespace dcc::display;
using namespace DCC_NAMESPACE::display;

DisplayModule::DisplayModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
    , m_displayModel(nullptr)
    , m_displayWorker(nullptr)
{
}

DisplayModule::~DisplayModule()
{
    m_displayModel->deleteLater();
    m_displayWorker->deleteLater();
}

void DisplayModule::windowUpdate()
{
    if (m_pMainWindow)
        m_pMainWindow->updateWinsize();
}

void DisplayModule::initialize()
{
}

const QString DisplayModule::name() const
{
    return QStringLiteral("display");
}

const QString DisplayModule::displayName() const
{
    return tr("Display");
}

void DisplayModule::active()
{
    m_displayWidget = new DisplayWidget(m_displayModel);
    m_displayWidget->setVisible(false);
    m_displayWidget->setModel();

    connect(m_displayWidget, &DisplayWidget::requestShowScalingPage,
            this, &DisplayModule::showScalingPage);
    connect(m_displayWidget, &DisplayWidget::requestShowResolutionPage,
            this, &DisplayModule::showResolutionDetailPage);
    connect(m_displayWidget, &DisplayWidget::requestShowBrightnessPage,
            this, &DisplayModule::showBrightnessPage);
    connect(m_displayWidget, &DisplayWidget::requestShowRefreshRatePage,
            this, &DisplayModule::showRefreshRotePage);
    connect(m_displayWidget, &DisplayWidget::requestRotate, this, [ this ] {
        windowUpdate();
        showRotate();
    });
    connect(m_displayWidget, &DisplayWidget::requestShowMultiScreenPage,
            this, &DisplayModule::showMultiScreenSettingPage);
    connect(m_displayWidget, &DisplayWidget::requestShowCustomConfigPage,
            this, &DisplayModule::showCustomSettingDialog);
    connect(m_displayWidget, &DisplayWidget::requestShowTouchscreenPage,
            this, &DisplayModule::showTouchScreenPage);
    connect(m_displayWidget, &DisplayWidget::requestShowTouchscreenPage,
            this, &DisplayModule::showTouchRecognize);
    connect(m_displayWidget, &DisplayWidget::requestShowMultiResolutionPage,
            this, &DisplayModule::showMultiResolutionPage);
    connect(m_displayWidget, &DisplayWidget::requestShowMultiRefreshRatePage,
            this, &DisplayModule::showMultiRefreshRatePage);

    m_frameProxy->pushWidget(this, m_displayWidget);
    m_displayWidget->setVisible(true);
    m_displayWidget->setDefaultWidget();
}

int DisplayModule::load(const QString &path)
{
    if (!m_displayWidget) {
        active();
    }

    return m_displayWidget->showPath(path);
}

void DisplayModule::preInitialize(bool sync , FrameProxyInterface::PushType pushtype)
{
    Q_UNUSED(sync);
    Q_UNUSED(pushtype);
    if (m_displayModel) {
        delete m_displayModel;
    }
    m_displayModel = new DisplayModel;
    m_displayWorker = new DisplayWorker(m_displayModel);

    m_displayModel->moveToThread(qApp->thread());
    m_displayWorker->moveToThread(qApp->thread());

    connect(m_displayModel, &DisplayModel::monitorListChanged, this, [this]() {
        m_frameProxy->setRemoveableDeviceStatus(tr("Multiple Displays"), m_displayModel->monitorList().size() > 1);
        if (m_displayWidget) {
            m_displayWidget->initMenuUI();
        }
    });
    connect(m_displayModel, &DisplayModel::touchscreenListChanged, this, [this]() {
        if (m_displayWidget) {
            m_displayWidget->initMenuUI();
        }
    });
    QTimer::singleShot(0, m_displayWorker, [=] {
        m_displayWorker->active();
    });
}

QStringList DisplayModule::availPage() const
{
    QStringList sl;
    sl << "Resolution"
       << "Refresh Rate"
       << "Brightness";

    if (m_displayModel && m_displayModel->monitorList().size() > 1) {
        sl << "Multiple Displays";
    }

    if (IsServerSystem) {
        sl << "Display Scaling";
    }

    if (m_displayModel && !m_displayModel->touchscreenList().isEmpty()) {
        sl << "Touch Screen";
    }

    return sl;
}

void DisplayModule::showBrightnessPage()
{
    BrightnessPage *page = new BrightnessPage;
    page->setVisible(false);
    page->setMode(m_displayModel);
    connect(page, &BrightnessPage::requestSetColorTemperature, m_displayWorker, &DisplayWorker::setColorTemperature);
    connect(page, &BrightnessPage::requestSetMonitorBrightness,
            m_displayWorker, &DisplayWorker::setMonitorBrightness);
    connect(page, &BrightnessPage::requestAmbientLightAdjustBrightness,
            m_displayWorker, &DisplayWorker::setAmbientLightAdjustBrightness);
    connect(page, &BrightnessPage::requestSetMethodAdjustCCT,
                m_displayWorker, &DisplayWorker::SetMethodAdjustCCT);

    m_frameProxy->pushWidget(this, page);
    page->setVisible(true);
}

void DisplayModule::showResolutionDetailPage()
{
    ResolutionDetailPage *page = new ResolutionDetailPage;
    page->setVisible(false);
    page->setModel(m_displayModel);

    connect(page, &ResolutionDetailPage::requestSetResolution, this,
            &DisplayModule::onDetailPageRequestSetResolution);
    connect(page, &ResolutionDetailPage::requestReset, m_displayWorker,
            &DisplayWorker::discardChanges);
    connect(page, &ResolutionDetailPage::requestSave, m_displayWorker,
            &DisplayWorker::saveChanges);

    m_frameProxy->pushWidget(this, page);
    page->setVisible(true);
}

void DisplayModule::showScalingPage()
{
    ScalingPage *page = new ScalingPage;
    page->setVisible(false);
    page->setModel(m_displayModel);

    connect(page, &ScalingPage::requestUiScaleChange,
            m_displayWorker, &DisplayWorker::setUiScale);
    connect(page, &ScalingPage::requestIndividualScaling,
            m_displayWorker, &DisplayWorker::setIndividualScaling);

    m_frameProxy->pushWidget(this, page);
    page->setVisible(true);
}

void DisplayModule::showMultiScreenSettingPage()
{
    MultiScreenSettingPage *page = new MultiScreenSettingPage();
    page->setVisible(false);
    page->setModel(m_displayModel);

    connect(page, &MultiScreenSettingPage::requestDuplicateMode, m_displayWorker,
            &DisplayWorker::duplicateMode);
    connect(page, &MultiScreenSettingPage::requestExtendMode, m_displayWorker,
            &DisplayWorker::extendMode);
    connect(page, &MultiScreenSettingPage::requestOnlyMonitor, m_displayWorker,
            &DisplayWorker::onlyMonitor);
    connect(page, &MultiScreenSettingPage::requestCustomMode, [this]() {
        m_displayWorker->switchMode(0, m_displayModel->DDE_Display_Config);
    });
    connect(page, &MultiScreenSettingPage::requestCustomDiglog, this,
            &DisplayModule::showCustomSettingDialog);
    connect(page, &MultiScreenSettingPage::requsetCreateConfig,
            m_displayWorker, &DisplayWorker::createConfig);
    connect(page, &MultiScreenSettingPage::requsetRecord,
            m_displayWorker, &DisplayWorker::record);

    m_frameProxy->pushWidget(this, page);
    page->setVisible(true);
}

void DisplayModule::showCustomSettingDialog()
{
    auto displayMode = m_displayModel->displayMode();
    Q_ASSERT(displayMode == CUSTOM_MODE);

    CustomSettingDialog *dlg = new CustomSettingDialog();

    connect(dlg, &CustomSettingDialog::requestShowRotateDialog,
            this, &DisplayModule::showRotate);
    connect(dlg, &CustomSettingDialog::requestSetResolution, this,
            &DisplayModule::onCustomPageRequestSetResolution);
    connect(dlg, &CustomSettingDialog::requestMerge,
            m_displayWorker, &DisplayWorker::mergeScreens);
    connect(dlg, &CustomSettingDialog::requestEnalbeMonitor, [=](Monitor *mon, bool enable) {
        m_displayWorker->onMonitorEnable(mon, enable);
    });
    connect(dlg, &CustomSettingDialog::requestSplit,
            m_displayWorker, &DisplayWorker::splitScreens);
    connect(dlg, &CustomSettingDialog::requestSetMonitorPosition,
            m_displayWorker, &DisplayWorker::setMonitorPosition);
    connect(dlg, &CustomSettingDialog::requestRecognize, this,
            &DisplayModule::showDisplayRecognize);
    connect(dlg, &CustomSettingDialog::requestSetPrimaryMonitor,
            m_displayWorker, &DisplayWorker::setPrimary);
    connect(m_displayModel, &DisplayModel::monitorListChanged, dlg, &QDialog::reject);

    m_displayModel->setIsMerge(m_displayModel->monitorsIsIntersect());
    dlg->setModel(m_displayModel);
    if (dlg->exec() != QDialog::Accepted) {
        m_displayWorker->restore();
    } else {
        m_displayWorker->saveChanges();
    }

    dlg->deleteLater();
}

void DisplayModule::showRefreshRotePage()
{
    auto page = new RefreshRatePage();
    page->setVisible(false);
    page->setModel(m_displayModel);

    connect(page, &RefreshRatePage::requestSetResolution,
            this, &DisplayModule::onDetailPageRequestSetResolution);

    m_frameProxy->pushWidget(this, page);
    page->setVisible(true);

}

void DisplayModule::showTouchScreenPage()
{
    auto page = new TouchscreenPage();
    page->setVisible(false);
    page->setModel(m_displayModel);

    connect(page, &TouchscreenPage::requestAssociateTouch, m_displayWorker, &DisplayWorker::setTouchScreenAssociation);

    m_frameProxy->pushWidget(this, page);
    page->setVisible(true);
}

void DisplayModule::showMultiResolutionPage()
{
    auto page = new MultiScreenDetailPage(true);
    page->setVisible(false);
    page->setModel(m_displayModel);
    page->setVisible(true);

    m_frameProxy->pushWidget(this, page);
}

void DisplayModule::showMultiRefreshRatePage()
{
    auto page = new MultiScreenDetailPage(false);
    page->setVisible(false);
    page->setModel(m_displayModel);

    m_frameProxy->pushWidget(this, page);
    page->setVisible(true);
}

void DisplayModule::onDetailPageRequestSetResolution(Monitor *mon, const int mode)
{
    auto lastMode = mon->currentMode().id();
    m_displayWorker->setMonitorResolution(mon, mode);
    m_displayWorker->applyChanges();

    if (showTimeoutDialog(mon) == QDialog::Accepted) {
        m_displayWorker->saveChanges();
    } else {
        m_displayWorker->setMonitorResolution(mon, lastMode);
        m_displayWorker->applyChanges();
    }
}

void DisplayModule::onCustomPageRequestSetResolution(Monitor *mon, CustomSettingDialog::ResolutionDate mode)
{
    CustomSettingDialog::ResolutionDate lastres;
    if (mon) {
        lastres.id = mon->currentMode().id();
    } else {
        lastres.w = qint16(m_displayModel->primaryMonitor()->currentMode().width());
        lastres.h = qint16(m_displayModel->primaryMonitor()->currentMode().height());
        lastres.rate = m_displayModel->primaryMonitor()->currentMode().rate();
    }

    auto tfunc = [this](Monitor *tmon, CustomSettingDialog::ResolutionDate tmode) {
        if (!tmon) {
            int w = tmode.w;
            int h = tmode.h;
            double r = tmode.rate;
            qDebug() << "resolution:"<< tmode.w << "x" << tmode.h
                     << "\t rate:" << tmode.rate
                     << "\t id: " << tmode.id;
            for (auto m : m_displayModel->monitorList()) {
                for (auto res : m->modeList()) {
                    if (fabs(r) < 0.000001) {
                        if (res.width() == w && res.height() == h) {
                            m_displayWorker->setMonitorResolution(m, res.id());
                            break;
                        }
                    } else {
                        if (res.width() == w && res.height() == h) {
                            if (m->hasRatefresh(r)) {
                                if (abs(res.rate() - r) < 0.000001) {
                                    m_displayWorker->setMonitorResolution(m, res.id());
                                    break;
                                }
                            } else {
                                m_displayWorker->setMonitorResolutionBySize(m, w, h);
                                break;
                            }
                        }
                    }
                }
            }
        } else {
            qDebug() << "resolution:"<< tmode.w << "x" << tmode.h
                     << "\t rate:" << tmode.rate
                     << "\t id: " << tmode.id;
            m_displayWorker->setMonitorResolution(tmon, tmode.id);
        }
        m_displayWorker->applyChanges();
    };

    tfunc(mon, mode);

    if (showTimeoutDialog(mon ? mon : m_displayModel->primaryMonitor()) != QDialog::Accepted) {
        tfunc(mon, lastres);
    }
}

int DisplayModule::showTimeoutDialog(Monitor *mon)
{
    TimeoutDialog *timeoutDialog = new TimeoutDialog(15);
    qreal radio = qApp->devicePixelRatio();
    connect(mon, &Monitor::geometryChanged, timeoutDialog, [ = ] {
        if (timeoutDialog) {
            QRectF rt(mon->x(), mon->y(), mon->w() / radio, mon->h() / radio);
            timeoutDialog->moveToCenterByRect(rt.toRect());
        }
    }, Qt::QueuedConnection);
    connect(timeoutDialog, &TimeoutDialog::closed,
            timeoutDialog, &TimeoutDialog::deleteLater);

    return timeoutDialog->exec();
}

void DisplayModule::showDisplayRecognize()
{
    // 复制模式
    if (m_displayModel->monitorsIsIntersect()) {
        QString text = m_displayModel->monitorList().first()->name();
        for (int i = 1; i < m_displayModel->monitorList().size(); i++) {
            text += QString(" = %1").arg(m_displayModel->monitorList()[i]->name());
        }

        // 所在显示器不存在显示框
        if (m_recognizeDialg.value(text) == nullptr) {
            RecognizeDialog *dialog = new RecognizeDialog(m_displayModel->monitorList()[0], text);
            QTimer::singleShot(5000, this, [=]{
                dialog->deleteLater();
                m_recognizeDialg.remove(text);
            });
            m_recognizeDialg[text] = dialog;
        }
    } else { // 拓展模式 or 自定义模式
        for (auto monitor : m_displayModel->monitorList()) {
            // 所在显示器不存在显示框
            if (m_recognizeDialg.value(monitor->name()) == nullptr) {
                RecognizeDialog *dialog = new RecognizeDialog(monitor, monitor->name());
                m_recognizeDialg[monitor->name()] = dialog;
                QTimer::singleShot(5000, this, [=]{
                    dialog->deleteLater();
                    m_recognizeDialg.remove(monitor->name());
                });
                m_recognizeDialg[monitor->name()] = dialog;
            }
        }
    }
}

void DisplayModule::showTouchRecognize()
{
    // 只有一块屏幕不显示触摸提示
    if (m_displayModel->monitorList().size() < 2) {
        return;
    }

    // 复制模式
    if (m_displayModel->monitorsIsIntersect()) {
        QString text = m_displayModel->monitorList().first()->name();
        for (int i = 1; i < m_displayModel->monitorList().size(); i++) {
            text += QString(" = %1").arg(m_displayModel->monitorList()[i]->name());
        }

        // 所在显示器不存在显示框
        if (m_recognizeDialg.value(text) == nullptr) {
            RecognizeDialog *dialog = new RecognizeDialog(m_displayModel->monitorList()[0], text);
            QTimer::singleShot(5000, this, [=]{
                dialog->deleteLater();
                m_recognizeDialg.remove(text);
            });
            m_recognizeDialg[text] = dialog;
        }
    } else { // 拓展模式 or 自定义模式
        for (auto monitor : m_displayModel->monitorList()) {
            // 所在显示器不存在显示框
            if (m_recognizeDialg.value(monitor->name()) == nullptr) {
                RecognizeDialog *dialog = new RecognizeDialog(monitor, monitor->name());
                m_recognizeDialg[monitor->name()] = dialog;
                QTimer::singleShot(5000, this, [=]{
                    dialog->deleteLater();
                    m_recognizeDialg.remove(monitor->name());
                });
                m_recognizeDialg[monitor->name()] = dialog;
            }
        }
    }
}

void DisplayModule::showRotate(Monitor *mon)
{
    if (DGuiApplicationHelper::isTabletEnvironment()) {
        mon = mon ? mon : m_displayModel->primaryMonitor();
        Q_ASSERT(mon);

        const auto rotates = mon->rotateList();
        const auto rotate = mon->rotate();
        const int s = rotates.size();
        Q_ASSERT(rotates.contains(rotate));

        const quint16 nextValue = rotates[(rotates.indexOf(rotate) - 1 + s) % s];
        m_displayWorker->setMonitorRotate(mon, nextValue);
    } else {
        RotateDialog *dialog = new RotateDialog(mon);
        dialog->setModel(m_displayModel);

        connect(dialog, &RotateDialog::requestRotate, m_displayWorker, &DisplayWorker::setMonitorRotate);
        connect(dialog, &RotateDialog::requestRotateAll, m_displayWorker, &DisplayWorker::setMonitorRotateAll);

        QMap<Monitor *, quint16> mMonitorRotate;
        for (auto m : m_displayModel->monitorList()) {
            mMonitorRotate.insert(m, m->rotate());
        }

        qApp->setOverrideCursor(Qt::BlankCursor);
        if (QDialog::DialogCode::Accepted == dialog->exec()) {
            // if monitor list size > 1 means the config file will be saved by CustomSettingDialog
            qDebug() << "monitor size: " << m_displayModel->monitorList().size() <<
                        ", displayMode is " << m_displayModel->displayMode();
            if (m_displayModel->monitorList().size() == 1 || m_displayModel->displayMode() != CUSTOM_MODE) {
                qDebug() << "m_displayWorker->saveChanges()" << "rotate:" << m_displayModel->monitorList()[0]->rotate();
                m_displayWorker->saveChanges();
            }
        } else {
            for (auto m : m_displayModel->monitorList()) {
                if (mMonitorRotate.end() == mMonitorRotate.find(m))
                    continue;

                if (m->rotate() != mMonitorRotate[m]) {
                    m_displayWorker->setMonitorRotate(m, mMonitorRotate[m]);
                }
            }
        }

        qApp->restoreOverrideCursor();
        QCursor::setPos(m_displayWidget->getRotateBtnPos());
        dialog->deleteLater();
    }
}
