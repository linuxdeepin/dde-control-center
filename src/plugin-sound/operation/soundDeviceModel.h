// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QObject>
#ifndef SOUNDDEVICEMODEL_H
#define SOUNDDEVICEMODEL_H

#include <QAbstractListModel>

#include "soundDeviceData.h"
//#include "soundmodel.h"

#include "port.h"

class SoundDeviceModel: public QAbstractListModel {

    Q_OBJECT
public:
    enum SoundEffectsRoles{
        NameRole = Qt::UserRole + 1,
        IsEnabled,
        IsActive,
    };
    Q_ENUM(SoundEffectsRoles)

    explicit SoundDeviceModel(QObject *parent = nullptr);

    void clearData();
    void addData(Port* port);
    void removeData(Port* port);
    int getRowCount();

    int getCurrentIndex() const;

    Port* getSoundDeviceData(int index);
    void updateSoundDeviceData(Port* port);
    void updateAllSoundDeviceData();

protected:

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override {
        QHash<int, QByteArray> roles;
        roles[NameRole] = "name";
        roles[IsEnabled] = "isEnabled";
        roles[IsActive] = "isActive";
        return roles;
    }


private:
    QList<Port*> m_ports;
};



#endif //SOUNDDEVICEMODEL_H
