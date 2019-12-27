/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
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

#include "widgets/settingsitem.h"

#include <DPalette>

#include <QStyle>
#include <QVBoxLayout>
#include <QResizeEvent>

DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE

namespace dcc {
namespace widgets {

SettingsItem::SettingsItem(QWidget *parent)
    : QFrame(parent)
    , m_isErr(false)
{
}


bool SettingsItem::isErr() const
{
    return m_isErr;
}

void SettingsItem::setIsErr(const bool err)
{
    if (m_isErr == err) return;
    m_isErr = err;

    style()->unpolish(this);
    style()->polish(this);
}

void SettingsItem::addBackground()
{
    //当 SettingsItem 被　appendItem　加入　SettingsGroup　时被调用
    QMargins mg(0, 0, 0, 0);

    //因为需要SettingsItem 整体为一个圆角，所有用一个　widget　当背景
    auto bgWidget = new QWidget();

    //将　widget　加入到一个新建的　DBackgroundGroup中
    auto bglayout = new QVBoxLayout;
    bglayout->setContentsMargins(mg);
    if (m_bgGroup)
        m_bgGroup->deleteLater();
    m_bgGroup = new DFrame(this);
    m_bgGroup->setContentsMargins(0, 0, 0, 0);
    m_bgGroup->setBackgroundRole(DPalette::ItemBackground);
    m_bgGroup->setLineWidth(0);
    bglayout->addWidget(bgWidget);

    //将 m_bgGroup 沉底
    m_bgGroup->lower();

    //设置m_bgGroup 的大小
    m_bgGroup->setFixedSize(size());
}

void SettingsItem::resizeEvent(QResizeEvent *event)
{
    QFrame::resizeEvent(event);

    //设置m_bgGroup 的大小
    if (m_bgGroup)
        m_bgGroup->setFixedSize(size());
}
}
}

