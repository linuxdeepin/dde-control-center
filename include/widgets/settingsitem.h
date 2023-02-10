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
#pragma once

#include "interface/namespace.h"

#include <QFrame>

namespace DCC_NAMESPACE {

class SettingsItem : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(bool isErr READ isErr DESIGNABLE true SCRIPTABLE true)

public:
    explicit SettingsItem(QWidget *parent = nullptr);

    bool isErr() const;
    virtual void setIsErr(const bool err = true);

    void addBackground();
    void removeBackground();

    bool clickable() const;
    void setClickable(const bool clickable);

Q_SIGNALS:
    void clicked(QWidget *widget);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    bool m_isErr;
    bool m_hasBack;
    bool m_hover;
    bool m_clickable;
};

} // namespace DCC_NAMESPACE
