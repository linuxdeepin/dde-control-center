/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "settingsitem.h"

namespace dcc {

SettingsItem::SettingsItem(QFrame *parent)
    : QFrame(parent),
      m_isHead(false),
      m_isTail(false)
{
    setStyleSheet("dcc--SettingsItem {"
                  "background-color:gray;"
                  "}"
                  "dcc--SettingsItem[isHead=true] {"
                  "border-top-left-radius:10px;"
                  "border-top-right-radius:10px;"
                  "}"
                  "dcc--SettingsItem[isTail=true] {"
                  "border-bottom-left-radius:10px;"
                  "border-bottom-right-radius:10px;"
                  "}");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

bool SettingsItem::isHead() const
{
    return m_isHead;
}

void SettingsItem::setIsHead(bool head)
{
//    static const QString HeadStyleSheet = "dcc--SettingsItem { border-top-left-radius:10px; border-top-right-radius:10px; } ";

    if (head == m_isHead) return;
    m_isHead = head;

    setStyleSheet(styleSheet());
//    if (head) {
//        setStyleSheet(styleSheet() + HeadStyleSheet);
//    } else {
//        setStyleSheet(styleSheet().replace(HeadStyleSheet, ""));
//    }
}

bool SettingsItem::isTail() const
{
    return m_isTail;
}

void SettingsItem::setIsTail(bool tail)
{
//    static const QString TailStyleSheet = "dcc--SettingsItem { border-bottom-left-radius:10px; border-bottom-right-radius:10px; } ";

    if (tail == m_isTail) return;
    m_isTail = tail;

    setStyleSheet(styleSheet());
//    if (tail) {
//        setStyleSheet(styleSheet() + TailStyleSheet);
//    } else {
//        setStyleSheet(styleSheet().replace(TailStyleSheet, ""));
//    }
}

}
