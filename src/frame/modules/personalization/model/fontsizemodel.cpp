// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "fontsizemodel.h"

using namespace dcc;
using namespace dcc::personalization;

FontSizeModel::FontSizeModel(QObject *parent)
    : QObject(parent)
    , m_size(0)
{

}

void FontSizeModel::setFontSize(const int size)
{
    if (m_size!=size) {
        m_size = size;
        Q_EMIT sizeChanged(size);
    }
}
