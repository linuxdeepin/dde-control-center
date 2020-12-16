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

#include "interface/namespace.h"

#include <DDesktopServices>
#include <DListView>

#include <QWidget>
#include <QSound>
#include <QFile>

QT_BEGIN_NAMESPACE


class QListView;
class QVBoxLayout;
QT_END_NAMESPACE

namespace dcc {

namespace sound {
class SoundModel;
}

namespace widgets {
class SwitchWidget;
}
}

namespace DCC_NAMESPACE {

namespace sound {

class SoundEffectsPage : public QWidget
{
    Q_OBJECT
public:
    SoundEffectsPage(QWidget *parent = nullptr);
    ~SoundEffectsPage();

public:
    void setModel(dcc::sound::SoundModel *model);

Q_SIGNALS:
    void requestSwitchSoundEffects(bool isOn);
    void requestRefreshList();
    void requestSetEffectAble(DTK_WIDGET_NAMESPACE::DDesktopServices::SystemSoundEffect effect, bool enable);

public Q_SLOTS:
    void startPlay(const QModelIndex &index);

private:
    void initList();

private:
    QVBoxLayout *m_layout{nullptr};
    dcc::widgets::SwitchWidget *m_sw{nullptr};
    dcc::sound::SoundModel *m_model{nullptr};
    DTK_WIDGET_NAMESPACE::DListView *m_effectList{nullptr};
    QStandardItemModel *m_listModel{nullptr};
    QScopedPointer<QSound> m_sound;
    QTimer *m_aniTimer{nullptr};
    QModelIndex m_playIdx;
    int m_aniDuration{0};

};

}
}

#endif // SOUNDEFFECTSPAGE_H_V20
