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

#include "monitorsettingdialog.h"
#include "monitorcontrolwidget.h"
#include "displaymodel.h"
#include "settingslistwidget.h"
#include "widgets/basiclistmodel.h"
#include "widgets/basiclistview.h"
#include "widgets/basiclistdelegate.h"
#include "dthememanager.h"
#include <QVBoxLayout>
#include <QTimer>
#include <QMouseEvent>
#include <DSuggestButton>

DWIDGET_USE_NAMESPACE

using namespace dcc::widgets;

namespace dcc {

namespace display {

MonitorSettingDialog::MonitorSettingDialog(DisplayModel *model, QWidget *parent)
    : DAbstractDialog(parent),

      m_primary(true),

      m_model(model),
      m_monitor(nullptr),

      m_positionWatcher(new QTimer(this))
{
    init();
    reloadMonitorObject(model->primaryMonitor());
    initPrimary();
}

MonitorSettingDialog::MonitorSettingDialog(Monitor *monitor, QWidget *parent)
    : DAbstractDialog(parent),

      m_primary(false),
      m_monitor(nullptr),

      m_positionWatcher(new QTimer(this))
{
    init();
    reloadMonitorObject(monitor);
}

MonitorSettingDialog::~MonitorSettingDialog()
{
    qDeleteAll(m_otherDialogs);
}

void MonitorSettingDialog::mouseMoveEvent(QMouseEvent *e)
{
    e->ignore();
}

void MonitorSettingDialog::init()
{
    setWindowFlags(windowFlags() | Qt::X11BypassWindowManagerHint);

    DThemeManager::instance()->setTheme(this, "light");

    m_fullIndication = std::unique_ptr<MonitorIndicator>(new MonitorIndicator(this));

    setBackgroundColor(QColor(Qt::white));
    setBorderColor(QColor(0, 0, 0, 0.2 * 255));

    m_resolutionsModel = new BasicListModel;

    BasicListView *resolutionView = new BasicListView;
    resolutionView->setModel(m_resolutionsModel);
    resolutionView->setItemDelegate(new BasicListDelegate);

    connect(resolutionView, &BasicListView::entered, m_resolutionsModel, &BasicListModel::setHoveredIndex);

    if (m_primary)
    {
        resolutionView->setAutoFitHeight(false);
        resolutionView->setFixedHeight(36 * 3);
    }

    resolutionView->setMinimumWidth(448);

    QLabel *resoLabel = new QLabel;
    resoLabel->setObjectName("Resolution");
    resoLabel->setText(tr("Resolution"));

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->setMargin(0);
    hlayout->setSpacing(0);
    hlayout->addSpacing(30);
    hlayout->addWidget(resoLabel);

    QVBoxLayout *resoLayout = new QVBoxLayout;
    resoLayout->addLayout(hlayout);
    resoLayout->addWidget(resolutionView);
    resoLayout->setSpacing(5);
    resoLayout->setContentsMargins(10, 0, 10, 0);

#ifndef DCC_DISABLE_ROTATE
    m_rotateBtn = new DImageButton;
    m_rotateBtn->setNormalPic(":/display/themes/dark/icons/rotate_normal.png");
    m_rotateBtn->setHoverPic(":/display/themes/dark/icons/rotate_hover.png");
    m_rotateBtn->setPressPic(":/display/themes/dark/icons/rotate_press.png");
#endif

    m_btnsLayout = new QHBoxLayout;
    m_btnsLayout->addSpacing(15);
#ifndef DCC_DISABLE_ROTATE
    m_btnsLayout->addWidget(m_rotateBtn);
#endif
    m_btnsLayout->addStretch();
    m_btnsLayout->setSpacing(10);
    m_btnsLayout->setContentsMargins(10, 0, 10, 0);

    m_mainLayout = new QVBoxLayout;
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(10);

    m_mainLayout->addSpacing(40);
    m_mainLayout->addLayout(resoLayout);
    m_mainLayout->addSpacing(10);
    m_mainLayout->addLayout(m_btnsLayout);
    m_mainLayout->addSpacing(10);

    QWidget *widget = new QWidget;
    widget->setLayout(m_mainLayout);

    setContentsMargins(0, 0, 0, 0);

    setLayout(m_mainLayout);

    m_positionWatcher->setSingleShot(false);
    m_positionWatcher->setInterval(1000);
    m_positionWatcher->start();

    connect(resolutionView, &BasicListView::clicked, [=](const QModelIndex &index) { onMonitorResolutionSelected(index.row()); });
#ifndef DCC_DISABLE_ROTATE
    connect(m_rotateBtn, &DImageButton::clicked, this, &MonitorSettingDialog::onRotateBtnClicked);
#endif
    connect(m_positionWatcher, &QTimer::timeout, this, &MonitorSettingDialog::onMonitorRectChanged);
}

void MonitorSettingDialog::initPrimary()
{
    Q_ASSERT(m_primary);

    QPushButton *cancelBtn = new QPushButton;
    cancelBtn->setText(tr("Cancel"));
    DSuggestButton *applySaveBtn = new DSuggestButton;
    applySaveBtn->setText(tr("Save"));

    m_btnsLayout->addWidget(cancelBtn);
    m_btnsLayout->addWidget(applySaveBtn);

    // add primary screen settings widget
    m_primarySettingsWidget = new SettingsListWidget;
    m_primarySettingsWidget->setTitle(tr("Primary"));
    m_mainLayout->insertWidget(1, m_primarySettingsWidget);

    m_ctrlWidget = new MonitorControlWidget;
    m_ctrlWidget->setDisplayModel(m_model);

    m_mainLayout->insertWidget(1, m_ctrlWidget);

    // add primary settings
    for (auto mon : m_model->monitorList())
        m_primarySettingsWidget->appendOption(mon->name());

    connect(m_ctrlWidget, &MonitorControlWidget::requestMonitorPress, this, &MonitorSettingDialog::onMonitorPress);
    connect(m_ctrlWidget, &MonitorControlWidget::requestMonitorRelease, this, &MonitorSettingDialog::onMonitorRelease);
    connect(m_ctrlWidget, &MonitorControlWidget::requestRecognize, this, &MonitorSettingDialog::requestRecognize);
    connect(m_ctrlWidget, &MonitorControlWidget::requestMerge, this, &MonitorSettingDialog::requestMerge);
    connect(m_ctrlWidget, &MonitorControlWidget::requestSplit, this, &MonitorSettingDialog::requestSplit);
    connect(m_ctrlWidget, &MonitorControlWidget::requestSetMonitorPosition, this, &MonitorSettingDialog::requestSetMonitorPosition);
    connect(m_primarySettingsWidget, &SettingsListWidget::clicked, this, &MonitorSettingDialog::requestSetPrimary);
    connect(m_model, &DisplayModel::primaryScreenChanged, this, &MonitorSettingDialog::onPrimaryChanged);
    connect(m_model, &DisplayModel::screenHeightChanged, this, &MonitorSettingDialog::updateScreensRelation, Qt::QueuedConnection);
    connect(m_model, &DisplayModel::screenWidthChanged, this, &MonitorSettingDialog::updateScreensRelation, Qt::QueuedConnection);
    connect(m_model, &DisplayModel::displayModeChanged, this, &MonitorSettingDialog::reject);
    connect(cancelBtn, &QPushButton::clicked, this, &MonitorSettingDialog::reject);
    connect(applySaveBtn, &DSuggestButton::clicked, this, &MonitorSettingDialog::requestApplySave);
    connect(applySaveBtn, &DSuggestButton::clicked, this, &MonitorSettingDialog::accept);
    reloadOtherScreensDialog();

    onPrimaryChanged();
    QTimer::singleShot(1, this, &MonitorSettingDialog::updateScreensRelation);

    applySaveBtn->setFocus();
}

void MonitorSettingDialog::reloadMonitorObject(Monitor *monitor)
{
    // only refersh mode list
    if (m_monitor == monitor)
        return onMonitorModeChanged();

    if (m_monitor)
    {
        disconnect(m_monitor, &Monitor::currentModeChanged, this, &MonitorSettingDialog::onMonitorModeChanged);
        disconnect(m_monitor, &Monitor::geometryChanged, m_positionWatcher, static_cast<void (QTimer::*)()>(&QTimer::start));
    }

    m_monitor = monitor;

    connect(m_monitor, &Monitor::currentModeChanged, this, &MonitorSettingDialog::onMonitorModeChanged, Qt::QueuedConnection);
    connect(m_monitor, &Monitor::geometryChanged, m_positionWatcher, static_cast<void (QTimer::*)()>(&QTimer::start));

    setWindowTitle(m_monitor->name());
    onMonitorModeChanged();

    QTimer::singleShot(10, this, &MonitorSettingDialog::onMonitorRectChanged);
}

void MonitorSettingDialog::reloadOtherScreensDialog()
{
    qDeleteAll(m_otherDialogs);
    m_otherDialogs.clear();

    // load other non-primary dialogs
    for (auto mon : m_model->monitorList())
    {
        if (mon == m_monitor)
            continue;

        MonitorSettingDialog *dialog = new MonitorSettingDialog(mon, this);

        connect(dialog, &MonitorSettingDialog::requestSetPrimary, this, &MonitorSettingDialog::requestSetPrimary);
        connect(dialog, &MonitorSettingDialog::requestSetMonitorResolution, this, &MonitorSettingDialog::requestSetMonitorResolution);
#ifndef DCC_DISABLE_ROTATE
        connect(dialog, &MonitorSettingDialog::requestMonitorRotate, this, &MonitorSettingDialog::requestMonitorRotate);
#endif
        dialog->show();
        m_otherDialogs.append(dialog);
    }
}

void MonitorSettingDialog::updateScreensRelation()
{
    const bool merged = m_model->monitorsIsIntersect();

    m_ctrlWidget->setScreensMerged(merged);
    m_primarySettingsWidget->setVisible(!merged);

    for (auto d : m_otherDialogs)
        d->setVisible(!merged);

    onMonitorModeChanged();

    adjustSize();
}

void MonitorSettingDialog::onPrimaryChanged()
{
    Q_ASSERT(m_primary);

    // update current index
    const QString primary = m_model->primary();
    for (int i(0); i != m_model->monitorList().size(); ++i)
    {
        if (m_model->monitorList()[i]->name() == primary)
        {
            m_primarySettingsWidget->setSelectedIndex(i);
            break;
        }
    }

    if (m_monitor == m_model->primaryMonitor())
        return;

    reloadMonitorObject(m_model->primaryMonitor());
    reloadOtherScreensDialog();
}

void MonitorSettingDialog::onMonitorRectChanged()
{
    if (!m_monitor)
        return;

    const qreal ratio = devicePixelRatioF();
    const QRect r(m_monitor->rect().topLeft(),
                  m_monitor->rect().size() / ratio);

    DAbstractDialog::move(r.center() - rect().center());
}

void MonitorSettingDialog::onMonitorModeChanged()
{
    const bool intersect = m_primary && m_model->monitorsIsIntersect();
    if (intersect)
        updateModeList(m_model->monitorsSameModeList());
    else
        updateModeList(m_monitor->modeList());

    if (!intersect)
    {
        m_resolutionsModel->setSelectedIndex(m_resolutionsModel->index(m_monitor->modeList().indexOf(m_monitor->currentMode())));
    }
    else
    {
        const ResolutionList list = m_model->monitorsSameModeList();
        const Resolution mode = m_model->monitorList().first()->currentMode();

        for (auto it = list.cbegin(); it != list.cend(); ++it) {
            if (it->id() == mode.id()) {
                m_resolutionsModel->setSelectedIndex(m_resolutionsModel->index(it - list.cbegin()));
                break;
            }
        }
    }
}

void MonitorSettingDialog::updateModeList(const QList<Resolution> &modeList)
{
    m_resolutionsModel->clear();

    bool first = true;
    for (auto r : modeList)
    {
        const QString option = QString::number(r.width()) + "×" + QString::number(r.height());

        if (first)
        {
            first = false;
            m_resolutionsModel->appendOption(option + tr(" (Recommended)"));
        } else {
            m_resolutionsModel->appendOption(option);
        }
    }

    Q_EMIT m_resolutionsModel->layoutChanged();
}

void MonitorSettingDialog::onMonitorResolutionSelected(const int index)
{
    const bool intersect = m_primary && m_model->monitorsIsIntersect();

    if (intersect)
    {
        const ResolutionList modeList = m_model->monitorsSameModeList();
        Q_ASSERT(modeList.size() > index);
        const Resolution mode = modeList[index];

        for (Monitor* mon : m_model->monitorList()) {
            const ResolutionList& list = mon->modeList();
            for (auto it = list.cbegin(); it != list.cend(); ++it) {
                if (it->width() == mode.width() && it->height() == mode.height()) {
                    Q_EMIT requestSetMonitorResolution(mon, it->id());
                    break;
                }
            }
        }
    } else {
        const auto modeList = m_monitor->modeList();
        Q_ASSERT(modeList.size() > index);

        Q_EMIT requestSetMonitorResolution(m_monitor, modeList[index].id());
    }
}

#ifndef DCC_DISABLE_ROTATE
void MonitorSettingDialog::onRotateBtnClicked()
{
    const bool intersect = m_primary && m_model->monitorsIsIntersect();

    if (intersect)
        Q_EMIT requestMonitorRotate(nullptr);
    else
        Q_EMIT requestMonitorRotate(m_monitor);
}
#endif

void MonitorSettingDialog::onMonitorPress(Monitor *mon)
{
    m_fullIndication->setGeometry(mon->rect());
    m_fullIndication->show();
}

void MonitorSettingDialog::onMonitorRelease(Monitor *mon)
{
    // FIXME: I don't know why indicator not hide at new for monitorproxywidget
    m_fullIndication->hide();
}

} // namespace display

} // namespace dcc
