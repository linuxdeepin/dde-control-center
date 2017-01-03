// Copyright (c) 2016 Deepin Ltd. All rights reserved.
// Use of this source is governed by General Public License that can be found
// in the LICENSE file.

#include "timezone_map.h"

#include <QDebug>
#include <QItemSelectionModel>
#include <QLabel>
#include <QListView>
#include <QMouseEvent>
#include <QVBoxLayout>

#include "file_util.h"
#include "timezone_map_util.h"
#include "popup_menu.h"
#include "tooltip_pin.h"

namespace installer {

namespace {

const int kDotVerticalMargin = 2;
const int kZonePinHeight = 30;
const int kZonePinMinimumWidth = 60;

const double kDistanceThreshold = 100.0;
const char kDotFile[] = ":/timezone_dialog/images/indicator_active.png";
const char kTimezoneMapFile[] = ":/timezone_dialog/images/timezone_map_big.png";

// At absolute position of |zone| on a map with size (map_width, map_height).
QPoint ZoneInfoToPosition(const ZoneInfo& zone, int map_width, int map_height) {
  // TODO(xushaohua): Call round().
  const int x = int(ConvertLongitudeToX(zone.longitude) * map_width);
  const int y = int(ConvertLatitudeToY(zone.latitude) * map_height);
  return QPoint(x, y);
}

}  // namespace

TimezoneMap::TimezoneMap(QWidget* parent)
    : QFrame(parent),
      current_zone_(),
      total_zones_(GetZoneInfoList()),
      nearest_zones_() {
  this->setObjectName("timezone_map");

  this->initUI();
  this->initConnections();
}

TimezoneMap::~TimezoneMap() {
  if (popup_window_) {
    delete popup_window_;
    popup_window_ = nullptr;
  }
}

const QString TimezoneMap::getTimezone() const {
  return current_zone_.timezone;
}

void TimezoneMap::setTimezone(const QString& timezone) {
  nearest_zones_.clear();
  const int index = GetZoneInfoByZone(total_zones_, timezone);
  if (index > -1) {
    current_zone_ = total_zones_.at(index);
    nearest_zones_.append(current_zone_);
    this->remark();
  } else {
    qWarning() << "Timezone not found:" << timezone;
  }
}

void TimezoneMap::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    nearest_zones_ = GetNearestZones(total_zones_, kDistanceThreshold,
                                     event->x(), event->y(),
                                     this->width(), this->height());
    qDebug() << nearest_zones_;
    if (nearest_zones_.length() == 1) {
      current_zone_ = nearest_zones_.first();
      this->remark();
      emit this->timezoneUpdated(current_zone_.timezone);
    } else {
      this->popupZoneWindow(event->pos());
    }
  } else {
    QWidget::mousePressEvent(event);
  }
}

void TimezoneMap::resizeEvent(QResizeEvent* event) {
    QPixmap timezone_pixmap(kTimezoneMapFile);
    timezone_pixmap = timezone_pixmap.scaled(event->size().width(), event->size().height());
    Q_ASSERT(!timezone_pixmap.isNull());
    background_label_->setPixmap(timezone_pixmap);

  if (popup_window_->isVisible()) {
    dot_->hide();
    popup_window_->hide();
  }
  QWidget::resizeEvent(event);
}

void TimezoneMap::initConnections() {
  // Hide dot when popup-zones window is hidden.
  connect(popup_window_, &PopupMenu::onHide,
          dot_, &QLabel::hide);

  // Hide popup_window_ and mark new timezone on map.
  connect(popup_window_, &PopupMenu::menuActivated,
          this, &TimezoneMap::onPopupWindowActivated);
}

void TimezoneMap::initUI() {
  background_label_ = new QLabel(this);
  background_label_->setObjectName("background_label");

  dot_ = new QLabel(this);
  QPixmap dot_pixmap(kDotFile);
  Q_ASSERT(!dot_pixmap.isNull());
  dot_->setPixmap(dot_pixmap);
  dot_->setFixedSize(dot_pixmap.size());
  dot_->hide();

  zone_pin_ = new TooltipPin(this);
  zone_pin_->setFixedHeight(kZonePinHeight);
  zone_pin_->setMinimumWidth(kZonePinMinimumWidth);
  zone_pin_->hide();

  popup_window_ = new PopupMenu();
  popup_window_->hide();

  this->setContentsMargins(0, 0, 0, 0);
}

void TimezoneMap::popupZoneWindow(const QPoint& pos) {
  // Hide all marks first.
  dot_->hide();
  zone_pin_->hide();
  popup_window_->hide();

  // Popup zone list window.
  QStringList zone_names;
  for (const ZoneInfo& zone : nearest_zones_) {
    zone_names.append(GetTimezoneName(zone.timezone));
  }

  popup_window_->setStringList(zone_names);
  const int dy = pos.y() - dot_->height() - kDotVerticalMargin;
  const QPoint global_pos = this->mapToGlobal(QPoint(pos.x(), dy));
  popup_window_->popup(global_pos);

  dot_->move(pos.x() - dot_->width() / 2, pos.y() - dot_->height() / 2);
  dot_->show();
}

void TimezoneMap::remark() {
  // Hide all marks first.
  dot_->hide();
  zone_pin_->hide();
  popup_window_->hide();

  const int map_width = this->width();
  const int map_height = this->height();

  Q_ASSERT(!nearest_zones_.isEmpty());
  if (!nearest_zones_.isEmpty()) {
    // TODO(xushaohua): Convert timezone to other names.
    zone_pin_->setText(GetTimezoneName(current_zone_.timezone));

    // Adjust size of pin to fit its content.
    zone_pin_->adjustSize();

    // Show zone pin at current marked zone.
    const QPoint point = ZoneInfoToPosition(current_zone_,
                                            map_width,
                                            map_height);
    const int dy = point.y() - dot_->height() / 2 - kDotVerticalMargin;
    zone_pin_->popup(QPoint(point.x(), dy));

    dot_->move(point.x() - dot_->width() / 2,
               point.y() - dot_->height() / 2);
    dot_->show();
  }
}

void TimezoneMap::onPopupWindowActivated(int index) {
  // Hide popup_window_.
  popup_window_->hide();
  dot_->hide();

  // Update current selected timezone and mark it on map.
  Q_ASSERT(index < nearest_zones_.length());
  if (index < nearest_zones_.length()) {
    current_zone_ = nearest_zones_.at(index);
    this->remark();
    emit this->timezoneUpdated(current_zone_.timezone);
  }
}

}  // namespace installer
