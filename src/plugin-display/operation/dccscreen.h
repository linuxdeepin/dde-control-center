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
class DccScreenItemPrivate;

class DccScreenItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double brightness READ brightness WRITE setBrightness NOTIFY brightnessChanged FINAL)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged FINAL)
public:
    Q_INVOKABLE bool canBrightness() const;
    double brightness() const;
    void setBrightness(const double brightness);
    QString name() const;

Q_SIGNALS:
    void brightnessChanged();
    void nameChanged();

protected:
    explicit DccScreenItem(QObject *parent = nullptr);
    ~DccScreenItem() override;

    QScopedPointer<DccScreenItemPrivate> d_ptrDccScreenItem;
    Q_DECLARE_PRIVATE_D(d_ptrDccScreenItem, DccScreenItem)
    Q_DISABLE_COPY(DccScreenItem)
    friend class DccScreenItemPrivate;
};

class DccScreen : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged FINAL)
    Q_PROPERTY(bool enable READ enable NOTIFY enableChanged FINAL)
    Q_PROPERTY(int x READ x NOTIFY xChanged FINAL)
    Q_PROPERTY(int y READ y NOTIFY yChanged FINAL)
    Q_PROPERTY(int width READ width NOTIFY widthChanged FINAL)
    Q_PROPERTY(int height READ height NOTIFY heightChanged FINAL)
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
    Q_PROPERTY(qreal scale READ scale WRITE setScale NOTIFY scaleChanged FINAL)
    Q_PROPERTY(qreal maxScale READ maxScale NOTIFY maxScaleChanged FINAL)
    Q_PROPERTY(QList<DccScreenItem *> screenItems READ screenItems NOTIFY screenItemsChanged FINAL)
    Q_PROPERTY(QString wallpaper READ wallpaper NOTIFY wallpaperChanged FINAL)

public:
    QString name() const;
    bool enable() const;
    int x() const;
    int y() const;
    int width() const;
    int height() const;
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
    qreal scale() const;
    void setScale(qreal scale);
    qreal maxScale() const;
    QList<DccScreenItem *> screenItems() const;
    QString wallpaper() const;

Q_SIGNALS:
    void nameChanged();
    void enableChanged();
    void xChanged();
    void yChanged();
    void widthChanged();
    void heightChanged();
    void bestResolutionChanged();
    void currentResolutionChanged();
    void resolutionListChanged();
    void bestRateChanged();
    void currentRateChanged();
    void rateListChanged();
    void rotateChanged();
    void brightnessChanged();
    void currentModeChanged();
    void currentFillModeChanged();
    void availableFillModesChanged();
    void screenChanged();
    void scaleChanged();
    void maxScaleChanged();
    void screenItemsChanged();
    void wallpaperChanged();

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
