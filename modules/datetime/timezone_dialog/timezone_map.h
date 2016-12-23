// Copyright (c) 2016 Deepin Ltd. All rights reserved.
// Use of this source is governed by General Public License that can be found
// in the LICENSE file.

#ifndef INSTALLER_UI_WIDGETS_TIMEZONE_MAP_H
#define INSTALLER_UI_WIDGETS_TIMEZONE_MAP_H

#include <QFrame>
class QLabel;
class QListView;
class QResizeEvent;
class QStringListModel;

#include "timezone.h"

namespace installer {

class PopupMenu;
class TooltipPin;

// Draw timezone map and bubble.
class TimezoneMap : public QFrame {
  Q_OBJECT

 public:
  explicit TimezoneMap(QWidget* parent = nullptr);
  ~TimezoneMap();

  // Get current selected timezone, might be empty.
  const QString getTimezone() const;

 signals:
  void timezoneUpdated(const QString& timezone);

 public slots:
  // Remark |timezone| on map.
  void setTimezone(const QString& timezone);

 protected:
  void mousePressEvent(QMouseEvent* event) override;

  // Hide tooltips when window is resized.
  void resizeEvent(QResizeEvent* event) override;

 private:
  void initConnections();
  void initUI();

  // Popup zone window at |pos|.
  void popupZoneWindow(const QPoint& pos);

  // Mark current zone on the map.
  void remark();

  // Currently selected/marked timezone.
  ZoneInfo current_zone_;

  // A list of zone info found in system.
  const ZoneInfoList total_zones_;

  // A list of zone info which are near enough to current cursor position.
  ZoneInfoList nearest_zones_;

  // A round dot to indicate position on the map.
  QLabel* dot_ = nullptr;

  // To mark a zone on map.
  TooltipPin* zone_pin_ = nullptr;

  // To display a list of zones on map.
  PopupMenu* popup_window_ = nullptr;

 private slots:
  void onPopupWindowActivated(int index);
};

}  // namespace installer

#endif  // INSTALLER_UI_WIDGETS_TIMEZONE_MAP_H
