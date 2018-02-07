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

#ifndef QUICKSWITCHBUTTON_H
#define QUICKSWITCHBUTTON_H

#include <QLabel>

namespace dcc {

class QuickSwitchButton : public QLabel
{
    Q_OBJECT
public:
    explicit QuickSwitchButton(const int index, const QString &iconName, QWidget *parent = 0);

    Q_PROPERTY(QString themeName READ themeName WRITE setThemeName)

    QString themeName() const;
    void setThemeName(const QString &themeName);

    bool checked() const { return m_checked; }

signals:
    void hovered(const int index) const;
    void clicked(const int index) const;
    void checkedChanged(const bool checked) const;

public slots:
    void setChecked(const bool checked);
    void setCheckable(const bool checkable);
    void setSelected(const bool selected);
    void setBackgroundVisible(const bool visible);

protected:
    void mouseReleaseEvent(QMouseEvent *e);
    void enterEvent(QEvent *e);
    void paintEvent(QPaintEvent *e);

private:
    QPixmap normalPixmap() const;
    QPixmap hoverPixmap() const;
    QPixmap pressedPixmap() const;

private:
    const int m_index;

    bool m_selected;
    bool m_checked;
    bool m_checkable;
    bool m_showBackground;
    QString m_themeName;
    const QString m_iconName;
};

}

#endif //
