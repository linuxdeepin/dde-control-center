/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     duanhongyu <duanhongyu@uniontech.com>

 * Maintainer: duanhongyu <duanhongyu@uniontech.com>
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

#ifndef BUTTONEDITWIDGET_H
#define BUTTONEDITWIDGET_H

#include "widgets/settingsitem.h"

DWIDGET_USE_NAMESPACE

QT_BEGIN_NAMESPACE
class QHBoxLayout;
class QLabel;
QT_END_NAMESPACE

DWIDGET_BEGIN_NAMESPACE
class DLineEdit;
class DIconButton;
DWIDGET_END_NAMESPACE

namespace dcc {
namespace widgets {

class ErrorTip;

class ButtonEditWidget : public SettingsItem
{
    Q_OBJECT

public:
    explicit ButtonEditWidget(QFrame *parent = nullptr);
    ~ButtonEditWidget();

    inline DLineEdit *dTextEdit() const { return m_edit; }
    inline DIconButton *addBtn() const { return m_addBtn; }
    inline DIconButton *reduceBtn() const { return m_reduceBtn; }

    void initConnect();
    void hideIconBtn();
    void showIconBtn();
    void hideAlertMessage();

Q_SIGNALS:
    void addNewDnsEdit() const;
    void deleteCurrentDnsEdit() const;

public Q_SLOTS:
    void setTitle(const QString &title);
    void setText(const QString &text);

protected:
    QHBoxLayout *m_mainLayout;

private:
    QLabel *m_title;
    dcc::widgets::ErrorTip *m_errTip;
    Dtk::Widget::DLineEdit *m_edit;
    Dtk::Widget::DIconButton *m_addBtn;
    Dtk::Widget::DIconButton *m_reduceBtn;
};

}
}

#endif // BUTTONEDITWIDGET_H
