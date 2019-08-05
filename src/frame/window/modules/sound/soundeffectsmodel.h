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

#ifndef SOUNDEFFECTSMODEL_H
#define SOUNDEFFECTSMODEL_H

#include "window/namespace.h"

#include <QAbstractListModel>
#include <DStyleOption>

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

namespace dcc {

namespace sound {
class SoundModel;
}
}

namespace DCC_NAMESPACE {

namespace sound {

class SoundEffectsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    SoundEffectsModel(dcc::sound::SoundModel *model, QObject *parent = nullptr);

public:
    static QRect getHoverImgRect(const QStyleOptionViewItem &option,
                                 const QModelIndex &index);
public:
    void setAnimImg(const QStringList &imgs);
    void setHoverBtnImg(const QString &img) {  m_btnImg = img; }

public Q_SLOTS:
    void switchItem(const QModelIndex &idx);
    void setItemSelect(const QModelIndex &idx, bool isOn);
    void startPlay(const QModelIndex &index);
    void stopPlay();
    void animIndexChanged();
    void setHoverIndex(int index);

Q_SIGNALS:
    void itemSelectChanged(const QModelIndex &idx, bool isOn);

public:
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

public:
    enum class Role : int {
        Playing = Dtk::UserRole + 1,
        AnimImg,
        HoverIndex,
        HoverBtnImg,
        HoverBtnSize,
        HoveBtnMargin
    };

private:
    dcc::sound::SoundModel *m_model{nullptr};
    QList<int> m_selectList;
    int m_hoverInx{-1};
    int m_playingIndex{-1};
    int m_animIdx{0};
    QTimer *m_animTimer{nullptr};
    QTimer *m_stopTimer{nullptr};
    QStringList m_animImg;
    QString m_btnImg;
};

}

}

#endif // SOUNDEFFECTSMODEL_H
