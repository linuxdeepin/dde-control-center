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

#ifndef SOUNDEFFECTSPAGE_H_V20
#define SOUNDEFFECTSPAGE_H_V20

#include "window/namespace.h"

#include <DDesktopServices>

#include <QWidget>

class QListView;
class QVBoxLayout;
class QSound;

namespace dcc {

namespace sound {
class SoundModel;
}

namespace widgets {
class SwitchWidget;
}
}

DWIDGET_USE_NAMESPACE

namespace DCC_NAMESPACE {

namespace sound {

class HoverListView;
class SoundEffectsModel;

class SoundEffectsPage : public QWidget
{
    Q_OBJECT
public:
    SoundEffectsPage(QWidget *parent = nullptr);

public:
    void setModel(dcc::sound::SoundModel *model);

Q_SIGNALS:
    void requestSwitchSoundEffects(bool isOn);
    void requestSetEffectAble(DDesktopServices::SystemSoundEffect effect, bool enable);

public Q_SLOTS:
    void startPlay(const QModelIndex &index);
    void onEffectChanged(DDesktopServices::SystemSoundEffect effect, const bool enable);

private:
    void initList();

private:
    QVBoxLayout *m_layout{nullptr};
    dcc::widgets::SwitchWidget *m_sw{nullptr};
    dcc::sound::SoundModel *m_model{nullptr};
    HoverListView *m_effectList{nullptr};
    SoundEffectsModel *m_listModel{nullptr};
    QScopedPointer<QSound> m_sound;
};

}
}

#endif // SOUNDEFFECTSPAGE_H_V20
