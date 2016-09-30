/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef SETTINGSITEM_H
#define SETTINGSITEM_H

#include <QFrame>

namespace dcc {

class SettingsItem : public QFrame
{
    Q_OBJECT
public:
    explicit SettingsItem(QFrame *parent = 0);

    bool isHead() const;
    void setIsHead(bool head = true);

    bool isTail() const;
    void setIsTail(bool tail = true);

private:
    bool m_isHead;
    bool m_isTail;
};

}

#endif // SETTINGSITEM_H
