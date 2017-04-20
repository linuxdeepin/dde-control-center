#ifndef GRUBBACKGROUNDITEM_H
#define GRUBBACKGROUNDITEM_H

#include "settingsitem.h"
#include <com_deepin_daemon_grub2_theme.h>
using namespace dcc::widgets;

using GrubThemeDbus=com::deepin::daemon::grub2::Theme;

namespace dcc{
namespace systeminfo{

class GrubBackgroundItem : public SettingsItem
{
    Q_OBJECT

public:
    explicit GrubBackgroundItem(QFrame* parent = 0);

signals:
    void requestEnableTheme(const bool state);

public slots:
    void setThemeEnable(const bool state);

protected:
    void paintEvent(QPaintEvent *e);
    void dragEnterEvent(QDragEnterEvent *e);
    void dragLeaveEvent(QDragLeaveEvent *e);
    void dropEvent(QDropEvent *e);

private:
    QPixmap m_background;
    GrubThemeDbus *m_themeDbus;
    bool m_isDrop;
    bool m_themeEnable;
    Q_SLOT bool updateBackground(const QString &filename);
    Q_SLOT void onProperty(const QString& property, const QVariant&  variant);
};

}
}
#endif // GRUBBACKGROUNDITEM_H
