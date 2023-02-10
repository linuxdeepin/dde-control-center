//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LOGOITEM_H
#define LOGOITEM_H

#include "widgets/settingsitem.h"

class QLabel;
namespace DCC_NAMESPACE {

class LogoItem : public SettingsItem
{
    Q_OBJECT

    Q_PROPERTY(QString logo READ logo WRITE setLogo DESIGNABLE true SCRIPTABLE true)

public:
    explicit LogoItem(QFrame *parent = 0);
    void setDescription(const QString &des);
    void setDescription(bool isVisible);//修改m_description是否默认不显示
    void setLogo(const QString &logo);
    const QString logo();

private:
    QLabel *m_logo;
    QLabel *m_description;
};

}
#endif // LOGOITEM_H
