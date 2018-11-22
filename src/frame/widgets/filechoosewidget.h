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

#include <dimagebutton.h>

class QLabel;
class QLineEdit;

namespace dcc {

namespace widgets {

class FileChooseWidget : public SettingsItem
{
    Q_OBJECT

public:
    explicit FileChooseWidget(QWidget *parent = 0);

    QLineEdit *edit() const { return m_edit; }
    void setTitle(const QString &title);

Q_SIGNALS:
    void requestFrameKeepAutoHide(const bool autoHide) const;

private Q_SLOTS:
    void chooseFile();

private:
    QLabel *m_title;
    QLineEdit *m_edit;
    Dtk::Widget::DImageButton *m_btn;
};

}

}

#endif // FILECHOOSEWIDGET_H
