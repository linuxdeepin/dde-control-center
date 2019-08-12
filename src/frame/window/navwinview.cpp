/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     zccrs <zccrs@live.com>
 *
 * Maintainer: zccrs <zhangjide@deepin.com>
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
#include "navwinview.h"
#include "constant.h"

#include <DStyleOption>
#include <DPalette>

DWIDGET_USE_NAMESPACE

NavWinView::NavWinView(QWidget *parent)
    : QListView(parent)
    , m_delegate(new NavDelegate(IconMode, this))
{
    setItemDelegate(m_delegate);
    setViewMode(IconMode);
    setResizeMode(QListView::Adjust);
    setSpacing(10);
}

void NavWinView::setViewMode(QListView::ViewMode mode)
{
    m_delegate->setViewMode(mode);

    QSize size;
    QVariant bgTyle;
    if (mode == IconMode) {
        setWordWrap(true);
        setWrapping(true);
        setSpacing(20);
        setFlow(QListView::LeftToRight);

        size = QSize(170, 168);
        bgTyle = Dtk::RoundedBackground;
    } else {
        setWordWrap(false);
        setWrapping(false);
        setSpacing(0);
        setFlow(QListView::TopToBottom);

        size = QSize(168, 48);
        bgTyle = Dtk::RoundedBackground;
    }

    auto mod = model();
    if (!mod)
        return;

    DPalette pa = DPalette::get(this);
    pa.setBrush(DPalette::ItemBackground, palette().base());
    DPalette::set(this, pa);
    viewport()->setAutoFillBackground(mode != IconMode);

    for (auto r = 0; r < mod->rowCount(); ++r) {
        for (int c = 0; c < mod->columnCount(); ++c) {
            auto idx = mod->index(r, c);
            mod->setData(idx, bgTyle, Dtk::BackgroundTypeRole);
            mod->setData(idx, size, Qt::SizeHintRole);
        }
    }
}
