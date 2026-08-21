//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "fontsizemodel.h"


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
