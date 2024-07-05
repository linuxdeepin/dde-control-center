// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "soundeffectsmodel.h"

SoundEffectsModel::SoundEffectsModel(QObject *parent)
    : QAbstractListModel{parent}
{

}

void SoundEffectsModel::addData(SoundEffectsData* data)
{
    m_soundEffectsData.append(data);
}

void SoundEffectsModel::removeData(SoundEffectsData *data)
{
    m_soundEffectsData.removeAll(data);
    delete data;
    data = NULL;
}

void SoundEffectsModel::clearData()
{
    for (SoundEffectsData* soundEffectsData : m_soundEffectsData) {
        delete soundEffectsData;
    }
    m_soundEffectsData.clear();
}

// SoundEffectsData SoundEffectsModel::getSystemSoundEffect(int index)
// {
//     if (m_soundEffectsData.count() < index) {
//         return "";
//     }
// }

int SoundEffectsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_soundEffectsData.count();
}

QVariant SoundEffectsModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_soundEffectsData.count())
        return QVariant();

    const SoundEffectsData* soundEffectsData = m_soundEffectsData[index.row()];
    if (role == NameRole)
        return soundEffectsData->name();
    else if (role == DisplayTextRole)
        return soundEffectsData->dispalyText();
    else if (role == IsChecked)
        return soundEffectsData->checked();
    return QVariant();
}
