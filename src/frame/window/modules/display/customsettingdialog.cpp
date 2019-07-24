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
#include "monitorinfodelegate.h"
#include "monitorinfomodel.h"

#include "modules/display/displaymodel.h"
#include "modules/display/monitor.h"
#include "modules/display/monitorcontrolwidget.h"
#include "modules/display/monitorindicator.h"
#include "widgets/basiclistview.h"

#include "dimagebutton.h"

#include <QLabel>
#include <QListView>
#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QStandardItem>

using namespace dcc::display;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::display;
DWIDGET_USE_NAMESPACE

CustomSettingDialog::CustomSettingDialog(QWidget *parent):
    QDialog(parent),
    m_isPrimary(true)
{
    init();
}

CustomSettingDialog::CustomSettingDialog(dcc::display::Monitor *mon,
                                         dcc::display::DisplayModel *model,
                                         QWidget *parent):
    QDialog(parent),
    m_monitor(mon),
    m_isPrimary(false),
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

    if(m_isPrimary)
    {
        m_baseListView = new BasicListView();
        m_baseListView->installEventFilter(this);
        m_layout->addWidget(m_baseListView);
    }

    m_resolutionList = new QListView;
    m_layout->addWidget(m_resolutionList);
    setLayout(m_layout);

    m_layout->addStretch(1);

    QHBoxLayout *hlayout = new QHBoxLayout();
    m_layout->addLayout(hlayout);

    DImageButton *rotate = new DImageButton();
    rotate->setNormalPic(":/display/themes/dark/icons/rotate_normal.png");
    rotate->setHoverPic(":/display/themes/dark/icons/rotate_hover.png");
    rotate->setPressPic(":/display/themes/dark/icons/rotate_press.png");

    hlayout->addWidget(rotate);
    connect(rotate, &DImageButton::clicked, this, [ = ]() { Q_EMIT CustomSettingDialog::requestShowRotateDialog(m_monitor);});

    hlayout->addStretch(1);
    hlayout->setMargin(10);

    if (m_isPrimary) {
        QPushButton *btn{nullptr};
        btn = new QPushButton("cancel", this);
        connect(btn, &QPushButton::clicked, this, &CustomSettingDialog::reject);
        hlayout->addWidget(btn);

        btn = new QPushButton("save", this);
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

        Q_ASSERT(m_baseListView);
        m_baseListView->setModel(new MonitorInfoModel(m_model));
        m_baseListView->setItemDelegate(new MonitorInfoDelegate);
    }

    move(m_monitor->rect().center() - QPoint(width() / 2, height() / 2));
}

void CustomSettingDialog::initOtherDialog()
{
    for (int i = 0; i < m_otherDialog.size(); ++i) {
        m_otherDialog[i]->deleteLater();
    }
    m_otherDialog.clear();

    for (auto mon : m_model->monitorList()) {
        if (mon == m_model->primaryMonitor())
            continue;
        CustomSettingDialog *dlg = new CustomSettingDialog(mon, m_model, this);

        connect(dlg, &CustomSettingDialog::requestSetResolution, this, &CustomSettingDialog::requestSetResolution);
        connect(dlg, &CustomSettingDialog::requestShowRotateDialog, this, &CustomSettingDialog::requestShowRotateDialog);
        dlg->initWithModel();
        dlg->show();
        m_otherDialog.append(dlg);
    }
}

void CustomSettingDialog::initResolutionList()
{
    QStandardItemModel *itemModel = dynamic_cast<QStandardItemModel *>(m_resolutionList->model());
    if (itemModel) {
        itemModel->clear();
    }

    bool first = true;
    auto modes = m_monitor->modeList();
    const auto curMode = m_monitor->currentMode();

    itemModel = new QStandardItemModel(this);
    QStandardItem *curIdx{nullptr};
    for (auto m : modes) {
        const QString res = QString::number(m.width()) + "×" + QString::number(m.height());
        QStandardItem *item = new QStandardItem();

        if (first) {
            first = false;
            item->setText(res + tr(" (Recommended)"));
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

    connect(m_monitor,&Monitor::currentModeChanged,this,[=](const Resolution&){
        Q_ASSERT(m_model);
        Q_ASSERT(m_monitor);

        auto list = m_monitor->modeList();
        int idx = list.indexOf(m_monitor->currentMode());
        m_resolutionList->setCurrentIndex(m_resolutionList->model()->index(idx, 0));
    });
    connect(m_resolutionList, &QListView::clicked, [ = ](QModelIndex idx) {
        if(m_resolutionList->model()->index(modes.indexOf(m_monitor->currentMode()),0) == idx)
            return;
        Q_EMIT requestSetResolution(m_monitor, modes[idx.row()].id());
    });
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
