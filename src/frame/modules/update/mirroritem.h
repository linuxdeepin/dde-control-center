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

#ifndef MIRRORITEM_H
#define MIRRORITEM_H

#include "widgets/settingsitem.h"
#include <QLabel>
#include <QDebug>
#include <dimagebutton.h>
#include <types/mirrorinfolist.h>

#include "widgets/labels/smalllabel.h"

DWIDGET_USE_NAMESPACE

class QHBoxLayout;

using namespace dcc::widgets;

namespace dcc{
namespace update{

class MirrorItem : public SettingsItem
{
    Q_OBJECT
    Q_PROPERTY(bool checked READ selected WRITE setSelected NOTIFY selectStateChanged)

public:
    explicit MirrorItem(QFrame *parent = 0);

    void setMirrorInfo(const MirrorInfo& info);
    MirrorInfo mirrorInfo() const { return m_info;}

    inline bool selected() const {return m_selected;}
    void setSelected(bool state);

    int speed() const;
    void setSpeed(const int speed);

    void setTesting();

    void setMirrorName(const QString &name);
    inline QString mirrorName() const { return m_mirrorName->text(); }


Q_SIGNALS:
    void selectStateChanged(bool state) const;
    void clicked(MirrorItem* item) const;

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;

private:
    bool m_selected = false;
    int m_speed = 0;
    MirrorInfo m_info;

    DImageButton *m_selectedBtn;
    SmallLabel *m_mirrorName;
    SmallLabel *m_mirrorSpeed;

    QHBoxLayout *m_layout;

    int calcSpeedTextWidth() const;
};

}
}
#endif // MIRRORITEM_H
