// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dsstestwidget.h"
#include "network_module.h"
#include <QPushButton>

#include <DPalette>
#include <DFloatingButton>
#include <QMouseEvent>

#include <networkcontroller.h>

using namespace dss::module;
using namespace dde::network;
using namespace Dtk::Widget;
DGUI_USE_NAMESPACE

DssTestWidget::DssTestWidget(QWidget *parent)
    : QWidget(parent)
    , m_pModule(new NetworkPlugin(this))
{
    m_button = new Dtk::Widget::DFloatingButton(this);
    m_button->setIconSize(QSize(26, 26));
    m_button->setFixedSize(QSize(52, 52));
    m_button->setAutoExclusive(true);
    m_button->setBackgroundRole(DPalette::Button);
    m_button->installEventFilter(this);
    loadDssPlugin();
}

DssTestWidget::~DssTestWidget()
{
}

void DssTestWidget::loadDssPlugin()
{
    NetworkController::setServiceType(ServiceLoadType::LoadFromManager);
    m_pModule->init();
    m_button->setIcon(QIcon(m_pModule->icon()));
}

bool DssTestWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_button) {
        switch (event->type()) {
        case QEvent::MouseButtonPress: {
                QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
                if (mouseEvent->button() == Qt::RightButton) {
                    const QString itemMenu = m_pModule->itemContextMenu();
                    qInfo() << itemMenu;
                }
            }
            break;
        default: break;
        }
    }

    return QWidget::eventFilter(watched, event);
}
