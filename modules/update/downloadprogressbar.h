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

#ifndef DOWNLOADPROGRESSBAR_H
#define DOWNLOADPROGRESSBAR_H

#include <QProgressBar>

#include "common.h"

namespace dcc {
namespace widgets {
class NormalLabel;
}
}

namespace dcc{
namespace update{

class DownloadProgressBar : public QProgressBar
{
    Q_OBJECT

public:
    explicit DownloadProgressBar(QWidget* parent = 0);

    void setMessage(const QString &message);

protected:
    void mouseReleaseEvent(QMouseEvent *e);

signals:
    void clicked();

private:
    dcc::widgets::NormalLabel *m_message;
};

}
}
#endif // DOWNLOADPROGRESSBAR_H
