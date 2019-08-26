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

#include "customsettingdialog.h"

#include "modules/display/displaymodel.h"
#include "modules/display/monitor.h"
#include "modules/display/monitorcontrolwidget.h"
#include "modules/display/monitorindicator.h"
#include "widgets/basiclistview.h"

#include <QLabel>
#include <QListView>
#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QStandardItem>

using namespace dcc::display;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::display;
DWIDGET_USE_NAMESPACE

CustomSettingDialog::CustomSettingDialog(QWidget *parent)
    : QDialog(parent)
    , m_isPrimary(true)
{
    init();
}

CustomSettingDialog::CustomSettingDialog(dcc::display::Monitor *mon,
                                         dcc::display::DisplayModel *model,
                                         QWidget *parent):
    QDialog(parent),
    m_isPrimary(false),
    m_monitor(mon),
    m_model(model)
{
    init();
}


CustomSettingDialog::~CustomSettingDialog()
{
    qDeleteAll(m_otherDialog);
}

void CustomSettingDialog::init()
{
    setMinimumSize(640, 800);
    setWindowFlags(windowFlags() | Qt::X11BypassWindowManagerHint);

    m_layout = new QVBoxLayout(this);
    QLabel *resoLabel = new QLabel;
    resoLabel->setObjectName("Resolution");
    resoLabel->setText(tr("Resolution"));
    m_layout->addWidget(resoLabel);

    if (m_isPrimary) {
        m_moniList = new DListView;
        m_moniList->installEventFilter(this);
        m_moniList->setIconSize(QSize(42, 42));
        m_layout->addWidget(m_moniList);
    }

    m_resolutionList = new DListView;
    m_layout->addWidget(m_resolutionList);
    setLayout(m_layout);

    QHBoxLayout *hlayout = new QHBoxLayout();
    m_layout->addLayout(hlayout);

    QPushButton *rotate = new QPushButton(this);
    rotate->setIcon(QIcon::fromTheme("dcc_rotate"));

    hlayout->addWidget(rotate, 0, Qt::AlignLeft);
    connect(rotate, &QPushButton::clicked, this, [this]() {
        Q_EMIT CustomSettingDialog::requestShowRotateDialog(m_monitor);
    });

    hlayout->setMargin(10);

    if (m_isPrimary) {
        QPushButton *btn{nullptr};
        btn = new QPushButton(tr("Cancel"), this);
        connect(btn, &QPushButton::clicked, this, &CustomSettingDialog::reject);
        hlayout->addWidget(btn);

        btn = new QPushButton(tr("Save"), this);
        connect(btn, &QPushButton::clicked, this, &CustomSettingDialog::accept);
        hlayout->addWidget(btn);
    }

    m_fullIndication = std::unique_ptr<MonitorIndicator>(new MonitorIndicator(this));
}

void CustomSettingDialog::setModel(DisplayModel *model)
{
    m_model = model;

    m_monitor = model->primaryMonitor();
    m_isPrimary = true;

    initWithModel();
    initOtherDialog();
}

void CustomSettingDialog::initWithModel()
{
    Q_ASSERT(m_model);

    initResolutionList();
    if (m_isPrimary) {
        m_monitroControlWidget = new MonitorControlWidget();
        m_monitroControlWidget->setDisplayModel(m_model);

        connect(m_monitroControlWidget, &MonitorControlWidget::requestMonitorPress,
                this, &CustomSettingDialog::onMonitorPress);
        connect(m_monitroControlWidget, &MonitorControlWidget::requestMonitorRelease,
                this, &CustomSettingDialog::onMonitorRelease);
        connect(m_monitroControlWidget, &MonitorControlWidget::requestRecognize,
                this, &CustomSettingDialog::requestRecognize);
        connect(m_monitroControlWidget, &MonitorControlWidget::requestMerge,
                this, &CustomSettingDialog::requestMerge);
        connect(m_monitroControlWidget, &MonitorControlWidget::requestSplit,
                this, &CustomSettingDialog::requestSplit);
        connect(m_monitroControlWidget, &MonitorControlWidget::requestSetMonitorPosition,
                this, &CustomSettingDialog::requestSetMonitorPosition);

        m_layout->insertWidget(1, m_monitroControlWidget);

        Q_ASSERT(m_moniList);
        initMoniList();
    }

    move(m_monitor->rect().center() - QPoint(width() / 2, height() / 2));
}

void CustomSettingDialog::initOtherDialog()
{
    qDeleteAll(m_otherDialog);
    m_otherDialog.clear();

    if (m_model->isMerge())
        return;

    for (auto mon : m_model->monitorList()) {
        if (mon == m_model->primaryMonitor())
            continue;
        CustomSettingDialog *dlg = new CustomSettingDialog(mon, m_model, this);

        connect(dlg, &CustomSettingDialog::requestSetResolution, this,
                &CustomSettingDialog::requestSetResolution);
        connect(dlg, &CustomSettingDialog::requestShowRotateDialog, this,
                &CustomSettingDialog::requestShowRotateDialog);

        dlg->initWithModel();
        dlg->show();
        m_otherDialog.append(dlg);
    }
}

void CustomSettingDialog::initResolutionList()
{
    QStandardItemModel *itemModel = qobject_cast<QStandardItemModel *>(m_resolutionList->model());
    if (itemModel)
        itemModel->deleteLater();

    bool first = true;
    auto modes = m_monitor->modeList();
    const auto curMode = m_monitor->currentMode();

    itemModel = new QStandardItemModel(this);
    QStandardItem *curIdx{nullptr};
    for (auto m : modes) {
        if (m_model->isMerge()) {
            bool isComm = true;
            for (auto moni : m_model->monitorList()) {
                if (-1 == moni->modeList().indexOf(m)) {
                    isComm = false;
                    break;
                }
            }

            if (!isComm) {
                continue;
            }
        }

        const QString res = QString::number(m.width()) + "×" + QString::number(m.height());
        QStandardItem *item = new QStandardItem();

        if (first) {
            first = false;
            item->setText(res + QString(" (%1)").arg(tr("Recommended")));
        } else {
            item->setText(res);
        }

        if (curMode == m)
            curIdx = item;
        itemModel->appendRow(item);
    }

    m_resolutionList->setModel(itemModel);
    if (nullptr != curIdx)
        m_resolutionList->setCurrentIndex(curIdx->index());

    connect(m_monitor, &Monitor::currentModeChanged, this, [ = ](const Resolution &) {
        Q_ASSERT(m_model);
        Q_ASSERT(m_monitor);

        auto list = m_monitor->modeList();
        int idx = list.indexOf(m_monitor->currentMode());
        m_resolutionList->setCurrentIndex(m_resolutionList->model()->index(idx, 0));
    });
    connect(m_resolutionList, &QListView::clicked, [ = ](QModelIndex idx) {
        if (m_resolutionList->model()->index(modes.indexOf(m_monitor->currentMode()), 0) == idx)
            return;
        Q_EMIT requestSetResolution(m_monitor, modes[idx.row()].id());
    });
}

void CustomSettingDialog::initMoniList()
{
    auto listModel = new QStandardItemModel;
    m_moniList->setModel(listModel);

    auto moniList = m_model->monitorList();
    for (int idx = 0; idx < moniList.size(); ++idx) {
        auto item = new DStandardItem;
        item->setIcon(QIcon::fromTheme(idx % 2 ? "dcc_display_vga1" : "dcc_display_lvds1"));

        auto moni = moniList[idx];
        auto *titleAction = new DViewItemAction;
        titleAction->setText(moni->name());

        auto *subTitleAction = new DViewItemAction;
        QString str = QString("%1 %2 %3%4x%5").arg(moni->w()).arg(tr("inch"))
                      .arg(tr("Resolution").arg(QString::number(moni->w())).arg(QString::number(moni->h())));
        subTitleAction->setText(str);

        DViewItemActionList actionList;
        actionList << titleAction << subTitleAction;
        item->setTextActionList(actionList);
        listModel->appendRow(item);
    }
}

void CustomSettingDialog::onMonitorPress(Monitor *mon)
{
    m_fullIndication->setGeometry(mon->rect());
    m_fullIndication->show();
}

void CustomSettingDialog::onMonitorRelease(Monitor *mon)
{
    m_fullIndication->hide();
}
