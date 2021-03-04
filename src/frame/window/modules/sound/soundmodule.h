/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     lq <longqi_cm@deepin.com>
 *
 * Maintainer: lq <longqi_cm@deepin.com>
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

public Q_SLOTS:
    void showSpeakerPage();
    void showMicrophonePage();
    void showSoundEffectsPage();

private:
    dcc::sound::SoundModel *m_model {nullptr};
    dcc::sound::SoundWorker *m_worker {nullptr};
    DCC_NAMESPACE::sound::SoundWidget *m_soundWidget {nullptr};
    MainWindow *m_pMainWindow;
};
} // namespace sound
} // namespace DCC_NAMESPACE
#endif // SOUNDMODULE_H_V20
