//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef SOUNDEFFECTSPAGE_H_V20
#define SOUNDEFFECTSPAGE_H_V20

#include "interface/namespace.h"
#include "widgets/switchwidget.h"

#include <dtkgui_global.h>
#include <dtkwidget_global.h>
#include <DDesktopServices>
#include <QModelIndex>
#include <QWidget>

DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

QT_BEGIN_NAMESPACE
class QStandardItemModel;
class QVBoxLayout;
class QSound;
QT_END_NAMESPACE

DWIDGET_BEGIN_NAMESPACE
class DListView;
DWIDGET_END_NAMESPACE

//class switchwidget;
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
    void requestSetEffectAble(DDesktopServices::SystemSoundEffect effect, bool enable);

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
