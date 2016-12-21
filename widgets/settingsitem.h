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
namespace widgets {

class SettingsItem : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(bool isHead READ isHead DESIGNABLE true SCRIPTABLE true)
    Q_PROPERTY(bool isTail READ isTail DESIGNABLE true SCRIPTABLE true)
    Q_PROPERTY(bool isErr READ isErr DESIGNABLE true SCRIPTABLE true)

public:
    explicit SettingsItem(QWidget *parent = 0);

    bool isHead() const;
    void setIsHead(bool head = true);

    bool isTail() const;
    void setIsTail(bool tail = true);

    bool isErr() const;
    void setIsErr(const bool err = true);

private:
    bool m_isHead;
    bool m_isTail;
    bool m_isErr;
};

}
}

#endif // SETTINGSITEM_H
