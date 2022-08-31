// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LOGOITEM_H
#define LOGOITEM_H

#include "widgets/settingsitem.h"
#include <QLabel>

namespace dcc {
namespace widgets {
class TipsLabel;
}
}

using namespace dcc::widgets;

namespace dcc {
namespace systeminfo {

class LogoItem : public SettingsItem
{
    Q_OBJECT

    Q_PROPERTY(QString logo READ logo WRITE setLogo DESIGNABLE true SCRIPTABLE true)

public:
    explicit LogoItem(QFrame *parent = 0);
    void setDescription(const QString &des);
    void setDescription(bool isVisible);//修改m_description是否默认不显示
    void setLogo(const QString &logo);
    const QString logo() const { return m_logo->text(); }

private:
    QLabel *m_logo;
    TipsLabel *m_description;
};

}
}
#endif // LOGOITEM_H
