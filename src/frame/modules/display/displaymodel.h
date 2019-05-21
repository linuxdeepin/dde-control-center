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

#ifndef DISPLAYMODEL_H
#define DISPLAYMODEL_H

#include <QObject>

#include "monitor.h"
#include "types/brightnessmap.h"

namespace dcc {

namespace display {

class DisplayWorker;
class DisplayModel : public QObject
{
    Q_OBJECT

public:
    static const QString DDE_Display_Config;

    friend class DisplayWorker;

public:
    explicit DisplayModel(QObject *parent = 0);

    inline int screenHeight() const { return m_screenHeight; }
    inline int screenWidth() const { return m_screenWidth; }
    inline int displayMode() const { return m_mode; }
    inline double uiScale() const { return m_uiScale; }
    inline double minimumBrightnessScale() const { return m_minimumBrightnessScale; }
    inline const QString primary() const { Q_ASSERT(!m_primary.isEmpty()); return m_primary; }
    inline const QString config() const { return m_currentConfig; }
    inline const QStringList configList() const { return m_configList; }
    inline const QList<Monitor *> monitorList() const { return m_monitors; }
    const QList<Resolution> monitorsSameModeList() const;
    Monitor *primaryMonitor() const;

    bool monitorsIsIntersect() const;

    bool isNightMode() const;
    void setIsNightMode(bool isNightMode);

    bool redshiftIsValid() const;

    inline bool redshiftSetting() const { return m_redshiftSetting; }
    void setRedshiftSetting(bool redshiftSetting);

    inline BrightnessMap brightnessMap() const { return m_brightnessMap; }
    void setBrightnessMap(const BrightnessMap &brightnessMap);

    inline std::pair<int, QString> lastConfig() const { return m_lastConfig; }
    void setLastConfig(const std::pair<int, QString> &lastConfig);

    inline bool allowEnableMultiScaleRatio() { return m_allowEnableMultiScaleRatio; }
    void setAllowEnableMultiScaleRatio(bool allowEnableMultiScaleRatio);

    inline bool isMerge() const { return m_isMerged; }
    void setIsMerge(bool isMerge);

Q_SIGNALS:
    void screenHeightChanged(const int h) const;
    void screenWidthChanged(const int w) const;
    void displayModeChanged(const int mode) const;
    void uiScaleChanged(const double scale) const;
    void minimumBrightnessScaleChanged(const double) const;
    void primaryScreenChanged(const QString &primary) const;
    void currentConfigChanged(const QString &config) const;
    void configCreated(const QString &config) const;
    void configListChanged(const QStringList &configs) const;
    void monitorListChanged() const;
    void nightModeChanged(const bool nightmode) const;
    void redshiftVaildChanged(const bool isvalid) const;
    void redshiftSettingChanged(const bool setting) const;

private Q_SLOTS:
    void setScreenHeight(const int h);
    void setScreenWidth(const int w);
    void setDisplayMode(const int mode);
    void setUIScale(const double scale);
    void setMinimumBrightnessScale(const double scale);
    void setPrimary(const QString &primary);
    void setCurrentConfig(const QString &config);
    void setConfigList(const QStringList &configList);
    void setRedshiftIsValid(bool redshiftIsValid);
    void monitorAdded(Monitor *mon);
    void monitorRemoved(Monitor *mon);

private:
    int m_screenHeight;
    int m_screenWidth;
    int m_mode;
    double m_uiScale;
    double m_minimumBrightnessScale;
    QString m_primary;
    QString m_currentConfig;
    QStringList m_configList;
    QList<Monitor *> m_monitors;
    bool m_isNightMode;
    bool m_redshiftIsValid;
    bool m_redshiftSetting;
    bool m_allowEnableMultiScaleRatio;
    bool m_isMerged;
    BrightnessMap m_brightnessMap;
    std::pair<int, QString> m_lastConfig;
};

} // namespace display

} // namespace dcc

#endif // DISPLAYMODEL_H
