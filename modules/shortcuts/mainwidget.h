#ifndef GRUBWIDGET_H
#define GRUBWIDGET_H

#include <QFrame>
#include <QVBoxLayout>

#include <libdui/libdui_global.h>
#include <libdui/dheaderline.h>

#include "shortcutdbus.h"
#include "shortcutwidget.h"
#include "searchlist.h"

DUI_USE_NAMESPACE

class MainWidget : public QFrame
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

signals:
    void addCustomShortcut();
    void addCustomShortcutFinished();

private:
    QVBoxLayout *m_layout;
    QVBoxLayout *m_childLayout;
    DHeaderLine *m_header;
    ShortcutDbus *m_dbus;
    SearchList *m_searchList;
    SearchList *m_systemList;
    SearchList *m_windowList;
    SearchList *m_workspaceList;
    SearchList *m_customList;

    void init();

    SearchList *addSearchList(const ShortcutInfoList &tmplist);
    Q_SLOT void shortcutListChanged(SearchList *listw, const ShortcutInfoList &list, int offseIndex);
    Q_SLOT void editShortcut(ShortcutWidget *w, SearchList *listw, const QString &flag, QString shortcut);
    Q_SLOT void removeShortcut();
    QWidget *getAddShortcutWidget();
    QWidget *getCustomLstHeadBar(SearchList *shorcutlist);
};

#endif // GRUBWIDGET_H
