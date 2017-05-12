#ifndef MIRRORITEM_H
#define MIRRORITEM_H

#include "settingsitem.h"
#include <QLabel>
#include <QDebug>
#include <dimagebutton.h>
#include <types/mirrorinfolist.h>

#include "labels/smalllabel.h"

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


signals:
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
