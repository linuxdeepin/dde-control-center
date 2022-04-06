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
#include "widgets/switchwidget.h"

#include <DDesktopServices>
#include <QModelIndex>
#include <QWidget>

DWIDGET_USE_NAMESPACE

QT_BEGIN_NAMESPACE
class QStandardItemModel;
class QVBoxLayout;
class QSound;
QT_END_NAMESPACE

DWIDGET_BEGIN_NAMESPACE
class DListView;
DWIDGET_END_NAMESPACE

class switchwidget;
class SoundModel;
class SoundEffectsPage : public QWidget
{
    Q_OBJECT
public:
    SoundEffectsPage(QWidget *parent = nullptr);
    ~SoundEffectsPage();

public:
    void setModel(SoundModel *model);

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
    DCC_NAMESPACE::SwitchWidget *m_sw{nullptr};
    SoundModel *m_model{nullptr};
    DTK_WIDGET_NAMESPACE::DListView *m_effectList{nullptr};
    QStandardItemModel *m_listModel{nullptr};
    QScopedPointer<QSound> m_sound;
    QModelIndex m_playIdx;
    QTimer *m_aniTimer{nullptr};
    int m_aniDuration{0};
};

#endif // SOUNDEFFECTSPAGE_H_V20
