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

#ifndef MODULEWIDGET_H
#define MODULEWIDGET_H

#include "widgets/labels/largelabel.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>

namespace dcc {
namespace widgets {
class LargeLabel;
}
}

namespace dcc {

class ModuleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModuleWidget();
    const QString title() const;
    void setTitle(const QString &title);

Q_SIGNALS:
    void headerClicked() const;

private:
    bool event(QEvent *event);

protected:
    QLabel *m_moduleIcon;
    dcc::widgets::LargeLabel *m_moduleTitle;
    QVBoxLayout *m_centralLayout;
};

}

#endif // MODULEWIDGET_H
