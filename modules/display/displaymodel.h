#ifndef DISPLAYMODEL_H
#define DISPLAYMODEL_H

#include <QObject>

#include "monitor.h"

namespace dcc {

namespace display {

class DisplayWorker;
class DisplayModel : public QObject
{
    Q_OBJECT

    friend class DisplayWorker;

public:
    explicit DisplayModel(QObject *parent = 0);

    int screenHeight() const { return m_screenHeight; }
    int screenWidth() const { return m_screenWidth; }
    int displayMode() const { return m_mode; }
    bool hasConfig() const { return m_hasConfig; }
    const QString primary() const { return m_primary; }
    const QList<Monitor *> monitorList() const { return m_monitors; }
    const QList<Resolution> monitorsSameModeList() const;
    Monitor *primaryMonitor() const;

    bool monitorsIsIntersect() const;

signals:
    void screenHeightChanged(const int h) const;
    void screenWidthChanged(const int w) const;
    void displayModeChanged(const int mode) const;
    void hasConfigChanged(const bool config) const;
    void primaryScreenChanged(const QString &primary) const;
    void monitorListChanged() const;

private slots:
    void setScreenHeight(const int h);
    void setScreenWidth(const int w);
    void setDisplayMode(const int mode);
    void setHasConfig(const bool config);
    void setPrimary(const QString &primary);
    void monitorAdded(Monitor *mon);
    void monitorRemoved(Monitor *mon);

private:
    int m_screenHeight;
    int m_screenWidth;
    int m_mode;
    bool m_hasConfig;
    QString m_primary;
    QList<Monitor *> m_monitors;
};

} // namespace display

} // namespace dcc

#endif // DISPLAYMODEL_H
