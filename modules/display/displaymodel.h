#ifndef DISPLAYMODEL_H
#define DISPLAYMODEL_H

#include <QObject>
#include <QDBusObjectPath>

#include "displayworker.h"
#include "monitor.h"

class DisplayModel : public QObject
{
    Q_OBJECT

    friend class DisplayWorker;

public:
    explicit DisplayModel(QObject *parent = 0);

    int screenHeight() const { return m_screenHeight; }
    int screenWidth() const { return m_screenWidth; }
    const QString primary() const { return m_primary; }
    const QList<Monitor *> monitorList() const { return m_monitors; }
    Monitor *primaryMonitor() const;

    bool monitorsIsIntersect() const;

signals:
    void screenHeightChanged(const int h) const;
    void screenWidthChanged(const int w) const;
    void primaryScreenChanged(const QString &primary) const;
    void monitorListChanged() const;

private slots:
    void setScreenHeight(const int h);
    void setScreenWidth(const int w);
    void setPrimary(const QString &primary);
    void monitorAdded(Monitor *mon);

private:
    int m_screenHeight;
    int m_screenWidth;
    QString m_primary;
    QList<Monitor *> m_monitors;
};

#endif // DISPLAYMODEL_H
