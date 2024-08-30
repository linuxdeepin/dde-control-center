//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MONITOR_H
#define MONITOR_H

#include "interface/namespace.h"
#include "monitordbusproxy.h"

#include <QObject>
#include <QScreen>

#include <optional>

namespace DCC_NAMESPACE {

class DisplayWorker;
class TouchscreenWorker;
class Monitor : public QObject
{
    Q_OBJECT
    friend class DisplayWorker;
    friend class TouchscreenWorker;

public:
    enum RotateMode {
        Normal,
        Gravity,
        Rotate
    };

public:
    explicit Monitor(QObject *parent = 0);

    inline int x() const { return m_x; }
    inline int y() const { return m_y; }
    inline int w() const { return m_w; }
    inline int h() const { return m_h; }
    inline int mmWidth() const { return m_mmWidth; }
    inline int mmHeight() const { return m_mmHeight; }
    inline double scale() const { return m_scale; }
    inline bool isPrimary() const { return m_primary == m_name; }
    inline quint16 rotate() const { return m_rotate; }
    inline double brightness() const { return m_brightness; }
    inline const QRect rect() const { return QRect(m_x, m_y, m_w, m_h); }
    inline const QString name() const
    {
        Q_ASSERT(!m_name.isEmpty());
        return m_name;
    }
    inline const QString manufacturer() const
    {
        Q_ASSERT(!m_manufacturer.isEmpty());
        return m_manufacturer;
    }
    inline const QString model() const
    {
        Q_ASSERT(!m_model.isEmpty());
        return m_model;
    }
    inline bool canBrightness() const { return m_canBrightness; }
    inline const QString path() const { return m_path; }
    inline const Resolution currentMode() const { return m_currentMode; }
    inline const QList<quint16> rotateList() const { return m_rotateList; }
    inline const QList<Resolution> modeList() const { return m_modeList; }
    inline bool enable() const { return m_enable; }
    inline QStringList availableFillModes() const { return m_fillModeList; }
    inline QString currentFillMode() const { return m_currentFillMode; }
    inline const Resolution bestMode() const { return m_bestMode; }
    inline RotateMode currentRotateMode() const { return m_screenSensingMode; }

Q_SIGNALS:
    void geometryChanged() const;
    void xChanged(const int x) const;
    void yChanged(const int y) const;
    void wChanged(const int w) const;
    void hChanged(const int h) const;
    void scaleChanged(const double scale) const;
    void rotateChanged(const quint16 rotate) const;
    void brightnessChanged(const double brightness) const;
    void currentModeChanged(const Resolution &resolution) const;
    void modelListChanged(const QList<Resolution> &resolution) const;
    void enableChanged(bool enable) const;
    void bestModeChanged() const;
    void availableFillModesChanged(const QStringList &fillModeList);
    // TODO: 重力旋转
    void currentRotateModeChanged() const;
    void currentFillModeChanged(QString currentFillMode) const;

public:
    static bool isSameResolution(const Resolution &r1, const Resolution &r2);
    static bool isSameRatefresh(const Resolution &r1, const Resolution &r2);
    bool hasResolution(const Resolution &r);
    bool hasResolutionAndRate(const Resolution &r);
    bool hasRatefresh(const double r);
    QScreen *getQScreen();

private Q_SLOTS:
    void setX(const int x);
    void setY(const int y);
    void setW(const int w);
    void setH(const int h);
    void setMmWidth(const uint mmWidth);
    void setMmHeight(const uint mmHeight);
    void setScale(const double scale);
    void setPrimary(const QString &primaryName);
    void setRotate(const quint16 rotate);
    void setBrightness(const double brightness);
    void setName(const QString &name);
    void setManufacturer(const QString &manufacturer);
    void setModel(const QString &model);
    void setCanBrightness(bool canBrightness);
    void setPath(const QString &path);
    void setRotateList(const QList<quint16> &rotateList);
    void setCurrentMode(const Resolution &resolution);
    void setModeList(const ResolutionList &modeList);
    void setMonitorEnable(bool enable);
    void setBestMode(const Resolution &mode);
    void setCurrentRotateMode(const unsigned char mode);
    void setAvailableFillModes(const QStringList &fillModeList);
    void setCurrentFillMode(const QString currentFillMode);
    std::optional<Resolution> getResolutionById(quint32 id);

private:
    int m_x;
    int m_y;
    int m_w;
    int m_h;
    uint m_mmWidth;
    uint m_mmHeight;
    double m_scale;
    quint16 m_rotate;
    double m_brightness;
    QString m_name;
    QString m_manufacturer;
    QString m_model;
    QString m_path;
    QString m_primary;
    Resolution m_currentMode;
    QList<quint16> m_rotateList;
    QList<Resolution> m_modeList;
    bool m_enable;
    bool m_canBrightness;
    Resolution m_bestMode;
    RotateMode m_screenSensingMode;
    QStringList m_fillModeList;
    QString m_currentFillMode;
};
}

#endif // MONITOR_H
