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

#ifndef OPTIONITEM_H
#define OPTIONITEM_H

#include "widgets/settingsitem.h"
#include "widgets/labels/normallabel.h"

#include <QHBoxLayout>
#include <QPointer>

namespace dcc {
namespace widgets {

class OptionItem : public SettingsItem
{
    Q_OBJECT

public:
    explicit OptionItem(QWidget *parent = 0);
    explicit OptionItem(QString title, bool selected, QWidget *parent = 0);

    void setTitle(const QString &title);
    const QString title() const { return m_title; }
    void setTitleFixedHeight(int height);
    void setTitleWidget(QWidget *titleWidget);
    void setContentWidget(QWidget *contentWidget);
    void setContentsMargins(int left, int top, int right, int bottom);

    inline bool selected() const { return m_selected; }
    void setSelected(bool selected);

Q_SIGNALS:
    void selectedChanged() const;

protected:
    void mouseReleaseEvent(QMouseEvent *e);

protected:
    QString m_title;
    bool m_selected;

    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_titleLayout;
    QWidget *m_titleFrame;
    QPointer<QWidget> m_titleWidget;
    QPointer<QWidget> m_contentWidget;
    QLabel *m_selectedIcon;
};

} // namespace widgets;
} // namespace dcc;

#endif // OPTIONITEM_H
