#ifndef MIRRORITEM_H
#define MIRRORITEM_H

#include "settingsitem.h"
#include <QLabel>
#include <QDebug>
#include <dimagebutton.h>
#include <types/mirrorinfolist.h>

DWIDGET_USE_NAMESPACE

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

    QSize sizeHint() const Q_DECL_OVERRIDE;

    inline void setMirrorName(const QString &name) {m_mirrorName->setText(name);}
    inline QString mirrorName() const { return m_mirrorName->text(); }

    void testMirrorSpeed(const QString &mirrorAdr);

signals:
    void selectStateChanged(bool state) const;
    void clicked(MirrorItem* item) const;

public slots:
    void testMirrorSpeed_finish(int ret);

protected:
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;

private:
    DImageButton *m_selectedBtn;
    QLabel *m_mirrorName;
    QLabel *m_mirrorSpeed;

    bool m_selected = false;
    MirrorInfo m_info;
};

}
}
#endif // MIRRORITEM_H
