/*
 * Copyright (C) 2011 ~ 2021 Uniontech Technology Co., Ltd.
 *
 * Author:     huangweihua <huangweihua@uniontech.com>
 *
 * Maintainer: huangweihua <huangweihua@uniontech.com>
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

#include "touchscreenwidget.h"
#include "modules/display/monitor.h"
#include "modules/touchscreen/touchscreenmodel.h"
#include "widgets/settingsitem.h"
#include "widgets/buttontuple.h"

#include <QVBoxLayout>

using namespace dcc::widgets;
using namespace dcc::display;
using namespace DCC_NAMESPACE::touchscreen;

DWIDGET_USE_NAMESPACE

void MCombobox::showPopup()
{
    QComboBox::showPopup();
    QWidget *popup = this->findChild<QFrame*>();
    popup->move(this->mapToGlobal(this->pos()).x() - this->x(), popup->y());
}

TouchscreenWidget::TouchscreenWidget(QWidget *parent)
    : QWidget(parent)
    , m_model(nullptr)
    , m_contentLayout(new QVBoxLayout)
    , m_buttonTuple(new ButtonTuple(ButtonTuple::Save))
    , m_timer(new QTimer(this))
    , m_notifyInter(new Notifications("org.freedesktop.Notifications",
                                      "/org/freedesktop/Notifications",
                                      QDBusConnection::sessionBus(),
                                      this))
{
    setAccessibleName("TouchscreenWidget");
    //~ contents_path /touchscreen/Select your touch screen when connected or set it here.
    m_dTipLabel = new DTipLabel(tr("Select your touch screen when connected or set it here."));
    m_dTipLabel->setWordWrap(true);
    m_dTipLabel->setContentsMargins(10, 0, 0, 0);
    m_dTipLabel->setAlignment(Qt::AlignLeft);

    QPushButton *cancelBtn = m_buttonTuple->leftButton();
    QPushButton *acceptBtn = m_buttonTuple->rightButton();
    m_buttonTuple->setAutoFillBackground(true);
    m_buttonTuple->setBackgroundRole(QPalette::Base);
    cancelBtn->setText(tr("Cancel"));
    cancelBtn->setFixedSize(200, 36);
    acceptBtn->setFixedSize(200, 36);
    acceptBtn->setText(tr("Confirm"));
    acceptBtn->setEnabled(false);

    m_contentLayout->setContentsMargins(56, 20, 56, 10);
    m_contentLayout->addWidget(m_dTipLabel);
    m_contentLayout->addSpacing(10);
    m_contentLayout->addStretch();
    m_contentLayout->addWidget(m_buttonTuple);

    setLayout(m_contentLayout);

    m_timer->setSingleShot(true);
    m_timer->setInterval(100);

    connect(cancelBtn, &QPushButton::clicked, this, &TouchscreenWidget::onMonitorChanged);
    connect(acceptBtn, &QPushButton::clicked, this, &TouchscreenWidget::save);
    connect(m_timer, &QTimer::timeout, this, &TouchscreenWidget::onMonitorChanged);
}

void TouchscreenWidget::setModel(TouchscreenModel *model)
{
    m_model = model;

    connect(m_model, &TouchscreenModel::monitorListChanged, this, &TouchscreenWidget::clearShake);
    connect(m_model, &TouchscreenModel::touchscreenListChanged, this, &TouchscreenWidget::clearShake);
    connect(m_model, &TouchscreenModel::touchscreenMapChanged, this, &TouchscreenWidget::clearShake);

    onMonitorChanged();
}

void TouchscreenWidget::clearShake()
{
    if (!m_timer->isActive()) {
        m_timer->start();
    }
}

void TouchscreenWidget::onMonitorChanged()
{
    for (const auto &item : m_settingsItem) {
        item->deleteLater();
    }

    m_list.clear();

    m_settingsItem.clear();

    m_buttonTuple->rightButton()->setEnabled(false);

    TouchscreenMap touchMap = m_model->touchMap();
    TouchscreenInfoList touchscreenList = m_model->touchscreenList();
    QList<Monitor *> monitorList = m_model->monitorList();
    int idx = 0;
    int monitorMaxIdx = monitorList.count() - 1;
    for (const auto &i : touchscreenList) {
        QString touchscreenSerial = i.serialNumber;

        auto title = QString(tr("Touch Screen - %1 (%2)")).arg(i.name).arg(i.id);
        auto *label = new DLabel(title);
        label->setElideMode(Qt::ElideRight);
        label->setToolTip(title);

        MCombobox *listCombo = new MCombobox;
        listCombo->setProperty("touchscreenName", i.name);
        listCombo->setProperty("touchscreenSerial", touchscreenSerial);
        for (Monitor *m : monitorList) {
            listCombo->addItem(m->name());
        }

        // 第一次开机 touchMap 为空, 会设置默认值
        if (touchMap.isEmpty()) {
            listCombo->setCurrentIndex(idx > monitorMaxIdx ? monitorMaxIdx : idx);
        }

        if (touchMap.find(touchscreenSerial) != touchMap.end()) {
            listCombo->setCurrentText(touchMap.value(touchscreenSerial));
        }

        m_list.push_back(listCombo);

        SettingsItem *modeSettingsItem = new SettingsItem;
        QHBoxLayout *itemLayout = new QHBoxLayout;
        itemLayout->setContentsMargins(10, 0, 10, 0);
        itemLayout->addWidget(label);
        itemLayout->addWidget(listCombo);
        listCombo->setFocusPolicy(Qt::NoFocus);
        listCombo->setMinimumWidth(238);
        listCombo->setMinimumHeight(36);
        modeSettingsItem->addBackground();
        modeSettingsItem->setFixedHeight(56);
        modeSettingsItem->setLayout(itemLayout);
        m_contentLayout->insertWidget(1, modeSettingsItem);
        m_settingsItem.append(modeSettingsItem);

        connect(listCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this] {
            m_buttonTuple->rightButton()->setEnabled(true);
        });
        idx++;
    }
}

void TouchscreenWidget::save()
{
    m_buttonTuple->rightButton()->setEnabled(false);
    bool changed = false;
    for (auto i : m_list) {
        QString touchscreenName = i->property("touchscreenName").toString();
        QString touchscreenSerial = i->property("touchscreenSerial").toString();
        QString output = i->currentText();
        TouchscreenMap touchMap = m_model->touchMap();
        if (touchMap.value(touchscreenSerial) == output) {
            continue;
        }

        changed = true;
        Q_EMIT requestAssociateTouch(output, touchscreenSerial);
    }

    if (changed) {
        m_notifyInter->Notify("dde-control-center",
                              static_cast<uint>(QDateTime::currentMSecsSinceEpoch()),
                              "preferences-system",
                              tr("Touch Screen Settings"),
                              tr("The settings of touch screen changed"),
                              QStringList(),
                              QVariantMap(),
                              3000);
    }
}
