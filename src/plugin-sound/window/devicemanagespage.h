//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DEVICEMANAGES_H
#define DEVICEMANAGES_H

#include <dtkwidget_global.h>
#include "interface/namespace.h"
#include "soundmodel.h"

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QListView;
QT_END_NAMESPACE

DWIDGET_BEGIN_NAMESPACE
class DListView;
class DTipLabel;
DWIDGET_END_NAMESPACE


namespace DCC_NAMESPACE {
class TitledSliderItem;
class SettingsGroup;
}

DWIDGET_USE_NAMESPACE

class SoundModel;
class DevicemanagesPage : public QWidget
{
    Q_OBJECT
public:
    explicit DevicemanagesPage(Port::Direction direction, QWidget *parent = nullptr);
    ~DevicemanagesPage();

public:
    void setModel(SoundModel *model);

Q_SIGNALS:
    void requestSwitchSetEnable(unsigned int cardId, const QString &cardName, bool enable);

private Q_SLOTS:
    void refreshPort();
    void addPort(const Port *port);
    void removePort(const QString &portId, const uint &cardId);

protected:
    void initUI();

private:
    Port::Direction m_direction;
    SoundModel *m_model{ nullptr };
    QVBoxLayout *m_layout{ nullptr };

    DCC_NAMESPACE::SettingsGroup *m_deviceGroup{ nullptr };
    QList<const Port *> m_devicePort;
};

#endif // DEVICEMANAGES_H
