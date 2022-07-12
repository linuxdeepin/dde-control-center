/*
 * Copyright (C) 2021 ~ 2023 Deepin Technology Co., Ltd.
 *
 * Author:     caixiangrong <caixiangrong@uniontech.com>
 *
 * Maintainer: caixiangrong <caixiangrong@uniontech.com>
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
#include "layoutv20.h"

#include <QFormLayout>
#include <QScrollArea>
#include <QScrollBar>

DCC_USE_NAMESPACE

LayoutV20::LayoutV20()
    : m_layout(nullptr)
{
}

void LayoutV20::setCurrent(ModuleObject *const child)
{
    return;
}

QWidget *LayoutV20::layoutModule(ModuleObject *const module, QWidget *const parent, const QList<ModuleObject *> &children)
{
    Q_UNUSED(children)
    m_modules.clear();
    m_layout = new QHBoxLayout();
    m_layout->setMargin(0);
    m_layout->setSpacing(0);
    parent->setLayout(m_layout);

    int i = 0;

    QList<ModuleObject *> modules;
    modules.append(module);
    while (!modules.isEmpty()) {
        ModuleObject *tmpModule = modules.takeFirst();
        QWidget *page = nullptr;
        if (!LayoutBase::IsHiden(tmpModule))
            page = tmpModule->page();

        if (!page) {
            page = new QWidget(); // 空的占位
            page->setVisible(false);
        }
        m_layout->addWidget(page, i * 4 + 3);
        page->setDisabled(LayoutBase::IsDisabled(tmpModule));

        m_modules.append(tmpModule);
        QObject::connect(tmpModule, &ModuleObject::childStateChanged, m_layout, [this](ModuleObject *const tmpChild, uint32_t flag, bool state) {
            if (flag == WIDGETCHANGED_FLAG) {
                int index = m_modules.indexOf(tmpChild);
                if (index != -1) {
                    QWidget *newW = tmpChild->page();
                    if (!newW) {
                        newW = new QWidget();
                        newW->setVisible(false);
                    }
                    QWidget *oldW = m_layout->itemAt(index)->widget();
                    if (oldW != newW) {
                        QLayoutItem *item = m_layout->takeAt(index);
                        m_layout->insertWidget(index, newW, index * 4 + 3);
                        oldW->setVisible(false);
                        oldW->setParent(nullptr);
                        oldW->deleteLater();
                        delete item;
                    }
                }
            }
        });
        if (tmpModule->hasChildrens())
            modules.append(tmpModule->children(0));
    }

    return nullptr;
}
