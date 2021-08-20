/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
 *
 * Maintainer: liuhong <liuhong_cm@deepin.com>
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
#include "widgets/settingsitem.h"

#include "DIconButton"
#include "DLineEdit"

QT_BEGIN_NAMESPACE
class QWidget;
class QHBoxLayout;
class QStackedWidget;
class QLabel;
class QLineEdit;
QT_END_NAMESPACE

DWIDGET_USE_NAMESPACE

namespace DCC_NAMESPACE {
namespace authentication {

class AccounntFingeItem : public dcc::widgets::SettingsItem
{
    Q_OBJECT
public:
    explicit AccounntFingeItem(QWidget *parent = nullptr);
    void setTitle(const QString &title);
    QString getTitle() const { return m_fingerName; };
    void alertTitleRepeat();
    void appendItem(QWidget *widget);
    void setShowIcon(bool state);
    void setEditTitle(bool state);
    void setHideTitle(bool state);
    bool onNameEditFinished();
    //判断账户名是否符合规则
    bool validateName(const QString &password);
    void showAlertMessage(const QString &errMsg);

Q_SIGNALS:
    void removeClicked();
    void editClicked(bool state);
    void editTextFinished(QString finger);

private:
    QHBoxLayout *m_layout;
    QLabel *m_title;
    DIconButton *m_removeBtn;
    DIconButton *m_editBtn;
    DLineEdit *m_editTitle;
    QString m_fingerName;
};

}
}
