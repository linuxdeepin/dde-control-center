// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SOUNDMODULE_H_V20
#define SOUNDMODULE_H_V20

#include "interface/moduleinterface.h"
#include "interface/namespace.h"

#include <QObject>

namespace dcc {

namespace sound {
class SoundModel;
class SoundWorker;
} // namespace sound
} // namespace dcc

namespace DCC_NAMESPACE {
class MainWindow;
namespace sound {

class SoundWidget;

class SoundModule : public QObject
    , public ModuleInterface
{
    Q_OBJECT
public:
    SoundModule(FrameProxyInterface *frameProxy, QObject *parent = nullptr);

public:
    void initialize() override;
    const QString name() const override;
    const QString displayName() const override;
    void active() override;
    int load(const QString &path) override;
    QStringList availPage() const override;
    void preInitialize(bool sync = false, FrameProxyInterface::PushType = FrameProxyInterface::PushType::Normal) override;
    virtual void addChildPageTrans() const override;

private:
    void initSearchData() override;
    void getPortCount();

public Q_SLOTS:
    void showSpeakerPage();
    void showMicrophonePage();
    void showSoundEffectsPage();
    void showDeviceManagesPage();

private:
    dcc::sound::SoundModel *m_model {nullptr};
    dcc::sound::SoundWorker *m_worker {nullptr};
    DCC_NAMESPACE::sound::SoundWidget *m_soundWidget {nullptr};
    MainWindow *m_pMainWindow;
    int m_outputPortCount;
    int m_inputPortCount;
    int m_outputPortEnableCount;
    int m_inputPortEnableCount;
};
} // namespace sound
} // namespace DCC_NAMESPACE
#endif // SOUNDMODULE_H_V20
