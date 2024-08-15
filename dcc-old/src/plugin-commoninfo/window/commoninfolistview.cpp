//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "commoninfolistview.h"

CommonInfoListView::CommonInfoListView(QWidget *parent)
    : DTK_WIDGET_NAMESPACE::DListView(parent)
    , m_maxheight(std::nullopt)
{

}

void CommonInfoListView::setMaxShowHeight(int height)
{
    m_maxheight = height;
}

void CommonInfoListView::updateGeometries()
{
    DListView::updateGeometries();
    if (model()->rowCount() == 0) {
        return;
    }
    QRect r = rectForIndex(model()->index(model()->rowCount() - 1, 0));
    QMargins margins = viewportMargins();

    int height = r.y() + r.height() + margins.top() + margins.bottom() + 1;

    if (!m_maxheight.has_value() || m_maxheight.value() > height) {
        setFixedHeight(height);
    }
}
