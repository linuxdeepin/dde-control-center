// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SOUNDEFFECTSPAGE_H_V20
#define SOUNDEFFECTSPAGE_H_V20

#include "interface/namespace.h"

#include <DDesktopServices>
#include <DListView>

#include <QWidget>
#include <QSound>

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
