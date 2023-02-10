//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DISPLAYMODEL_H
#define DISPLAYMODEL_H

#include "math.h"

#include <QObject>
#include <QDebug>

#include "interface/namespace.h"
#include "monitor.h"
#include "machine.h"
#include "types/brightnessmap.h"
#include "types/touchscreeninfolist_v2.h"
#include "types/touchscreenmap.h"

#define CUSTOM_MODE 0
#define MERGE_MODE 1
#define EXTEND_MODE 2
#define SINGLE_MODE 3

namespace DCC_NAMESPACE {

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
    inline const QList<Machine *> machineList() const { return m_machines; }
    Monitor *primaryMonitor() const;
    inline const QString defaultFillMode() { return "None"; }

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

    inline TouchscreenInfoList_V2 touchscreenList() const { return m_touchscreenList; }
    void setTouchscreenList(const TouchscreenInfoList_V2 &touchscreenList);

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

    inline bool DeviceSharingSwitch() const { return m_deviceSharingSwitch; }
    void setDeviceSharingSwitch(const bool enable);

    inline bool allSupportFillModes() const { return m_allSupportFillModes; }
    void checkAllSupportFillModes();

    inline bool SharedDevices() const { return m_openSharedDevices; }
    void setOpenSharedDevices(bool open);

    inline bool SharedClipboard() const { return m_openSharedClipboard; }
    void setOpenSharedClipboard(bool open);

    inline QString filesStoragePath() const { return m_filesStoragePath; }
    void setFilesStoragePath(const QString& path);

Q_SIGNALS:
    void screenHeightChanged(const int h) const;
    void screenWidthChanged(const int w) const;
    void displayModeChanged(const int mode) const;
    void uiScaleChanged(const double scale) const;
    void minimumBrightnessScaleChanged(const double) const;
    void primaryScreenChanged(const QString &primary) const;
    void monitorListChanged() const;
    void machinesListChanged() const;
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
    void deviceSharingSwitchChanged(const bool enable);
    void sharedClipboardChanged(bool on) const;
    void sharedDevicesChanged(bool on) const;
    void filesStoragePathChanged(const QString& path) const;

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
    void machinesAdded(Machine *mon);
    void machinesRemoved(Machine *mon);
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
    QList<Machine *> m_machines;
    bool m_redshiftIsValid;
    bool m_RefreshRateEnable {false};
    bool m_isAutoLightAdjust {false};
    bool m_AutoLightAdjustIsValid {false};
    bool m_allowEnableMultiScaleRatio;
    bool m_resolutionRefreshEnable;
    bool m_brightnessEnable;
    BrightnessMap m_brightnessMap;
    TouchscreenInfoList_V2 m_touchscreenList;
    TouchscreenMap m_touchMap;
    uint m_maxBacklightBrightness {0};
    bool m_allSupportFillModes;
    bool m_deviceSharingSwitch;
    bool m_openSharedDevices;
    bool m_openSharedClipboard;
    QString m_filesStoragePath;
};
}

#endif // DISPLAYMODEL_H
