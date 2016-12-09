#ifndef GRUBBACKGROUNDITEM_H
#define GRUBBACKGROUNDITEM_H

#include "settingsitem.h"
#include <com_deepin_daemon_grub2_theme.h>
using namespace dcc;

using GrubThemeDbus=com::deepin::daemon::grub2::Theme;

namespace dcc{
namespace systeminfo{

class GrubBackgroundItem : public SettingsItem
{
    Q_OBJECT

public:
    explicit GrubBackgroundItem(QFrame* parent = 0);

protected:
    void paintEvent(QPaintEvent *e);
    void dragEnterEvent(QDragEnterEvent *e);
    void dragLeaveEvent(QDragLeaveEvent *e);
    void dropEvent(QDropEvent *e);

private:
    QPixmap m_background;
    GrubThemeDbus *m_themeDbus;
    bool m_isDrop;

    Q_SLOT bool updateBackground(const QString &filename);
    Q_SLOT void onProperty(const QString& property, const QVariant&  variant);
};

}
}
#endif // GRUBBACKGROUNDITEM_H
