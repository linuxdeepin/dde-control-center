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

#ifndef FILECHOOSEWIDGET_H
#define FILECHOOSEWIDGET_H

#include "widgets/settingsitem.h"

#include <dfilechooseredit.h>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

namespace dcc {

namespace widgets {

class FileChooseWidget : public SettingsItem
{
    Q_OBJECT

public:
    explicit FileChooseWidget(QWidget *parent = nullptr);

    DTK_WIDGET_NAMESPACE::DFileChooserEdit *edit() const { return m_fileChooserEdit; }
    void setTitle(const QString &title);
    virtual void setIsErr(const bool err = true) override;
Q_SIGNALS:
    void requestFrameKeepAutoHide(const bool autoHide) const;

private:
    DTK_WIDGET_NAMESPACE::DFileChooserEdit *m_fileChooserEdit;
    QLabel *m_title;
};

}

}

#endif // FILECHOOSEWIDGET_H
