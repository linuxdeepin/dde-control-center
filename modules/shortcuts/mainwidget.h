#ifndef GRUBWIDGET_H
#define GRUBWIDGET_H

#include <QFrame>
#include <QVBoxLayout>

#include <libdui/libdui_global.h>
#include <libdui/dheaderline.h>

#include "shortcutdbus.h"
#include "setshortcutlist.h"
#include "searchlist.h"

DUI_USE_NAMESPACE

class MainWidget : public QFrame
{
public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

private:
    QVBoxLayout *m_layout;
    QVBoxLayout *m_childLayout;
    DHeaderLine *m_header;
    ShortcutDbus *m_dbus;
    SearchList *m_searchList;

    void init();

    Q_SLOT void shortcutListChanged(SetShortcutList *listw, const ShortcutInfoList &list, int offseIndex);
};

#endif // GRUBWIDGET_H
