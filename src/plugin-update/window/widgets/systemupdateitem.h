//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef SYSTEMUPDATEITEM_H
#define SYSTEMUPDATEITEM_H

#include "updatesettingitem.h"

#include <qdbusreply.h>
#include <QDBusInterface>
#include <QDBusPendingCall>

class SystemUpdateItem: public UpdateSettingItem
{
    Q_OBJECT
public:
    explicit SystemUpdateItem(QWidget *parent = nullptr);
    void showMore() override;
    void setData(UpdateItemInfo *updateItemInfo) override;
    char getLastNumForString(const QString &value);
    double subVersion(const QString &firstVersion, const QString &secondVersion);
    void createDetailInfoItem(const QList<DetailInfo> &detailInfoList, int index, int groupIndex = -1);

private:
    QList<DCC_NAMESPACE::DetailInfoItem *> m_updateDetailItemList;
    QFrame *m_line;
    QWidget *m_lineWidget;

};

#endif // SYSTEMUPDATEITEM_H
