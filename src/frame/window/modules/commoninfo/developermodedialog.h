/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     yangyuyin <yangyuyin_cm@deepin.com>
 *
 * Maintainer: yangyuyin <yangyuyin@deepin.com>
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
#ifndef DeveloperModeDialog_H
#define DeveloperModeDialog_H

#include "interface/namespace.h"

#include <DAbstractDialog>
#include <DRadioButton>
#include <DSuggestButton>

struct TS{
    QString ts1;
    QString ts2;
};

namespace DCC_NAMESPACE {
namespace commoninfo {

class CommonInfoModel;

class DeveloperModeDialog : public DTK_WIDGET_NAMESPACE::DAbstractDialog
{
    Q_OBJECT
public:
    explicit DeveloperModeDialog(DAbstractDialog *parent = nullptr);

public:
     void setModel(CommonInfoModel *model);

Q_SIGNALS:
     void requestDeveloperMode(bool enabled);
     void requestLogin();
     void requestCommit(QString filePathName);
     void requestSetNextBtnStatus(bool state);
private Q_SLOTS:
     void setLogin();

private:
    DTK_WIDGET_NAMESPACE::DRadioButton *m_onlineBtn{nullptr};
    DTK_WIDGET_NAMESPACE::DRadioButton *m_offlineBtn{nullptr};
    DTK_WIDGET_NAMESPACE::DSuggestButton *m_nextButton{nullptr};
    CommonInfoModel *m_model{nullptr};
    bool m_enterDev{false};
};
}
}

#endif // DEVELPERMODEACTIVATE_H
