/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
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

#ifndef SOUNDMODULE_H
#define SOUNDMODULE_H

#include <QObject>

#include "interfaces/moduleinterface.h"
#include "soundwidget.h"
#include "soundworker.h"
#include "soundmodel.h"

namespace dcc {
namespace sound {

class SoundModule : public QObject, public ModuleInterface
{
    Q_OBJECT
public:
    explicit SoundModule(FrameProxyInterface * frame, QObject *parent = 0);
    ~SoundModule();

    void initialize() Q_DECL_OVERRIDE;
    const QString name() const Q_DECL_OVERRIDE;
    void moduleActive() Q_DECL_OVERRIDE;
    void moduleDeactive() Q_DECL_OVERRIDE;
    void reset() Q_DECL_OVERRIDE;
    ModuleWidget *moduleWidget() Q_DECL_OVERRIDE;
    void contentPopped(ContentWidget * const w) Q_DECL_OVERRIDE;

public Q_SLOTS:
    void showAdvancedPage();
    void showEffectPage();

private:
    SoundWidget *m_soundWidget;
    SoundWorker *m_soundWorker;
    SoundModel *m_soundModel;
};

}
}
#endif // SOUNDMODULE_H
