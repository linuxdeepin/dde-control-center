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

#ifndef DEFAPPMODEL_H
#define DEFAPPMODEL_H

#include <QObject>
#include <QDebug>
namespace dcc
{
namespace defapp
{
class Category;
class DefAppModel : public QObject
{
    Q_OBJECT
public:
    explicit DefAppModel(QObject *parent = 0);
    ~DefAppModel();

    Category *getModBrowser()     {return m_modBrowser;}
    Category *getModMail()        {return m_modMail;}
    Category *getModText()        {return m_modText;}
    Category *getModMusic()       {return m_modMusic;}
    Category *getModVideo()       {return m_modVideo;}
    Category *getModPicture()     {return m_modPicture;}
    Category *getModTerminal()    {return m_modTerminal;}

private:
    Category *m_modBrowser;
    Category *m_modMail;
    Category *m_modText;
    Category *m_modMusic;
    Category *m_modVideo;
    Category *m_modPicture;
    Category *m_modTerminal;
};
}
}



#endif // DEFAPPMODEL_H
