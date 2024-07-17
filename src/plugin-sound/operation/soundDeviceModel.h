// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QObject>
#ifndef SOUNDDEVICEMODEL_H
#define SOUNDDEVICEMODEL_H

#include <QAbstractListModel>
#include <QtQml/qqml.h>

#include "soundDeviceData.h"
//#include "soundmodel.h"

#include "port.h"

class SoundDeviceModel: public QAbstractListModel {

    Q_OBJECT

    QML_NAMED_ELEMENT(SoundDeviceModel)
    QML_SINGLETON

public:
    enum soundEffectsRoles{
        NameRole = Qt::UserRole + 1,
        IsChecked
    };

    explicit SoundDeviceModel(QObject *parent = nullptr);

    void clearData();
    void addData(Port* port);
    void removeData(Port* port);
    int getRowCount();

    SoundDeviceData* getSoundDeviceData(int index);
    void updateSoundDeviceData(Port* port);

protected:

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    Q_INVOKABLE QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override {
        QHash<int, QByteArray> roles;
        roles[NameRole] = "name";
        roles[IsChecked] = "isChecked";
        return roles;
    }


private:
    QList<SoundDeviceData*> m_soundDeviceDatas;
    QList<Port*> m_ports;
};



#endif //SOUNDDEVICEMODEL_H
