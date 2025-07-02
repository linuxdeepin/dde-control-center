// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DISPLAYMODULE_H
#define DISPLAYMODULE_H

#include <QObject>
QT_BEGIN_NAMESPACE
class QQuickItem;
QT_END_NAMESPACE

namespace dccV25 {
class DccScreen;
class DisplayModulePrivate;

class DisplayModule : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<DccScreen *> virtualScreens READ virtualScreens NOTIFY virtualScreensChanged FINAL)
    Q_PROPERTY(QList<DccScreen *> screens READ screens NOTIFY screensChanged FINAL)
    Q_PROPERTY(DccScreen * primaryScreen READ primaryScreen WRITE setPrimaryScreen NOTIFY primaryScreenChanged FINAL)
    Q_PROPERTY(QString  displayMode READ displayMode WRITE setDisplayMode NOTIFY displayModeChanged FINAL)
    Q_PROPERTY(bool isX11 READ isX11 NOTIFY isX11Changed FINAL)
    Q_PROPERTY(qreal globalScale READ globalScale WRITE setGlobalScale NOTIFY globalScaleChanged FINAL)
    Q_PROPERTY(qreal maxGlobalScale READ maxGlobalScale NOTIFY maxGlobalScaleChanged FINAL)
    Q_PROPERTY(bool colorTemperatureEnabled READ colorTemperatureEnabled WRITE setColorTemperatureEnabled NOTIFY colorTemperatureEnabledChanged FINAL)
    Q_PROPERTY(int colorTemperatureMode READ colorTemperatureMode WRITE setColorTemperatureMode NOTIFY colorTemperatureModeChanged FINAL)
    Q_PROPERTY(int colorTemperature READ colorTemperature WRITE setColorTemperature NOTIFY colorTemperatureChanged FINAL)
    Q_PROPERTY(QString customColorTempTimePeriod READ customColorTempTimePeriod WRITE setCustomColorTempTimePeriod NOTIFY customColorTempTimePeriodChanged FINAL)
public:
    explicit DisplayModule(QObject *parent = nullptr);
    ~DisplayModule() override;

    // 逻辑屏幕 去除禁用的
    QList<DccScreen *> virtualScreens() const;
    // 物理屏幕
    QList<DccScreen *> screens() const;
    DccScreen *primaryScreen() const; // 关联screens
    void setPrimaryScreen(DccScreen *primary);
    QString displayMode() const;
    void setDisplayMode(const QString &mode);
    bool isX11() const;
    qreal globalScale() const;
    void setGlobalScale(qreal scale);
    qreal maxGlobalScale() const;
    bool colorTemperatureEnabled() const;
    void setColorTemperatureEnabled(bool enabled);
    int colorTemperatureMode() const;
    void setColorTemperatureMode(int mode);
    int colorTemperature() const;
    void setColorTemperature(int pos);
    QString customColorTempTimePeriod() const;
    void setCustomColorTempTimePeriod(const QString &timePeriod);

public Q_SLOTS:
    void saveChanges();
    void resetBackup();

    void adsorptionScreen(QList<QObject *> listItems, QObject *pw, qreal scale);
    void executemultiScreenAlgo(QList<QObject *> listItems, QObject *pw, qreal scale);
    void applySettings(QList<QObject *> listItems, qreal scale);
    void applyChanged(); // 修改分辨率、方向时，要重新处理下拼接

Q_SIGNALS:
    void virtualScreensChanged();
    void screensChanged();
    void primaryScreenChanged();
    void displayModeChanged();
    void isX11Changed();
    void globalScaleChanged();
    void globalScaleEnabledChanged();
    void maxGlobalScaleChanged();
    void colorTemperatureEnabledChanged();
    void colorTemperatureModeChanged();
    void colorTemperatureChanged();
    void customColorTempTimePeriodChanged();
    void wallpaperChanged();

private:
    QScopedPointer<DisplayModulePrivate> d_ptrDisplayModule;
    Q_DECLARE_PRIVATE_D(d_ptrDisplayModule, DisplayModule)
    Q_DISABLE_COPY(DisplayModule)
};
} // namespace dccV25
#endif // DISPLAYMODULE_H
