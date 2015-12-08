#ifndef MONITOR_H
#define MONITOR_H

#include <QLabel>

class MonitorInterface;
class Monitor : public QFrame
{
    Q_OBJECT

    Q_PROPERTY(QColor dockBgColor READ dockBgColor WRITE setDockBgColor)
    Q_PROPERTY(QColor childBorderColor READ childBorderColor WRITE setChildBorderColor)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(Qt::Alignment nameAlignment READ nameAlignment WRITE setNameAlignment)
    Q_PROPERTY(const Monitor* child READ child)
    Q_PROPERTY(bool draggable READ draggable WRITE setDraggable)
    Q_PROPERTY(bool draging READ draging NOTIFY dragingChanged)
    Q_PROPERTY(bool eyeing READ eyeing NOTIFY eyeingChanged)
    Q_PROPERTY(bool isPrimary READ isPrimary WRITE setIsPrimary)

public:
    explicit Monitor(MonitorInterface *dbus, QWidget *parent = 0);

    void setName(QString name);

    QRect resolution() const;
    MonitorInterface *dbusInterface() const;
    QColor dockBgColor() const;
    QColor childBorderColor() const;
    bool draggable() const;
    QString name() const;
    Qt::Alignment nameAlignment() const;
    bool draging() const;
    bool eyeing() const;
    bool isPrimary() const;
    const Monitor* child() const;

    QRect parentRect() const;
    void setParentRect(const QRect &rect);

    QPoint mapToRealPoint() const;

public slots:
    void setResolution(const QRect &rect);
    void setDockBgColor(QColor dockBgColor);
    void setChildBorderColor(QColor childBorderColor);
    void setDraggable(bool draggable);
    void setAlignment(Qt::Alignment aalignment);
    void setNameAlignment(Qt::Alignment nameAlignment);
    void setIsPrimary(bool isPrimary);
    bool dragEnter(Monitor *e);
    void dragLeave();
    bool drop(Monitor *e);
    Monitor* split();
    void applyPostion();
    void resetResolution();

signals:
    void dragingChanged(bool draging);
    void mousePressed(QPoint pos);
    void mouseMoveing(QPoint pos);
    void mouseRelease(QPoint pos);
    void eyeingChanged(bool eyeing);
    void resolutionChanged(QRect rect);

protected:
    void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;

private:
    MonitorInterface *m_dbusInterface;
    QColor m_dockBgColor;
    QColor m_childBorderColor;
    bool m_draggable;
    QPoint m_pressPos;
    QPoint m_oldPos;
    QString m_name;
    Qt::Alignment m_nameAlignment;
    bool m_draging;
    bool m_eyeing;
    bool m_isPrimary;
    Monitor* m_child;
    QRect m_parentRect;
    QRect m_resolution;

    void setDraging(bool arg);
    void setEyeing(bool arg);
};

#endif // MONITOR_H
