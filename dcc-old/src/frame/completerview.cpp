// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "completerview.h"

DGUI_USE_NAMESPACE

namespace DCC_NAMESPACE {
const int MARGIN = 8;
const int COMPACT_MARGIN = 4;

CompleterView::CompleterView(QWidget *parent)
    : DListView(parent)
    , m_sizeMode(DGuiApplicationHelper::instance()->sizeMode())
{
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::sizeModeChanged, this, [this](DGuiApplicationHelper::SizeMode sizeMode) {
        if (sizeMode == m_sizeMode)
            return;

        m_sizeMode = sizeMode;
        updateViewportMargins();
    });

    setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    updateViewportMargins();
}

void CompleterView::updateViewportMargins()
{
    (m_sizeMode == DGuiApplicationHelper::CompactMode) ? setViewportMargins(0, COMPACT_MARGIN, 0, COMPACT_MARGIN) : setViewportMargins(0, MARGIN, 0, MARGIN);
}

int CompleterView::margin() const
{
    const static int compactOffset = -1;
    const static int normalOffset = 2;
    return m_sizeMode == DGuiApplicationHelper::CompactMode ? COMPACT_MARGIN + compactOffset : MARGIN + normalOffset;
}
} // namespace DCC_NAMESPACE
