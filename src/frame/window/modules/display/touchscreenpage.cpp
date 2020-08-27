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
#include <QScrollArea>
#include <QScroller>
#include <QList>

using namespace DCC_NAMESPACE::display;
using namespace dcc::display;
using namespace dcc::widgets;
DWIDGET_USE_NAMESPACE

void MCombobox::showPopup()
{
    QComboBox::showPopup();
    QWidget *popup = this->findChild<QFrame*>();
    popup->move(this->mapToGlobal(this->pos()).x() - this->x(), popup->y());
}

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

    m_titleString = tr("Select your touch screen when connected or set it here.");
    m_dTipLabel = new DTipLabel(m_titleString);
    m_dTipLabel->setWordWrap(true);
    m_dTipLabel->setContentsMargins(10, 0, 0, 0);
    m_dTipLabel->setAlignment(Qt::AlignLeft);

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

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setContentsMargins(ThirdPageContentsMargins);
    mainLayout->addWidget(m_dTipLabel);
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
    m_titleName.clear();
    m_labels.clear();

    auto *mainWidget = new QWidget();
    mainWidget->installEventFilter(this);
    m_contentArea->setWidget(mainWidget);

    auto *layout = new QVBoxLayout();
    mainWidget->setLayout(layout);
    layout->setAlignment(Qt::AlignTop);
    layout->setContentsMargins(0,0,0,0);

    TouchscreenMap touchMap = m_model->touchMap();
    TouchscreenInfoList touchscreenList = m_model->touchscreenList();
    auto monitorList = m_model->monitorList();
    for (const auto &i : touchscreenList) {
        QString touchscreenSerial = i.serialNumber;

        auto title = QString(tr("Touch Screen - %1 (%2)")).arg(i.name).arg(i.id);
        auto *label = new QLabel(title);
        label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        label->setContentsMargins(10, 0, 0, 0);
        label->setAlignment(Qt::AlignLeft);
        layout->addWidget(label);
        layout->addSpacing(5);

        auto *listCombo = new MCombobox();
        listCombo->setContentsMargins(0, 0, 0, 10);
        listCombo->setProperty("touchscreenName", i.name);
        listCombo->setProperty("touchscreenSerial", touchscreenSerial);
        listCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        layout->addWidget(listCombo);
        layout->addSpacing(10);
        m_list.push_back(listCombo);
        m_labels.push_back(label);
        m_titleName << title;

        for (Monitor *m : monitorList) {
            listCombo->addItem(m->name());
        }

        if (touchMap.find(touchscreenSerial) != touchMap.end()) {
            listCombo->setCurrentText(touchMap.value(touchscreenSerial));
        }
    }
}

void TouchscreenPage::save()
{
    for (auto i : m_list) {
        QString touchscreenName = i->property("touchscreenName").toString();
        QString touchscreenSerial = i->property("touchscreenSerial").toString();
        QString output = i->currentText();
        TouchscreenMap touchMap = m_model->touchMap();
        if (touchMap.value(touchscreenSerial) == output) {
            continue;
        }

        Q_EMIT requestAssociateTouch(output, touchscreenSerial);

        m_notifyInter->Notify("dde-control-center",
                              static_cast<uint>(QDateTime::currentMSecsSinceEpoch()),
                              "preferences-system",
                              tr("Touch Screen Settings"),
                              tr("The settings of touch screen %1 changed").arg(touchscreenName),
                              QStringList(),
                              QVariantMap(),
                              3000);
    }
}

bool TouchscreenPage::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);
    if (event->type() == QEvent::Resize) {
        for (int i = 0; i < m_labels.count(); i++)
        {
            QFontMetrics fontMetrics(m_titleName[i]);
            int fontSize = fontMetrics.width(m_titleName[i]);
            if (fontSize > m_labels[i]->width()) {
                m_labels[i]->setText(fontMetrics.elidedText(m_titleName[i], Qt::ElideMiddle, m_labels[i]->width()));
                m_labels[i]->setToolTip(m_titleName[i]);
            } else {
                m_labels[i]->setText(m_titleName[i]);
                m_labels[i]->setToolTip("");
            }
        }

        return true;
    }
    return dcc::ContentWidget::eventFilter(obj, event);
}
