// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef INSTALLER_UI_DELEGATES_POPUP_MENU_DELEGATE_H
#define INSTALLER_UI_DELEGATES_POPUP_MENU_DELEGATE_H

#include <QStyledItemDelegate>

namespace installer {

// Draw menu item in popup window.
class PopupMenuDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit PopupMenuDelegate(QWidget *parent = nullptr);

protected:
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
};

} // namespace installer

#endif // INSTALLER_UI_DELEGATES_POPUP_MENU_DELEGATE_H
