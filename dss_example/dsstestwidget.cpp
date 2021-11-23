/*
* Copyright (C) 2021 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     Zhang Qipeng <donghualin@uniontech.com>
*
* Maintainer: Zhang Qipeng <donghualin@uniontech.com>
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

#include "dsstestwidget.h"
#include "network_module.h"
#include <QPushButton>

#include <DPalette>
#include <DFloatingButton>
#include <QMouseEvent>

using namespace dss::module;
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
