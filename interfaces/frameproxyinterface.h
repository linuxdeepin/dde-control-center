/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
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

#ifndef FRAMEPROXYINTERFACE_H
#define FRAMEPROXYINTERFACE_H

#include <QString>

namespace dcc {
class ContentWidget;
class ModuleInterface;
}

namespace dcc {

class FrameProxyInterface
{
public:
    // Module request to into next page
    virtual void pushWidget(ModuleInterface * const inter, ContentWidget * const w) = 0;

    virtual void setFrameAutoHide(ModuleInterface * const inter, const bool autoHide) = 0;

    virtual void setModuleVisible(ModuleInterface * const inter, const bool visible) = 0;

    virtual void showModulePage(const QString &module, const QString &page, bool animation) = 0;
};

}

#endif // FRAMEPROXYINTERFACE_H
