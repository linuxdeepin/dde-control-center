/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     listenerri <listenerri@gmail.com>
 *
 * Maintainer: listenerri <listenerri@gmail.com>
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

#ifndef ABSTRACTSECTION_H
#define ABSTRACTSECTION_H

#include "widgets/settingsgroup.h"
#include "widgets/contentwidget.h"

namespace dcc {
namespace network {

class AbstractSection : public dcc::widgets::SettingsGroup
{
    Q_OBJECT

public:
    explicit AbstractSection(QFrame *parent = 0);
    explicit AbstractSection(const QString &title, QFrame *parent = 0);
    virtual ~AbstractSection();

    virtual bool allInputValid() = 0;
    virtual void saveSettings() = 0;

Q_SIGNALS:
    void requestNextPage(ContentWidget * const page) const;
    void requestFrameAutoHide(const bool autoHide) const;
};

} /* network */
} /* dcc */

#endif /* ABSTRACTSECTION_H */
