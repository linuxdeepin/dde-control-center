/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     listenerri <listenerri@gmail.com>
 *
 * Maintainer: listenerri <listenerri@gmail.com>
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

#include "abstractsection.h"

#include <QVBoxLayout>
#include <QPaintEvent>
#include <DGuiApplicationHelper>
#include <DStyleOption>

#include <widgets/settingshead.h>
#include <widgets/settingsgroup.h>
#include <widgets/contentwidget.h>

using namespace dcc::widgets;
DWIDGET_USE_NAMESPACE

AbstractSection::AbstractSection(QFrame *parent)
    : QFrame(parent)
    , m_group(new SettingsGroup(this, SettingsGroup::GroupBackground))
    , m_layout(new QVBoxLayout(this))
    , m_head(Q_NULLPTR)
{
    m_layout->addWidget(m_group);
    setLayout(m_layout);
}

AbstractSection::AbstractSection(const QString &title, QFrame *parent)
    : AbstractSection(parent)
{
    m_head = new SettingsHead(this);
    m_head->setTitle(title);
    m_head->setEditEnable(false);

    // 需要与列表中文字对齐，列表文字与边框边距为8
    m_head ->setContentsMargins(8, 0, 0, 0);
    m_layout->insertWidget(0, m_head);
}

AbstractSection::~AbstractSection()
{
}

void AbstractSection::setTitle(const QString &title)
{
    if (m_head)
        m_head->setTitle(title);
}

void AbstractSection::appendItem(SettingsItem *item)
{
    m_group->appendItem(item);
}

void AbstractSection::insertItem(int idx, SettingsItem *item)
{
    m_group->insertItem(idx, item);
}

void AbstractSection::removeItem(SettingsItem *item)
{
    m_group->removeItem(item);
}

void AbstractSection::setSettingsHead(SettingsHead *settingsHead)
{
    m_layout->insertWidget(0, settingsHead);
}

/**
 * @brief 按钮的实现类
 * @param parent
 */
ActionButton::ActionButton(ActionType type, QWidget *parent)
    : QWidget(parent)
    , m_actionType(type)
    , m_isMouseEnter(false)
{
    setMouseTracking(true);
    installEventFilter(this);
}

ActionButton::~ActionButton()
{
}

void ActionButton::paintEvent(QPaintEvent *event)
{
#define STARTPOS 4
#define RECTSIZE 2

    QRect rctEllipse(0, 0, size().width(), size().height());

    QPainter painter(this);
    painter.save();
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::Qt4CompatiblePainting);
    painter.setBrush(backgroundColor());
    painter.setPen(Qt::PenStyle::NoPen);
    painter.drawEllipse(rctEllipse);

    QRect rect(QRect(STARTPOS, (rctEllipse.height() - RECTSIZE) / 2, width() - STARTPOS * 2, RECTSIZE));
    painter.fillRect(rect, QColor(255, 255, 255));
    if (m_actionType == ActionType::Add) {
        rect = QRect((width() - RECTSIZE) /2, STARTPOS, RECTSIZE, height() - STARTPOS * 2);
        painter.fillRect(rect, QColor(255, 255, 255));
    }

    event->accept();
    painter.restore();
}

bool ActionButton::eventFilter(QObject *object, QEvent *event)
{
    if (object == this) {
        switch (event->type()) {
        case QEvent::MouseButtonRelease: {
            Q_EMIT clicked();
            return true;
        }
        case QEvent::Enter: {
            m_isMouseEnter = true;
            update();
            return true;
        }
        case QEvent::Leave: {
            m_isMouseEnter = false;
            update();
            return true;
        }
        default: break;
        }
    }

    return QWidget::eventFilter(object, event);
}

QColor ActionButton::backgroundColor()
{
    if (m_actionType == ActionType::Add)
        return m_isMouseEnter ? QColor(28, 150, 0) : QColor(72, 191, 0);

    return m_isMouseEnter ? QColor(255, 50, 50) : QColor(255, 106, 106);
}
