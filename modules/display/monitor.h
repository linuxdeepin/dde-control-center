#ifndef MONITOR_H
#define MONITOR_H

#include <QLabel>

class Monitor : public QFrame
{
    Q_OBJECT

    Q_PROPERTY(QColor dockBgColor READ dockBgColor WRITE setDockBgColor)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(Qt::Alignment nameAlignment READ nameAlignment WRITE setNameAlignment)
    Q_PROPERTY(Monitor* brother READ brother WRITE setBrother)
    Q_PROPERTY(const Monitor* child READ child)
    Q_PROPERTY(bool draggable READ draggable WRITE setDraggable)
    Q_PROPERTY(bool draging READ draging NOTIFY dragingChanged)
    Q_PROPERTY(bool eyeing READ eyeing)
    Q_PROPERTY(bool primary READ primary WRITE setPrimary)

public:
    explicit Monitor(QWidget *parent = 0);

    void setName(QString name);

    QSize resolution();
    void setResolution(int width, int height);
    QColor dockBgColor() const;
    bool draggable() const;
    QString name() const;
    Qt::Alignment nameAlignment() const;
    Monitor* brother() const;
    bool draging() const;
    bool eyeing() const;
    bool primary() const;
    const Monitor* child() const;

public slots:
    void setDockBgColor(QColor dockBgColor);
    void setDraggable(bool draggable);
    void setAlignment(Qt::Alignment aalignment);
    void setNameAlignment(Qt::Alignment nameAlignment);
    void setBrother(Monitor* brother);
    void setPrimary(bool primary);
    bool dragEnter(Monitor *e);
    void dragLeave();
    bool drop(Monitor *e);
    Monitor* split();

signals:
    void dragingChanged(bool draging);
    void mousePressed(QPoint pos);
    void mouseMoveing(QPoint pos);
    void mouseRelease(QPoint pos);

protected:
    void paintEvent(QPaintEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

private:
    QSize m_resolution;
    QColor m_dockBgColor;
    bool m_draggable;
    QPoint m_oldPos;
    QString m_name;
    Qt::Alignment m_nameAlignment;
    Monitor *m_brother;
    bool m_draging;
    bool m_eyeing;
    bool m_primary;
    Monitor* m_child;

    void setDraging(bool arg);
    void setEyeing(bool arg);
};

#endif // MONITOR_H
