/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#ifndef MONITOR_H
#define MONITOR_H

#include <QObject>

#include <com_deepin_daemon_display_monitor.h>

using MonitorInter = com::deepin::daemon::display::Monitor;

namespace dcc {

namespace display {

class DisplayWorker;
class Monitor : public QObject
{
    Q_OBJECT

    friend class DisplayWorker;

public:
    explicit Monitor(QObject *parent = 0);

    inline int x() const { return m_x; }
    inline int y() const { return m_y; }
    inline int w() const { return m_w; }
    inline int h() const { return m_h; }
    inline int mmWidth() const { return m_mmWidth; }
    inline int mmHeight() const { return m_mmHeight; }
    inline double scale() const { return m_scale; }
    inline bool isPrimary() const { return m_primary == m_name; }
    inline quint16 rotate() const { return m_rotate; }
    inline double brightness() const { return m_brightness; }
    inline const QRect rect() const { return QRect(m_x, m_y, m_w, m_h); }
    inline const QString name() const { Q_ASSERT(!m_name.isEmpty()); return m_name; }
    inline const bool canBrightness() const { return m_canBrightness; }
    inline const QString path() const { return m_path; }
    inline const Resolution currentMode() const { return m_currentMode; }
    inline const QList<quint16> rotateList() const { return m_rotateList; }
    inline const QList<Resolution> modeList() const { return m_modeList; }
    inline bool enable() const { return m_enable; }
    inline QPoint getLastPoint() {return m_lastPoint; }
    void setLastPoint(int x, int y) {
        m_lastPoint.setX(x);
        m_lastPoint.setY(y);
    }

Q_SIGNALS:
    void geometryChanged() const;
    void xChanged(const int x) const;
    void yChanged(const int y) const;
    void wChanged(const int w) const;
    void hChanged(const int h) const;
    void scaleChanged(const double scale) const;
    void rotateChanged(const quint16 rotate) const;
    void brightnessChanged(const double brightness) const;
    void currentModeChanged(const Resolution &resolution) const;
    void modelListChanged(const QList<Resolution> &resolution) const;
    void enableChanged(bool enable) const;

public:
    static bool isSameResolution(const Resolution &r1,const Resolution &r2);
    static bool isSameRatefresh(const Resolution &r1,const Resolution &r2);
    bool hasResolution(const Resolution &r);
    bool hasResolutionAndRate(const Resolution &r);

private Q_SLOTS:
    void setX(const int x);
    void setY(const int y);
    void setW(const int w);
    void setH(const int h);
    void setMmWidth(const uint mmWidth);
    void setMmHeight(const uint mmHeight);
    void setScale(const double scale);
    void setPrimary(const QString &primaryName);
    void setRotate(const quint16 rotate);
    void setBrightness(const double brightness);
    void setName(const QString &name);
    void setCanBrightness(bool canBrightness);
    void setPath(const QString &path);
    void setRotateList(const QList<quint16> &rotateList);
    void setCurrentMode(const Resolution &resolution);
    void setModeList(const ResolutionList &modeList);
    void setMonitorEnable(bool enable);

private:
    int m_x;
    int m_y;
    int m_w;
    int m_h;
    QPoint m_lastPoint;
    uint m_mmWidth;
    uint m_mmHeight;
    double m_scale;
    quint16 m_rotate;
    double m_brightness;
    QString m_name;
    QString m_path;
    QString m_primary;
    Resolution m_currentMode;
    QList<quint16> m_rotateList;
//    QList<QPair<int, int>> m_resolutionList;
//    QList<double> m_refreshList;
    QList<Resolution> m_modeList;
    bool m_enable;
    bool m_canBrightness;
};

} // namespace display

} // namespace dcc

#endif // MONITOR_H
