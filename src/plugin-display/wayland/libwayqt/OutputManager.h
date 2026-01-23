// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <wayland-client-protocol.h>

#include <QMap>
#include <QObject>
#include <QRect>
#include <QString>

struct wl_buffer;
struct wl_output;

struct zwlr_output_manager_v1;
struct zwlr_output_head_v1;
struct zwlr_output_mode_v1;
struct zwlr_output_configuration_v1;
struct zwlr_output_configuration_head_v1;

struct zwlr_output_manager_v1_listener;
struct zwlr_output_head_v1_listener;
struct zwlr_output_mode_v1_listener;
struct zwlr_output_configuration_v1_listener;

namespace WQt {
class OutputManager;
class OutputHead;
class OutputMode;
class OutputConfiguration;
class OutputConfigurationHead;
} // namespace WQt

class WQt::OutputManager : public QObject
{
    Q_OBJECT;

public:
    OutputManager(zwlr_output_manager_v1 *);
    ~OutputManager();

    /** Create a configuration object */
    WQt::OutputConfiguration *createConfiguration();

    QList<WQt::OutputHead *> heads();

    /** Stop monitoring the outupts */
    void stop();

    zwlr_output_manager_v1 *get();

private:
    static void handleHead(void *, zwlr_output_manager_v1 *, zwlr_output_head_v1 *);
    static void handleDone(void *, zwlr_output_manager_v1 *, uint32_t);
    static void handleFinished(void *, zwlr_output_manager_v1 *);

    zwlr_output_manager_v1 *mObj;
    uint32_t mSerial;

    static const zwlr_output_manager_v1_listener mListener;

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

    OutputHead();
    OutputHead(zwlr_output_head_v1 *);
    OutputHead(const WQt::OutputHead &);
    ~OutputHead();

    /** Get the suitable property of this head */
    QVariant property(WQt::OutputHead::Property);

    zwlr_output_head_v1 *get();

private:
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

    static const zwlr_output_head_v1_listener mListener;

    zwlr_output_head_v1 *mObj;

    /** Properties map */
    QMap<int, QVariant> mPropsMap;
    QList<WQt::OutputMode *> mModes;
    WQt::OutputMode *mCurrentMode;

Q_SIGNALS:
    void changed(WQt::OutputHead::Property);

    void finished();
};

class WQt::OutputMode : public QObject
{
    Q_OBJECT;

public:
    OutputMode();
    OutputMode(zwlr_output_mode_v1 *);
    OutputMode(const WQt::OutputMode &);
    ~OutputMode();

    QSize size();
    int32_t refreshRate();
    bool isPreferred();

    zwlr_output_mode_v1 *get();

private:
    static void handleSize(void *, zwlr_output_mode_v1 *, int32_t, int32_t);
    static void handleRefreshRate(void *, zwlr_output_mode_v1 *, int32_t);
    static void handlePreferred(void *, zwlr_output_mode_v1 *);
    static void handleFinished(void *, zwlr_output_mode_v1 *);

    static const zwlr_output_mode_v1_listener mListener;

    zwlr_output_mode_v1 *mObj{ nullptr };

    /** Resolution */
    QSize mSize{ 0, 0 };

    /** Refresh rate */
    int32_t mRefreshRate{ 0 };

    /** By default this is false */
    bool mIsPreferred{ false };

Q_SIGNALS:
    void sizeChanged(QSize);
    void refreshRateChanged(int32_t);
    void setAsPreferred();

    void finished();
};

class WQt::OutputConfiguration : public QObject
{
    Q_OBJECT;

public:
    enum Error {
        AlreadyConfiguredHead = 1,
        UnconfiguredHead = 2,
        AlreadyUsed = 3,
    };

    OutputConfiguration(zwlr_output_configuration_v1 *);
    ~OutputConfiguration();

    /** Get the outupt configuration head object for the enabled output head */
    WQt::OutputConfigurationHead *enableHead(WQt::OutputHead *);

    /** Disabled the given head */
    void disableHead(WQt::OutputHead *);

    /** This object is destroyed after calling this function */
    void apply();

    /** This object is destroyed after calling this function */
    void test();

private:
    static void handleSucceeded(void *, zwlr_output_configuration_v1 *);
    static void handleFailed(void *, zwlr_output_configuration_v1 *);
    static void handleCanceled(void *, zwlr_output_configuration_v1 *);

    static const zwlr_output_configuration_v1_listener mListener;

    zwlr_output_configuration_v1 *mObj;

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

    OutputConfigurationHead(zwlr_output_configuration_head_v1 *);
    ~OutputConfigurationHead();

    void setMode(WQt::OutputMode *);
    void setCustomMode(QSize, int32_t);
    void setPosition(QPoint);
    void setTransform(int32_t);
    void setScale(qreal);

private:
    zwlr_output_configuration_head_v1 *mObj;
};

Q_DECLARE_METATYPE(WQt::OutputHead);
Q_DECLARE_METATYPE(WQt::OutputMode);
Q_DECLARE_METATYPE(QList<WQt::OutputMode *>);
Q_DECLARE_METATYPE(QList<WQt::OutputHead *>);
