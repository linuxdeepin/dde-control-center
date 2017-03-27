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
    const QString primary() const { Q_ASSERT(!m_primary.isEmpty()); return m_primary; }
    const QString config() const { return m_currentConfig; }
    const QStringList configList() const { return m_configList; }
    const QList<Monitor *> monitorList() const { return m_monitors; }
    const QList<Resolution> monitorsSameModeList() const;
    Monitor *primaryMonitor() const;

    bool monitorsIsIntersect() const;

signals:
    void screenHeightChanged(const int h) const;
    void screenWidthChanged(const int w) const;
    void displayModeChanged(const int mode) const;
    void primaryScreenChanged(const QString &primary) const;
    void currentConfigChanged(const QString &config) const;
    void firstConfigCreated(const QString &config) const;
    void configListChanged(const QStringList &configs) const;
    void monitorListChanged() const;

private slots:
    void setScreenHeight(const int h);
    void setScreenWidth(const int w);
    void setDisplayMode(const int mode);
    void setPrimary(const QString &primary);
    void setCurrentConfig(const QString &config);
    void setConfigList(const QStringList &configList);
    void monitorAdded(Monitor *mon);
    void monitorRemoved(Monitor *mon);

private:
    int m_screenHeight;
    int m_screenWidth;
    int m_mode;
    QString m_primary;
    QString m_currentConfig;
    QStringList m_configList;
    QList<Monitor *> m_monitors;
};

} // namespace display

} // namespace dcc

#endif // DISPLAYMODEL_H
