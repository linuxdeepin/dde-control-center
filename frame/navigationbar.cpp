/*
 * Copyright (C) 2015 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             Hualet <mr.asianwang@gmail.com>
 *             kirigaya <kirigaya@mkacg.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             Hualet <mr.asianwang@gmail.com>
 *             kirigaya <kirigaya@mkacg.com>
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

#include "navigationbar.h"

#include <QVBoxLayout>
#include <QDebug>
#include <QSignalMapper>

static const QStringList ModuleList = {
#ifndef DISABLE_ACCOUNT
    "accounts",
#endif
#ifndef DISABLE_DISPALY
    "display",
#endif
#ifndef DISABLE_DEFAULT_APPLICATIONS
    "defapp",
#endif
#ifndef DISABLE_PERSONALIZATION
    "personalization",
#endif
#ifndef DISABLE_NETWORK
    "network",
#endif
#ifndef DISABLE_BLUETOOTH
    "bluetooth",
#endif
#ifndef DISABLE_SOUND
    "sound",
#endif
#ifndef DISABLE_DATETIME
    "datetime",
#endif
#ifndef DISABLE_POWER
    "power",
#endif
#ifndef DISABLE_MOUSE
    "mouse",
#endif
#ifndef DISABLE_KAYBOARD
    "keyboard",
#endif
#ifndef DISABLE_WACOM
    "wacom",
#endif
#ifndef DISABLE_SYS_UPDATE
    "update",
#endif
#ifndef DISABLE_SYSINFO
    "systeminfo",
#endif
};

NavigationBar::NavigationBar(QWidget *parent)
    : QWidget(parent)

{
    QVBoxLayout *buttonsLayout = new QVBoxLayout;
    buttonsLayout->setContentsMargins(0, 0, 0, 0);
    buttonsLayout->setSpacing(20);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addStretch();
    centralLayout->addLayout(buttonsLayout);
    centralLayout->addStretch();
    centralLayout->setContentsMargins(0, 0, 0, 0);
    centralLayout->setSpacing(20);

    setLayout(centralLayout);

    // init buttons
    for (const auto &module : ModuleList)
    {
        DImageButton *b = new DImageButton;
        b->setCheckedPic(QString(":/%1/themes/dark/icons/nav_%1.png").arg(module));
        b->setNormalPic(QString(":/%1/themes/dark/icons/nav_%1_normal.svg").arg(module));
        b->setCheckable(true);
        b->setVisible(false);

        buttonsLayout->addWidget(b);
        m_navigationButtons.insert(module, b);

        connect(b, &DImageButton::clicked, this, &NavigationBar::onNavigationButtonClicked);
    }

    // init first button to checked
    m_navigationButtons[ModuleList.first()]->clicked();
}

QString NavigationBar::currentModuleName() const
{
    if (!m_checkedButton.isNull())
        return m_navigationButtons.key(m_checkedButton.data());

    return QString();
}

void NavigationBar::setModuleVisible(const QString &module, bool visible)
{
    if (m_navigationButtons.contains(module))
        m_navigationButtons[module]->setVisible(visible);
}

void NavigationBar::setModuleChecked(const QString &module)
{
    DImageButton* b = m_navigationButtons[module];

    Q_ASSERT(b);

    setModuleChecked(b);
}

void NavigationBar::onNavigationButtonClicked()
{
    DImageButton *b = static_cast<DImageButton *>(sender());
    Q_ASSERT(b);

    setModuleChecked(b);

    emit requestModule(m_navigationButtons.key(b));
}

void NavigationBar::setModuleChecked(DImageButton *button)
{
    if (m_checkedButton == button)
        return button->setChecked(true);

    if (!m_checkedButton.isNull())
        m_checkedButton->setChecked(false);

    m_checkedButton = button;
    m_checkedButton->setChecked(true);
}
