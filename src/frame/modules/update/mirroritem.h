// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef MIRRORITEM_H
#define MIRRORITEM_H

#include "widgets/settingsitem.h"
#include "widgets/labels/smalllabel.h"

#include <types/mirrorinfolist.h>
#include <DFloatingButton>

DWIDGET_USE_NAMESPACE

class QHBoxLayout;

using namespace dcc::widgets;

namespace dcc {
namespace update {

class MirrorItem : public SettingsItem
{
    Q_OBJECT
    Q_PROPERTY(bool checked READ selected WRITE setSelected NOTIFY selectStateChanged)

public:
    explicit MirrorItem(QFrame *parent = nullptr);

    void setMirrorInfo(const MirrorInfo &info);
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
    void clicked(MirrorItem *item) const;

protected:
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;

private:
    int calcSpeedTextWidth() const;

private:
    bool m_selected = false;
    int m_speed = 0;
    MirrorInfo m_info;
    DFloatingButton *m_selectedBtn;
    SmallLabel *m_mirrorName;
    SmallLabel *m_mirrorSpeed;
    QHBoxLayout *m_layout;
};

}
}
#endif // MIRRORITEM_H
