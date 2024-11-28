// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DCCSCREEN_H
#define DCCSCREEN_H

#include <QMetaType>
#include <QObject>
#include <QScreen>
#include <QSize>

namespace dccV25 {
class DccScreenPrivate;

class DccScreen : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged FINAL)
    Q_PROPERTY(bool enable READ enable NOTIFY enableChanged FINAL)
    Q_PROPERTY(int x READ x NOTIFY xChanged FINAL)
    Q_PROPERTY(int y READ y NOTIFY yChanged FINAL)
    Q_PROPERTY(QSize bestResolution READ bestResolution NOTIFY bestResolutionChanged FINAL)
    Q_PROPERTY(QSize currentResolution READ currentResolution WRITE setCurrentResolution NOTIFY currentResolutionChanged FINAL)
    Q_PROPERTY(QList<QSize> resolutionList READ resolutionList NOTIFY resolutionListChanged FINAL)
    Q_PROPERTY(double bestRate READ bestRate NOTIFY bestRateChanged FINAL)
    Q_PROPERTY(double currentRate READ currentRate WRITE setCurrentRate NOTIFY currentRateChanged FINAL)
    Q_PROPERTY(QList<double> rateList READ rateList NOTIFY rateListChanged FINAL)
    Q_PROPERTY(double brightness READ brightness NOTIFY brightnessChanged FINAL)
    Q_PROPERTY(uint rotate READ rotate WRITE setRotate NOTIFY rotateChanged FINAL)
    Q_PROPERTY(QString currentFillMode READ currentFillMode WRITE setCurrentFillMode NOTIFY currentFillModeChanged FINAL)
    Q_PROPERTY(QStringList availableFillModes READ availableFillModes NOTIFY availableFillModesChanged FINAL)
    Q_PROPERTY(QScreen* screen READ screen NOTIFY screenChanged FINAL)
public:
    QString name() const;
    bool enable() const;
    int x() const;
    int y() const;
    QSize bestResolution() const;    // 推荐分辨率
    QSize currentResolution() const; // 分辨率
    void setCurrentResolution(const QSize &resolution);
    QList<QSize> resolutionList() const; // 可选分辨率
    double bestRate() const;             // 推荐刷新率
    double currentRate() const;          // 刷新率
    void setCurrentRate(const double &rate);
    QList<double> rateList() const; // 可选刷新率
    uint rotate() const;
    void setRotate(uint rotate);
    double brightness() const;
    QString currentFillMode() const;
    void setCurrentFillMode(const QString &fill);
    QStringList availableFillModes() const;
    QScreen *screen() const;

Q_SIGNALS:
    void nameChanged();
    void enableChanged();
    void xChanged();
    void yChanged();
    void bestResolutionChanged();
    void currentResolutionChanged();
    void resolutionListChanged();
    void bestRateChanged();
    void currentRateChanged();
    void rateListChanged();
    void rotateChanged();
    void brightnessChanged();
    void currentFillModeChanged();
    void availableFillModesChanged();
    void screenChanged();

protected:
    explicit DccScreen(QObject *parent = nullptr);
    ~DccScreen() override;

    QScopedPointer<DccScreenPrivate> d_ptrDccScreen;
    Q_DECLARE_PRIVATE_D(d_ptrDccScreen, DccScreen)
    Q_DISABLE_COPY(DccScreen)
    friend class DccScreenPrivate;
};
} // namespace dccV25
#endif // DCCSCREEN_H
