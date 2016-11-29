#ifndef MONITOR_H
#define MONITOR_H

#include <QObject>

#include <com_deepin_daemon_display_monitor.h>

using MonitorInter = com::deepin::daemon::display::Monitor;

class DisplayWorker;
class Monitor : public QObject
{
    Q_OBJECT

    friend class DisplayWorker;

public:
    explicit Monitor(QObject *parent = 0);

    int x() const { return m_x; }
    int y() const { return m_y; }
    int w() const { return m_w; }
    int h() const { return m_h; }
    quint16 rotate() const { return m_rotate; }
    const QList<quint16> rotateList() const { return m_rotateList; }
    const QList<Resolution> modeList() const { return m_modeList; }

signals:
    void xChanged(const int x);
    void yChanged(const int y);
    void wChanged(const int w);
    void hChanged(const int h);
    void rotateChanged(const quint16 rotate);

private slots:
    void setX(const int x);
    void setY(const int y);
    void setW(const int w);
    void setH(const int h);
    void setRotate(const quint16 rotate);
    void setRotateList(const QList<quint16> &rotateList);
    void setModeList(const ResolutionList &modeList);

private:
    int m_x;
    int m_y;
    int m_w;
    int m_h;
    quint16 m_rotate;
    QList<quint16> m_rotateList;
    QList<Resolution> m_modeList;
};

#endif // MONITOR_H
