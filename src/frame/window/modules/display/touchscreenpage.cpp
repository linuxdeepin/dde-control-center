/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author: quezhiyong@uniontech.com
 *         wangwei@uniontech.com
 *
 * Maintainer: quezhiyong@uniontech.com
 *             wangwei@uniontech.com
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
#include "touchscreenpage.h"
#include "modules/display/displaymodel.h"
#include "widgets/titlelabel.h"
#include "widgets/settingshead.h"
#include "widgets/buttontuple.h"
#include "widgets/translucentframe.h"
#include "window/utils.h"

#include <DToolTip>

#include <QVBoxLayout>
#include <QGridLayout>
#include <QComboBox>
#include <QScrollArea>
#include <QScroller>
#include <QList>

using namespace DCC_NAMESPACE::display;
using namespace dcc::display;
using namespace dcc::widgets;
DWIDGET_USE_NAMESPACE

TouchscreenPage::TouchscreenPage(QWidget *parent)
    : ContentWidget(parent)
    , m_buttonTuple(new ButtonTuple(ButtonTuple::Save))
    , m_contentArea(new QScrollArea())
    , m_notifyInter(new Notifications("org.freedesktop.Notifications",
                                      "/org/freedesktop/Notifications",
                                      QDBusConnection::sessionBus(),
                                      this))
{
    setTitle(tr("Select your touch screen"));

    auto dtip = new DTipLabel(tr("Select your touch screen when connected or set it here."));

    m_contentArea->setWidgetResizable(true);
    m_contentArea->setFrameStyle(QFrame::NoFrame);
    m_contentArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_contentArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_contentArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_contentArea->setContentsMargins(0, 0, 0, 0);
    QScroller::grabGesture(m_contentArea->viewport(), QScroller::LeftMouseButtonGesture);

    QPushButton *cancelBtn = m_buttonTuple->leftButton();
    QPushButton *acceptBtn = m_buttonTuple->rightButton();
    m_buttonTuple->setAutoFillBackground(true);
    cancelBtn->setText(tr("Cancel"));
    acceptBtn->setText(tr("Confirm"));
    acceptBtn->setEnabled(false);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->addWidget(dtip);
    mainLayout->addWidget(m_contentArea);
    layout()->addWidget(m_buttonTuple);

    TranslucentFrame *w = new TranslucentFrame;
    w->setLayout(mainLayout);

    setContent(w);

    connect(cancelBtn, &QPushButton::clicked, this, &TouchscreenPage::onMonitorChanged);
    connect(acceptBtn, &QPushButton::clicked, this, &TouchscreenPage::save);
}

void TouchscreenPage::setModel(DisplayModel *model)
{
    m_model = model;

    connect(m_model, &DisplayModel::monitorListChanged, this, &TouchscreenPage::onMonitorChanged);
    connect(m_model, &DisplayModel::touchscreenListChanged, this, &TouchscreenPage::onMonitorChanged);
    connect(m_model, &DisplayModel::touchscreenMapChanged, this, &TouchscreenPage::onMonitorChanged);

    onMonitorChanged();
}

void TouchscreenPage::onMonitorChanged()
{
    auto *oldWidget = m_contentArea->widget();
    if (oldWidget) {
        oldWidget->deleteLater();
    }
    m_list.clear();

    auto *mainWidget = new QWidget();
    m_contentArea->setWidget(mainWidget);

    auto *layout = new QVBoxLayout();
    mainWidget->setLayout(layout);
    layout->setAlignment(Qt::AlignTop);

    TouchscreenMap touchMap = m_model->touchMap();
    TouchscreenInfoList touchscreenList = m_model->touchscreenList();
    auto monitorList = m_model->monitorList();
    for (const auto &i : touchscreenList) {
        QString touchscreenSerial = i.serialNumber;

        auto text = QString(tr("Touch Screen - %1 (%2)")).arg(i.name).arg(i.id);
        auto *label = new QLabel(text);
        label->setToolTip(text);
        layout->addWidget(label);
        layout->addSpacing(5);

        auto *listCombo = new QComboBox();
        listCombo->setProperty("touchscreenName", i.name);
        listCombo->setProperty("touchscreenSerial", touchscreenSerial);
        layout->addWidget(listCombo);
        layout->addSpacing(10);
        m_list.push_back(listCombo);

        for (Monitor *m : monitorList) {
            listCombo->addItem(m->name());
        }

        if (touchMap.find(touchscreenSerial) != touchMap.end()) {
            listCombo->setCurrentText(touchMap.value(touchscreenSerial));
            connect(listCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this] {
                m_buttonTuple->rightButton()->setEnabled(true);
            });
        }
    }
}

void TouchscreenPage::save()
{
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
