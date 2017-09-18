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

#ifndef LINEEDITWIDGET_H
#define LINEEDITWIDGET_H

#include "settingsitem.h"

#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>

namespace dcc {
namespace widgets {

class LineEditWidget : public SettingsItem
{
    Q_OBJECT

public:
    explicit LineEditWidget(QFrame *parent = 0);

    inline QLineEdit *textEdit() const { return m_edit; }
    inline QString text() const { return m_edit->text(); }
    void setTitleVisible(const bool visible) { m_title->setVisible(visible); }
    void addRightWidget(QWidget *widget);
    void setReadOnly(const bool state);

public slots:
    void setTitle(const QString &title);
    void setText(const QString &text);
    void setPlaceholderText(const QString &text);

protected:
    void mousePressEvent(QMouseEvent *e);

private:
    QLabel *m_title;
    QLineEdit *m_edit;
    QHBoxLayout *m_mainLayout;
};

}
}

#endif // LINEEDITWIDGET_H
