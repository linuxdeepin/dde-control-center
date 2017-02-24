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
    inline bool isPrimary() const { return m_primary == m_name; }
    inline quint16 rotate() const { return m_rotate; }
    inline double brightness() const { return m_brightness; }
    inline const QRect rect() const { return QRect(m_x, m_y, m_w, m_h); }
    inline const QString name() const { Q_ASSERT(!m_name.isEmpty()); return m_name; }
    inline const QString path() const { return m_path; }
    inline const Resolution currentMode() const { return m_currentMode; }
    inline const QList<quint16> rotateList() const { return m_rotateList; }
    inline const QList<Resolution> modeList() const { return m_modeList; }

signals:
    void geometryChanged() const;
    void xChanged(const int x) const;
    void yChanged(const int y) const;
    void wChanged(const int w) const;
    void hChanged(const int h) const;
    void rotateChanged(const quint16 rotate) const;
    void brightnessChanged(const double brightness) const;
    void currentModeChanged(const Resolution &resolution) const;

private slots:
    void setX(const int x);
    void setY(const int y);
    void setW(const int w);
    void setH(const int h);
    void setPrimary(const QString &primaryName);
    void setRotate(const quint16 rotate);
    void setBrightness(const double brightness);
    void setName(const QString &name);
    void setPath(const QString &path);
    void setRotateList(const QList<quint16> &rotateList);
    void setCurrentMode(const Resolution &resolution);
    void setModeList(const ResolutionList &modeList);

private:
    int m_x;
    int m_y;
    int m_w;
    int m_h;
    quint16 m_rotate;
    double m_brightness;
    QString m_name;
    QString m_path;
    QString m_primary;
    Resolution m_currentMode;
    QList<quint16> m_rotateList;
    QList<Resolution> m_modeList;
};

} // namespace display

} // namespace dcc

#endif // MONITOR_H
