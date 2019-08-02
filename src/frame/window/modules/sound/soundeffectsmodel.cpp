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

#include "soundeffectsmodel.h"

#include "modules/sound/soundmodel.h"

using namespace dcc::sound;
using namespace DCC_NAMESPACE::sound;

#include <QSize>
#include <QEvent>
#include <QMouseEvent>
#include <QTimer>

const int AnimPerFrame = 200;
const int AnimDelay = 5000;

SoundEffectsModel::SoundEffectsModel(SoundModel *model, QObject *parent)
    : QAbstractListModel(parent)
    , m_model(model)
{
    //defualt img;
    m_btnImg = ":/sound/themes/dark/sound_preview_%1.svg";
    m_animImg << ":/sound/themes/dark/sound_preview_%1.svg";
}

QRect SoundEffectsModel::getHoverImgRect(const QStyleOptionViewItem &option,
                                         const QModelIndex &index)
{
    QStyle *style = option.widget->style();
    auto indRect = style->subElementRect(QStyle::SubElement::SE_ViewItemCheckIndicator,
                                         &option);
    int left = indRect.left();
    if (indRect.center().x() < option.rect.center().x())
        left = option.rect.right();
    left -= index.data(int(Role::HoveBtnMargin)).toInt();

    auto imgSize = index.data(int(Role::HoverBtnSize)).toSize();
    return QRect(QPoint(left, option.rect.top()
                        + (option.rect.height() - imgSize.height()) / 2), imgSize);
}


void SoundEffectsModel::setAnimImg(const QStringList &imgs)
{
    stopPlay();
    m_animIdx = 0;
    m_animImg = imgs;
}

int SoundEffectsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_model->soundEffectMap().size();
}

QVariant SoundEffectsModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case Qt::DisplayRole:
        return m_model->soundEffectMap()[index.row()].first;
    case Qt::CheckStateRole:
        return -1 != m_selectList.indexOf(index.row()) ?
               Qt::CheckState::Checked : Qt::CheckState::Unchecked;
    case Qt::SizeHintRole:
        return QSize(0, 40);
    default:
        break;
    }

    switch (Role(role)) {
    case Role::Playing:
        return index.row() == m_playingIndex;
    case Role::AnimImg:
        if (-1 == m_animIdx)
            break;
        return m_animImg[m_animIdx];
    case Role::HoverIndex:
        return m_hoverInx;
    case Role::HoveBtnMargin:
        return 20;
    case Role::HoverBtnSize:
        return QPixmap(m_btnImg).size();
    case Role::HoverBtnImg:
        return m_btnImg;
    }

    return QVariant();
}

void SoundEffectsModel::switchItem(const QModelIndex &index)
{
    int idx = index.row();
    if (-1 == m_selectList.indexOf(idx)) {
        setItemSelect(index, true);
        Q_EMIT itemSelectChanged(index, true);
    } else {
        setItemSelect(index, false);
        Q_EMIT itemSelectChanged(index, false);
    }
}

void SoundEffectsModel::setItemSelect(const QModelIndex &idx, bool isOn)
{
    if (isOn && -1 == m_selectList.indexOf(idx.row())) {
        m_selectList.append(idx.row());
        QVector<int> crole;
        crole.append(Qt::CheckStateRole);
        Q_EMIT QAbstractItemModel::dataChanged(idx, idx, crole);
    } else if (!isOn && -1 != m_selectList.indexOf(idx.row())) {
        m_selectList.removeOne(idx.row());
        QVector<int> crole;
        crole.append(Qt::CheckStateRole);
        Q_EMIT dataChanged(idx, idx, crole);
    }
}

void SoundEffectsModel::startPlay(const QModelIndex &index)
{
    int idx = index.row();
    m_playingIndex = idx;
    if (m_animTimer)
        m_animTimer->stop();
    else
        m_animTimer = new QTimer;

    m_animTimer->disconnect();

    connect(m_animTimer, &QTimer::timeout, this, &SoundEffectsModel::animIndexChanged);
    m_animTimer->start(AnimPerFrame);

    if (m_stopTimer) {
        m_stopTimer->stop();
    } else {
        m_stopTimer = new QTimer();
        m_stopTimer->setSingleShot(true);
    }

    connect(m_stopTimer, &QTimer::timeout, this, &SoundEffectsModel::stopPlay);
    m_stopTimer->start(AnimDelay);
}

void SoundEffectsModel::stopPlay()
{
    if (m_animTimer)
        m_animTimer->stop();
    if (m_playingIndex == -1)
        return;
    const QModelIndex cidx = this->index(m_playingIndex, 0);
    QVector<int> crole;
    crole.append(int(Role::AnimImg));
    m_playingIndex = -1;
    Q_EMIT QAbstractItemModel::dataChanged(cidx, cidx, crole);
}

void SoundEffectsModel::animIndexChanged()
{
    Q_ASSERT(m_animImg.size());
    m_animIdx = (m_animIdx + 1) % m_animImg.size();
    const QModelIndex cidx = this->index(m_playingIndex, 0);
    QVector<int> crole;
    crole.append(int(Role::AnimImg));
    Q_EMIT QAbstractItemModel::dataChanged(cidx, cidx, crole);
}

void SoundEffectsModel::setHoverIndex(int idx)
{
    if (idx == m_hoverInx)
        return;
    QVector<int> crole;
    crole.append(int(Role::HoverIndex));
    int tindex = m_hoverInx;
    m_hoverInx = idx;
    Q_EMIT QAbstractItemModel::dataChanged(index(tindex), index(tindex), crole);
    Q_EMIT QAbstractItemModel::dataChanged(index(m_hoverInx), index(m_hoverInx), crole);
}
