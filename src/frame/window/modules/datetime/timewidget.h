/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
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

#include "window/namespace.h"
#include "widgets/settingsitem.h"

#include <dimagebutton.h>

#include <QObject>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QEvent>
#include <QFrame>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace datetime {
class DatetimeModel;
}
}

namespace DCC_NAMESPACE {
namespace datetime {

class TimeWidget : public dcc::widgets::SettingsItem
{
    Q_OBJECT
public:
    explicit TimeWidget(QFrame *parent = 0, bool bType = true);

    int getEditValue() const;

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    QLineEdit *m_lineEdit;
    DImageButton *m_addBtn;
    DImageButton *m_reducedBtn;
    QVBoxLayout *m_btnLayout;
    QHBoxLayout *m_layout;
    int m_value;
    bool m_bType;

public Q_SLOTS:
    void slotAdd();
    void slotReduced();
    void slotSetLineEdit();
};

}// namespace datetime
}// namespace DCC_NAMESPACE
