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
#include "types/touchscreeninfolist.h"
#include "types/touchscreenmap.h"

#define CUSTOM_MODE 0
#define MERGE_MODE 1
#define EXTEND_MODE 2
#define SINGLE_MODE 3

namespace dcc {

namespace display {

class DisplayWorker;
class DisplayModel : public QObject
{
    Q_OBJECT

public:
    friend class DisplayWorker;

public:
    explicit DisplayModel(QObject *parent = 0);

    double monitorScale(Monitor *moni);
    inline int screenHeight() const { return m_screenHeight; }
    inline int screenWidth() const { return m_screenWidth; }
    inline int displayMode() const { return m_mode; }
    inline double uiScale() const { return m_uiScale; }
    inline double minimumBrightnessScale() const { return m_minimumBrightnessScale; }
    inline const QString primary() const { return m_primary; }
    inline const QList<Monitor *> monitorList() const { return m_monitors; }
    Monitor *primaryMonitor() const;
    inline const QString defaultFillMode() { return "Full"; }

    bool isNightMode() const;
    void setIsNightMode(bool isNightMode);

    bool redshiftIsValid() const;

    inline int adjustCCTMode() const { return m_adjustCCTMode; }
    void setAdjustCCTmode(int mode);

    inline int colorTemperature() const { return m_colorTemperature; }
    void setColorTemperature(int value);

    inline bool autoLightAdjustIsValid() const { return m_AutoLightAdjustIsValid; }

    inline bool isAudtoLightAdjust() const { return m_isAutoLightAdjust; }
    void setAutoLightAdjust(bool);

    inline BrightnessMap brightnessMap() const { return m_brightnessMap; }
    void setBrightnessMap(const BrightnessMap &brightnessMap);

    inline TouchscreenInfoList touchscreenList() const { return m_touchscreenList; }
    void setTouchscreenList(const TouchscreenInfoList &touchscreenList);

    inline TouchscreenMap touchMap() const { return m_touchMap; }
    void setTouchMap(const TouchscreenMap &touchMap);

    inline bool allowEnableMultiScaleRatio() { return m_allowEnableMultiScaleRatio; }
    void setAllowEnableMultiScaleRatio(bool allowEnableMultiScaleRatio);

    inline bool isRefreshRateEnable() const { return m_RefreshRateEnable; }
    void setRefreshRateEnable(bool isEnable);

    inline uint maxBacklightBrightness() const { return m_maxBacklightBrightness; }

    inline bool resolutionRefreshEnable() const { return m_resolutionRefreshEnable; }
    void setResolutionRefreshEnable(const bool enable);

    inline bool brightnessEnable() const { return m_brightnessEnable; }
    void setBrightnessEnable(const bool enable);

    inline bool allSupportFillModes() const { return m_allSupportFillModes; }
    void checkAllSupportFillModes();

Q_SIGNALS:
    void screenHeightChanged(const int h) const;
    void screenWidthChanged(const int w) const;
    void displayModeChanged(const int mode) const;
    void uiScaleChanged(const double scale) const;
    void minimumBrightnessScaleChanged(const double) const;
    void primaryScreenChanged(const QString &primary) const;
    void monitorListChanged() const;
    void nightModeChanged(const bool nightmode) const;
    void redshiftVaildChanged(const bool isvalid) const;
    void autoLightAdjustSettingChanged(bool setting) const;
    void autoLightAdjustVaildChanged(bool isvalid) const;
    void touchscreenListChanged() const;
    void touchscreenMapChanged() const;
    void maxBacklightBrightnessChanged(uint value);
    void adjustCCTmodeChanged(int mode);
    void colorTemperatureChanged(int value);
    void resolutionRefreshEnableChanged(const bool enable);
    void brightnessEnableChanged(const bool enable);

private Q_SLOTS:
    void setScreenHeight(const int h);
    void setScreenWidth(const int w);
    void setDisplayMode(const int mode);
    void setUIScale(const double scale);
    void setMinimumBrightnessScale(const double scale);
    void setPrimary(const QString &primary);
    void setRedshiftIsValid(bool redshiftIsValid);
    void monitorAdded(Monitor *mon);
    void monitorRemoved(Monitor *mon);
    void setAutoLightAdjustIsValid(bool);
    void setmaxBacklightBrightness(const uint value);

private:
    int m_screenHeight;
    int m_screenWidth;
    int m_mode {-1};
    int m_colorTemperature {0}; //当前色温对应的颜色值
    int m_adjustCCTMode {0}; //当前自动调节色温模式   0  不开启      1  自动调节    2 手动调节
    double m_uiScale;
    double m_minimumBrightnessScale;
    QString m_primary;
    QList<Monitor *> m_monitors;
    bool m_redshiftIsValid;
    bool m_RefreshRateEnable {false};
    bool m_isAutoLightAdjust {false};
    bool m_AutoLightAdjustIsValid {false};
    bool m_allowEnableMultiScaleRatio;
    bool m_resolutionRefreshEnable;
    bool m_brightnessEnable;
    BrightnessMap m_brightnessMap;
    TouchscreenInfoList m_touchscreenList;
    TouchscreenMap m_touchMap;
    uint m_maxBacklightBrightness {0};
    bool m_allSupportFillModes;
};

} // namespace display

} // namespace dcc

#endif // DISPLAYMODEL_H
