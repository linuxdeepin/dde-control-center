/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
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

#ifndef MOUSEMODEL_H
#define MOUSEMODEL_H

#include <QObject>
#include <QDebug>

namespace dcc {
namespace mouse {
class MouseModelBase;
class MouseModelBaseSettings;
class MouseModelMouseSettings;
class MouseModelThinkpadSettings;
class MouseModel : public QObject
{
    Q_OBJECT
    friend class MouseWorker;
public:
    explicit MouseModel(QObject *parent = 0);
    ~MouseModel();
    MouseModelBaseSettings *getBaseSettings() const;
    MouseModelMouseSettings *getMouseSettings() const;
    MouseModelMouseSettings *getTouchSettings() const;
    MouseModelThinkpadSettings *getTrackSettings() const;

private:
    MouseModelBaseSettings     *m_baseSettings;
    MouseModelMouseSettings    *m_mouseSettings;
    MouseModelMouseSettings    *m_touchSettings;
    MouseModelThinkpadSettings *m_trackSettings;
};
}
}



#endif // MOUSEMODEL_H
