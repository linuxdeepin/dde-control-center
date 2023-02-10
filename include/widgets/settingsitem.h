//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
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

}
