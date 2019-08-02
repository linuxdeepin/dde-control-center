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

#include "soundeffectspage.h"
#include "soundeffectsmodel.h"
#include "soundeffectsdelegate.h"
#include "hoverlistview.h"

#include "modules/sound/soundmodel.h"
#include "widgets/switchwidget.h"

#include <QVBoxLayout>
#include <QListView>
#include <QLabel>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QSound>

#include <QDebug>

using namespace dcc::sound;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::sound;

SoundEffectsPage::SoundEffectsPage(QWidget *parent)
    : QWidget(parent)
    , m_layout(new QVBoxLayout)
    , m_sw(new SwitchWidget(tr("System Sound Effect")))
    , m_effectList(new HoverListView)
    , m_sound(nullptr)
{
    setMinimumWidth(400);
    m_layout->addWidget(m_sw);
    m_layout->addSpacing(20);

    m_effectList->setMaximumHeight(800);
    m_effectList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_effectList->setSelectionMode(QListView::SelectionMode::NoSelection);
    m_layout->addWidget(m_effectList);
    setLayout(m_layout);
}

void SoundEffectsPage::setModel(dcc::sound::SoundModel *model)
{
    m_model = model;

    initList();

    connect(m_model, &SoundModel::soundEffectOnChanged, m_sw, &SwitchWidget::setChecked);
    connect(m_sw, &SwitchWidget::checkedChanged,
            this, &SoundEffectsPage::requestSwitchSoundEffects);
    connect(m_sw, &SwitchWidget::checkedChanged, m_effectList, &HoverListView::setVisible);
}

void SoundEffectsPage::startPlay(const QModelIndex &index)
{
    auto eff = m_model->soundEffectMap()[index.row()].second;
    m_sound.reset(new QSound(m_model->soundEffectPathByType(eff)));

    m_sound->stop();
    m_sound->play();
}

void SoundEffectsPage::onEffectChanged(DDesktopServices::SystemSoundEffect effect,
                                       const bool enable)
{
    if (!m_listModel)
        return;
    for (int i = 0; i < m_model->soundEffectMap().size(); ++i) {
        auto eff = m_model->soundEffectMap()[i];
        if (effect == eff.second) {
            auto idx = m_listModel->index(i, 0);
            m_listModel->setItemSelect(idx, enable);
        }
    }
}

void SoundEffectsPage::initList()
{
    m_listModel = new SoundEffectsModel(m_model, this);
    m_effectList->setModel(m_listModel);
    auto delegate = new SoundEffectsDelegate(this);
    m_effectList->setItemDelegate(delegate);
    m_sw->setChecked(m_model->enableSoundEffect());

    QStringList sl;
    for (int i = 1; i <= 4; ++i) {
        sl.append(QString(":/sound/themes/dark/sound_preview_%1.svg")
                  .arg(i));
    }
    m_listModel->setAnimImg(sl);
    m_listModel->setHoverBtnImg(":/sound/themes/dark/sound_preview_4.svg");

    connect(delegate, &SoundEffectsDelegate::hoverItemChange,
            m_listModel, &SoundEffectsModel::setHoverIndex);
    connect(delegate, &SoundEffectsDelegate::playBtnClicked,
            m_listModel, &SoundEffectsModel::startPlay);
    connect(delegate, &SoundEffectsDelegate::playBtnClicked, this, &SoundEffectsPage::startPlay);
    connect(m_effectList, &QListView::clicked, m_listModel, &SoundEffectsModel::switchItem);
    connect(m_listModel, &SoundEffectsModel::itemSelectChanged,
    [ this ](const QModelIndex & idx, bool isOn) {
        Q_EMIT requestSetEffectAble(m_model->soundEffectMap()[idx.row()].second, isOn);
    });
    connect(m_model, &SoundModel::soundEffectDataChanged,
            this, &SoundEffectsPage::onEffectChanged);

    Q_ASSERT(m_listModel);
    for (int i = 0; i < m_model->soundEffectMap().size(); ++i) {
        auto eff = m_model->soundEffectMap()[i];
        auto idx = m_listModel->index(i, 0);
        m_listModel->setItemSelect(idx, m_model->queryEffectData(eff.second));
    }
}
