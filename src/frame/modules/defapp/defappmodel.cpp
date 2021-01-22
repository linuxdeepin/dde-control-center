/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#include "defappmodel.h"
#include <string.h>
#include "model/category.h"
using namespace dcc;
using namespace dcc::defapp;
DefAppModel::DefAppModel(QObject *parent)
    :QObject(parent)
{
    m_modBrowser = new Category(this);
    m_modMail = new Category(this);
    m_modText = new Category(this);
    m_modMusic = new Category(this);
    m_modVideo = new Category(this);
    m_modPicture = new Category(this);
    m_modTerminal = new Category(this);
}

DefAppModel::~DefAppModel()
{
    m_modBrowser->deleteLater();
    m_modMail->deleteLater();
    m_modText->deleteLater();
    m_modMusic->deleteLater();
    m_modVideo->deleteLater();
    m_modPicture->deleteLater();
    m_modTerminal->deleteLater();
}


