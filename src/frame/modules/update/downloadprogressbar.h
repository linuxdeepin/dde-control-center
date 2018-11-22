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

#ifndef DOWNLOADPROGRESSBAR_H
#define DOWNLOADPROGRESSBAR_H

#include <QWidget>

#include "common.h"

namespace dcc{
namespace update{

class DownloadProgressBar : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadProgressBar(QWidget* parent = 0);

    void setMessage(const QString &message);
    void setValue(const int progress);

    inline const QString text() const { return m_message; }
    inline int value() const { return m_currentValue; }
    inline int minimum() const { return 0; }
    inline int maximum() const { return 100; }

protected:
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

Q_SIGNALS:
    void clicked();

private:
    int m_currentValue;
    QString m_message;
};

}
}
#endif // DOWNLOADPROGRESSBAR_H
