// Copyright (c) 2016 Deepin Ltd. All rights reserved.
// Use of this source is governed by General Public License that can be found
// in the LICENSE file.

#ifndef INSTALLER_UI_WIDGETS_TOOLTIP_PIN_H
#define INSTALLER_UI_WIDGETS_TOOLTIP_PIN_H

#include <QLabel>
class QPaintEvent;

namespace installer {

// Displays tooltip text with a sharp corner at middle of bottom edge.
class TooltipPin : public QLabel {
  Q_OBJECT

 public:
  explicit TooltipPin(QWidget* parent = nullptr);

 public slots:
  // Show tooltip and set position of pin at |point|.
  void popup(const QPoint& point);

 protected:
  void paintEvent(QPaintEvent* event) override;
};

}  // namespace installer

#endif  // INSTALLER_UI_WIDGETS_TOOLTIP_PIN_H
