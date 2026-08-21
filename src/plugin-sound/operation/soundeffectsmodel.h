// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef SOUNDEFFECTSMODEL_H
#define SOUNDEFFECTSMODEL_H

#include <QObject>
#include <QAbstractListModel>


#include "soundEffectsData.h"
#include <QtQml/qqml.h>

class SoundEffectsModel : public QAbstractListModel
{
    Q_OBJECT

    QML_NAMED_ELEMENT(SoundEffectsModel)
    QML_SINGLETON
public:

    enum soundEffectsRoles{
        NameRole = Qt::UserRole + 1,
        DisplayTextRole,
        IsEnabled,
        AniIconPath
    };

    explicit SoundEffectsModel(QObject *parent = nullptr);
    ~SoundEffectsModel();

    void addData(SoundEffectsData* data);
    void removeData(SoundEffectsData* data);
    void clearData();
    SoundEffectsData* getSystemSoundEffect(int index);
    int getRowCount();

    void updateSoundEffectsData(int index, bool enable);
    void updateSoundEffectsAniIcon(int index, QString path);

protected:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    Q_INVOKABLE QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override {
        QHash<int, QByteArray> roles;
        roles[NameRole] = "name";
        roles[DisplayTextRole] = "dispalyText";
        roles[IsEnabled] = "isEnabled";
        roles[AniIconPath] = "aniIconPath";
        return roles;
    }

private:
    QList<SoundEffectsData*> m_soundEffectsData;
};

#endif // SOUNDEFFECTSMODEL_H
