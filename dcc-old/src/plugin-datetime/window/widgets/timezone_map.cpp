//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "timezone_map.h"

#include <QDebug>
#include <QItemSelectionModel>
#include <QLabel>
#include <QListView>
#include <QMouseEvent>
#include <QVBoxLayout>
#include "basiclistdelegate.h"

#include "file_util.h"
#include "timezone_map_util.h"
#include "popup_menu.h"
#include "tooltip_pin.h"

using namespace installer;

const int kDotVerticalMargin = 2;
const int kZonePinHeight = 30;
const int kZonePinMinimumWidth = 60;

const double kDistanceThreshold = 64.0;
const char kDotFile[] = ":/icons/deepin/builtin/images/indicator_active.png";
const char kTimezoneMapFile[] = ":/icons/deepin/builtin/images/timezone_map_big@1x.svg";

// At absolute position of |zone| on a map with size (map_width, map_height).
QPoint ZoneInfoToPosition(const installer::ZoneInfo &zone, int map_width, int map_height)
{
    // TODO(xushaohua): Call round().
    const int x = int(ConvertLongitudeToX(zone.longitude) * map_width);
    const int y = int(ConvertLatitudeToY(zone.latitude) * map_height);
    return QPoint(x, y);
}

TimezoneMap::TimezoneMap(QWidget *parent)
    : QFrame(parent)
    , current_zone_()
    , total_zones_(GetZoneInfoList())
    , nearest_zones_()
{
    this->setObjectName("timezone_map");
    this->setAccessibleName("timezone_map");
    this->initUI();
    this->initConnections();
}

TimezoneMap::~TimezoneMap()
{
    if (popup_window_) {
        delete popup_window_;
        popup_window_ = nullptr;
    }
}

const QString TimezoneMap::getTimezone() const
{
    return current_zone_.timezone;
}

bool TimezoneMap::setTimezone(const QString &timezone)
{
    nearest_zones_.clear();
    const int index = GetZoneInfoByZone(total_zones_, timezone);
    if (index > -1) {
        // 找到时区并标记到地图上
        current_zone_ = total_zones_.at(index);
        nearest_zones_.append(current_zone_);
        this->remark();
        return true;
    } else {
        // NOTE(xushaohua): "Etc/UTC" can not be set on the map
        qInfo() << "Timezone not found:" << timezone;
    }

    return false;
}

void TimezoneMap::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // Get nearest zones around mouse.
        nearest_zones_ = GetNearestZones(total_zones_, kDistanceThreshold,
                                         event->x(), event->y(),
                                         this->width(), this->height());
        qDebug() << nearest_zones_;
        current_zone_ = nearest_zones_.first();
        if (nearest_zones_.length() == 1) {
            // 单个时区
            this->remark();
        } else {
            this->popupZoneWindow(event->pos());
        }
        Q_EMIT this->timezoneUpdated(current_zone_.timezone);
    } else {
        QWidget::mousePressEvent(event);
    }
}

void TimezoneMap::resizeEvent(QResizeEvent *event)
{
    if (popup_window_->isVisible()) {
        dot_->hide();
        popup_window_->hide();
    }

    if (!nearest_zones_.isEmpty()) {
        this->remark();
    }
    QLabel *background_label = findChild<QLabel *>("background_label");
    if (background_label) {
        QPixmap timezone_pixmap = loadPixmap(kTimezoneMapFile);
        background_label->setPixmap(timezone_pixmap.scaled(event->size() * devicePixelRatioF(), Qt::KeepAspectRatio, Qt::FastTransformation));
    }

    QWidget::resizeEvent(event);
}

void TimezoneMap::initConnections()
{
    // Hide dot when popup-zones window is hidden.
    connect(popup_window_, &PopupMenu::onHide,
            dot_, &QLabel::hide);

    // Hide popup_window_ and mark new timezone on map.
    connect(popup_window_, &PopupMenu::menuActivated,
            this, &TimezoneMap::onPopupWindowActivated);
}

void TimezoneMap::initUI()
{
    QLabel *background_label = new QLabel(this);
    background_label->setObjectName("background_label");
    QPixmap timezone_pixmap = loadPixmap(kTimezoneMapFile);
    Q_ASSERT(!timezone_pixmap.isNull());
    background_label->setPixmap(timezone_pixmap);

    // Set parent widget of dot_ to SystemInfoTimezoneFrame.
    dot_ = new QLabel(this->parentWidget());
    const QPixmap dot_pixmap(kDotFile);
    Q_ASSERT(!dot_pixmap.isNull());
    dot_->setPixmap(dot_pixmap);
    dot_->setFixedSize(dot_pixmap.size());
    dot_->hide();

    // Set parent widget of zone_pin_ to SystemInfoTimezoneFrame.
    zone_pin_ = new TooltipPin(this->parentWidget());
    zone_pin_->setFixedHeight(kZonePinHeight);
    zone_pin_->setMinimumWidth(kZonePinMinimumWidth);
    // Allow mouse event to pass through.
    zone_pin_->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    zone_pin_->hide();

    popup_window_ = new PopupMenu();
    popup_window_->hide();

    this->setContentsMargins(0, 0, 0, 0);
}

void TimezoneMap::popupZoneWindow(const QPoint &pos)
{
    // Hide all marks first.
    dot_->hide();
    zone_pin_->hide();
    popup_window_->hide();

    // Popup zone list window.
    const QString locale = QLocale::system().name();
    QStringList zone_names;
    for (const ZoneInfo &zone : nearest_zones_) {
        zone_names.append(GetLocalTimezoneName(zone.timezone, locale));
    }

    // Show popup window above dot
    popup_window_->setStringList(zone_names);
    const int dy = pos.y() - dot_->height() - kDotVerticalMargin;
    const QPoint popup_window_pos = this->mapToGlobal(QPoint(pos.x(), dy));
    popup_window_->popup(popup_window_pos);

    const QPoint dot_relative_pos(pos.x() - dot_->width() / 2,
                                  pos.y() - dot_->height() / 2);
    const QPoint dot_pos(this->mapToParent(dot_relative_pos));
    dot_->move(dot_pos);
    dot_->show();
}

void TimezoneMap::remark()
{
    // Hide all marks first.
    dot_->hide();
    zone_pin_->hide();
    popup_window_->hide();

    const int map_width = this->width();
    const int map_height = this->height();

    Q_ASSERT(!nearest_zones_.isEmpty());
    const QString locale = QLocale::system().name();
    if (!nearest_zones_.isEmpty()) {
        zone_pin_->setText(GetLocalTimezoneName(current_zone_.timezone, locale));

        // Adjust size of pin to fit its content.
        zone_pin_->adjustSize();

        // Show zone pin at current marked zone.
        const QPoint zone_pos = ZoneInfoToPosition(current_zone_, map_width,
                                                   map_height);
        const int zone_dy = zone_pos.y() - dot_->height() / 2 - kDotVerticalMargin;
        const QPoint zone_pin_relative_pos(zone_pos.x(), zone_dy);
        const QPoint zone_pin_pos(this->mapToParent(zone_pin_relative_pos));

        if (zone_pin_pos.x() < 100) {
            // 左侧位置不够，箭头放到左边
            zone_pin_->setArrowDirection(TooltipPin::ArrowLeft);
        } else {
            zone_pin_->setArrowDirection(TooltipPin::ArrowDown);
        }
        zone_pin_->popup(zone_pin_pos);

        const QPoint dot_relative_pos(zone_pos.x() - dot_->width() / 2,
                                      zone_pos.y() - dot_->height() / 2);
        const QPoint dot_pos(this->mapToParent(dot_relative_pos));
        dot_->move(dot_pos);
        dot_->show();
    }
}

// 鼠标点击位置有多个时区，弹出菜单选择后
void TimezoneMap::onPopupWindowActivated(int index)
{
    // Hide popup window and dot first.
    popup_window_->hide();
    dot_->hide();

    // Update current selected timezone and mark it on map.
    Q_ASSERT(index < nearest_zones_.length());
    if (index < nearest_zones_.length()) {
        current_zone_ = nearest_zones_.at(index);
        this->remark();
        Q_EMIT this->timezoneUpdated(current_zone_.timezone);
    }
}
