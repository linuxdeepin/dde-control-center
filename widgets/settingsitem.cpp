/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "settingsitem.h"

#include <QStyle>

namespace dcc {
namespace widgets {

SettingsItem::SettingsItem(QWidget *parent)
    : QFrame(parent),
      m_isHead(false),
      m_isTail(false),
      m_isErr(false)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

bool SettingsItem::isHead() const
{
    return m_isHead;
}

void SettingsItem::setIsHead(bool head)
{
    if (head == m_isHead) return;
    m_isHead = head;

    style()->unpolish(this);
    style()->polish(this);
}

bool SettingsItem::isTail() const
{
    return m_isTail;
}

void SettingsItem::setIsTail(bool tail)
{
    if (tail == m_isTail) return;
    m_isTail = tail;

    style()->unpolish(this);
    style()->polish(this);
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

}
}
