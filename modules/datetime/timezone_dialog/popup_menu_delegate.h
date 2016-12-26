// Copyright (c) 2016 Deepin Ltd. All rights reserved.
// Use of this source is governed by General Public License that can be found
// in the LICENSE file.

#ifndef INSTALLER_UI_DELEGATES_POPUP_MENU_DELEGATE_H
#define INSTALLER_UI_DELEGATES_POPUP_MENU_DELEGATE_H

#include <QStyledItemDelegate>

namespace installer {

// Draw menu item in popup window.
class PopupMenuDelegate : public QStyledItemDelegate {
  Q_OBJECT

 public:
  explicit PopupMenuDelegate(QWidget* parent = nullptr);

 protected:
  void paint(QPainter* painter,
             const QStyleOptionViewItem& option,
             const QModelIndex& index) const override;
};

}  // namespace installer

#endif  // INSTALLER_UI_DELEGATES_POPUP_MENU_DELEGATE_H
