//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
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
};

// Draw timezone map and bubble.
class TimezoneMap : public QFrame
{
    Q_OBJECT

public:
    explicit TimezoneMap(QWidget *parent = nullptr);
    ~TimezoneMap();

    // Get current selected timezone, might be empty.
    const QString getTimezone() const;

Q_SIGNALS:
    void timezoneUpdated(const QString &timezone);

public Q_SLOTS:
    // Remark |timezone| on map.
    bool setTimezone(const QString &timezone);

protected:
    void mousePressEvent(QMouseEvent *event) override;

    // Hide tooltips when window is resized.
    void resizeEvent(QResizeEvent *event) override;

private:
    void initConnections();
    void initUI();

    // Popup zone window at |pos|.
    void popupZoneWindow(const QPoint &pos);

    // Mark current zone on the map.
    void remark();

    // Currently selected/marked timezone.
    installer::ZoneInfo current_zone_;

    // A list of zone info found in system.
    const installer::ZoneInfoList total_zones_;

    // A list of zone info which are near enough to current cursor position.
    installer::ZoneInfoList nearest_zones_;

    // A round dot to indicate position on the map.
    QLabel *dot_ = nullptr;

    // To mark a zone on map.
    installer::TooltipPin *zone_pin_ = nullptr;

    // To display a list of zones on map.
    installer::PopupMenu *popup_window_ = nullptr;

private Q_SLOTS:
    void onPopupWindowActivated(int index);
};

#endif // INSTALLER_UI_WIDGETS_TIMEZONE_MAP_H
