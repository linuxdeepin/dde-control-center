// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "qwayland-wlr-output-management-unstable-v1.h"

#include <QMap>
#include <QObject>
#include <QRect>
#include <QString>
#include <QtWaylandClient/QWaylandClientExtension>

namespace WQt {
class OutputManager;
class OutputHead;
class OutputMode;
class OutputConfiguration;
class OutputConfigurationHead;
} // namespace WQt

class WQt::OutputManager : public QWaylandClientExtensionTemplate<WQt::OutputManager>, public QtWayland::zwlr_output_manager_v1
{
    Q_OBJECT
    Q_PROPERTY(bool active READ isActive NOTIFY activeChanged)

public:
    OutputManager(QObject *parent = nullptr);
    ~OutputManager() override;

    WQt::OutputConfiguration *createConfiguration();
    QList<WQt::OutputHead *> heads();

    void stop();

    ::zwlr_output_manager_v1 *get();

protected:
    void zwlr_output_manager_v1_head(zwlr_output_head_v1 *id) override;
    void zwlr_output_manager_v1_done(uint32_t serial) override;
    void zwlr_output_manager_v1_finished() override;

private:
    uint32_t mSerial = 0;
    QList<WQt::OutputHead *> mHeads;
    bool mIsDone = false;

Q_SIGNALS:
    void headAttached(WQt::OutputHead *);
    void done();
};

class WQt::OutputHead : public QObject
{
    Q_OBJECT;

public:
    enum Property {
        Name = 0xbf278e,
        Description,
        PhysicalSize,
        Modes,
        Enabled,
        CurrentMode,
        Position,
        Transform,
        Scale,
        Make,
        Model,
        SerialNumber,
    };

    OutputHead(QObject *parent = nullptr);
    OutputHead(zwlr_output_head_v1 *obj, QObject *parent = nullptr);
    OutputHead(const WQt::OutputHead &);
    ~OutputHead();

    QVariant property(WQt::OutputHead::Property);
    zwlr_output_head_v1 *get();

private:
    zwlr_output_head_v1 *mObj = nullptr;
    QMap<int, QVariant> mPropsMap;
    QList<WQt::OutputMode *> mModes;
    WQt::OutputMode *mCurrentMode = nullptr;

    void setupListeners();

    static void handleName(void *, zwlr_output_head_v1 *, const char *);
    static void handleDescription(void *, zwlr_output_head_v1 *, const char *);
    static void handlePhysicalSize(void *, zwlr_output_head_v1 *, int32_t, int32_t);
    static void handleMode(void *, zwlr_output_head_v1 *, zwlr_output_mode_v1 *);
    static void handleEnabled(void *, zwlr_output_head_v1 *, int32_t);
    static void handleCurrentMode(void *, zwlr_output_head_v1 *, zwlr_output_mode_v1 *);
    static void handlePosition(void *, zwlr_output_head_v1 *, int32_t, int32_t);
    static void handleTransform(void *, zwlr_output_head_v1 *, int32_t);
    static void handleScale(void *, zwlr_output_head_v1 *, wl_fixed_t);
    static void handleFinished(void *, zwlr_output_head_v1 *);
    static void handleMake(void *, zwlr_output_head_v1 *, const char *);
    static void handleModel(void *, zwlr_output_head_v1 *, const char *);
    static void handleSerialNumber(void *, zwlr_output_head_v1 *, const char *);
    static void handleAdaptiveSync(void *, zwlr_output_head_v1 *, uint32_t);

Q_SIGNALS:
    void changed(WQt::OutputHead::Property);
    void finished();
};

class WQt::OutputMode : public QObject
{
    Q_OBJECT;

public:
    OutputMode(QObject *parent = nullptr);
    OutputMode(zwlr_output_mode_v1 *obj, QObject *parent = nullptr);
    OutputMode(const WQt::OutputMode &);
    ~OutputMode();

    QSize size();
    int32_t refreshRate();
    bool isPreferred();

    zwlr_output_mode_v1 *get();

private:
    zwlr_output_mode_v1 *mObj = nullptr;
    QSize mSize{ 0, 0 };
    int32_t mRefreshRate{ 0 };
    bool mIsPreferred{ false };

    void setupListeners();

    static void handleSize(void *, zwlr_output_mode_v1 *, int32_t, int32_t);
    static void handleRefreshRate(void *, zwlr_output_mode_v1 *, int32_t);
    static void handlePreferred(void *, zwlr_output_mode_v1 *);
    static void handleFinished(void *, zwlr_output_mode_v1 *);

Q_SIGNALS:
    void sizeChanged(QSize);
    void refreshRateChanged(int32_t);
    void setAsPreferred();
    void finished();
};

class WQt::OutputConfiguration : public QObject, public QtWayland::zwlr_output_configuration_v1
{
    Q_OBJECT;

public:
    enum Error {
        AlreadyConfiguredHead = 1,
        UnconfiguredHead = 2,
        AlreadyUsed = 3,
    };

    OutputConfiguration(struct ::zwlr_output_configuration_v1 *obj, QObject *parent = nullptr);
    ~OutputConfiguration() override;

    WQt::OutputConfigurationHead *enableHead(WQt::OutputHead *);
    void disableHead(WQt::OutputHead *);
    void apply();
    void test();

protected:
    void zwlr_output_configuration_v1_succeeded() override;
    void zwlr_output_configuration_v1_failed() override;
    void zwlr_output_configuration_v1_cancelled() override;

Q_SIGNALS:
    void succeeded();
    void failed();
    void canceled();
};

class WQt::OutputConfigurationHead : public QObject
{
    Q_OBJECT;

public:
    enum Error {
        AlreadySet = 1,
        InvalidMode = 2,
        InvalidCustomMode = 3,
        InvalidTransform = 4,
        InvalidScale = 5,
    };

    OutputConfigurationHead(zwlr_output_configuration_head_v1 *obj, QObject *parent = nullptr);
    ~OutputConfigurationHead();

    void setMode(WQt::OutputMode *);
    void setCustomMode(QSize, int32_t);
    void setPosition(QPoint);
    void setTransform(int32_t);
    void setScale(qreal);

private:
    zwlr_output_configuration_head_v1 *mObj = nullptr;
};

Q_DECLARE_METATYPE(WQt::OutputHead);
Q_DECLARE_METATYPE(WQt::OutputMode);
Q_DECLARE_METATYPE(QList<WQt::OutputMode *>);
Q_DECLARE_METATYPE(QList<WQt::OutputHead *>);
