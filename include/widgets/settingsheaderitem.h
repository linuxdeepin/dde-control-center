// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SETTINGSHEADERITEM_H
#define SETTINGSHEADERITEM_H

#include "widgets/settingsitem.h"
#include "widgets/titlelabel.h"

#include <QHBoxLayout>
#include <QLabel>

namespace dcc {
namespace widgets {

class SettingsHeaderItem : public SettingsItem
{
    Q_OBJECT

public:
    explicit SettingsHeaderItem(QWidget *parent = 0);

    TitleLabel *textLabel() const { return m_headerText; }
    QHBoxLayout *layout() const { return m_mainLayout; }

    void setTitle(const QString &title);
    void setRightWidget(QWidget *widget);

private:
    QHBoxLayout *m_mainLayout;
    TitleLabel *m_headerText;
};

}
}

#endif // SETTINGSHEADERITEM_H
