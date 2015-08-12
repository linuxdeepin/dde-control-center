#include <libdui/dtextbutton.h>
#include <libdui/dseparatorhorizontal.h>
#include <libdui/dsearchedit.h>
#include <libdui/darrowlineexpand.h>
#include <libdui/dtextbutton.h>
#include<libdui/dbuttonlist.h>

#include "mainwidget.h"

MainWidget::MainWidget(QWidget *parent):
    QFrame(parent),
    m_layout(new QVBoxLayout()),
    m_header(new DHeaderLine()),
    m_dbus(new ShortcutDbus(this))
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    init();
}

MainWidget::~MainWidget()
{

}

DArrowLineExpand *addExpand(const QString &title, QWidget *widget)
{
    DArrowLineExpand *expand = new DArrowLineExpand;
    expand->setTitle(title);
    expand->setContent(widget);
    expand->setExpand(false);

    return expand;
}

QString shortcutTransfrom(const QString& str)
{
    if(str == "")
        return QObject::tr("No");

    QStringList shortnamelist = str.split("-");
    for(QString &keyname: shortnamelist){
        keyname[0]=keyname[0].toUpper();
    }
    return shortnamelist.join("+");
}

void MainWidget::init()
{
    m_layout->setMargin(0);

    m_header->setTitle(tr("Keyboard Shortcuts"));
    DTextButton *re_button = new DTextButton(tr("Reset"));
    connect(re_button, &DTextButton::clicked, m_dbus, &ShortcutDbus::Reset);
    m_header->setContent(re_button);

    SetShortcutList *tmp_systemList = new SetShortcutList;
    tmp_systemList->setFixedWidth(310);
    tmp_systemList->setItemSize(280, 30);
    ShortcutInfoList tmplist = m_dbus->systemList();
    foreach (const ShortcutInfo &info, tmplist) {
        tmp_systemList->addItem(info.id, info.title, shortcutTransfrom(info.shortcut));
    }

    SetShortcutList *tmp_windowList = new SetShortcutList;
    tmp_windowList->setFixedWidth(310);
    tmp_windowList->setItemSize(280, 30);
    tmplist = m_dbus->windowList();
    foreach (const ShortcutInfo &info, tmplist) {
        tmp_windowList->addItem(info.id, info.title, shortcutTransfrom(info.shortcut));
    }

    SetShortcutList *tmp_workspaceList = new SetShortcutList;
    tmp_workspaceList->setFixedWidth(310);
    tmp_workspaceList->setItemSize(280, 30);
    tmplist = m_dbus->workspaceList();
    foreach (const ShortcutInfo &info, tmplist) {
        tmp_workspaceList->addItem(info.id, info.title, shortcutTransfrom(info.shortcut));
    }

    SetShortcutList *tmp_customList = new SetShortcutList;
    tmp_customList->setFixedWidth(310);
    tmp_customList->setItemSize(280, 30);
    tmplist = m_dbus->customList();
    foreach (const ShortcutInfo &info, tmplist) {
        tmp_customList->addItem(info.id, info.title, shortcutTransfrom(info.shortcut));
    }

    connect(m_dbus, &ShortcutDbus::systemListChanged, [=](const ShortcutInfoList& list){
        shortcutListChanged(tmp_systemList, list);
    });
    connect(m_dbus, &ShortcutDbus::windowListChanged, [=](const ShortcutInfoList& list){
        shortcutListChanged(tmp_windowList, list);
    });
    connect(m_dbus, &ShortcutDbus::workspaceListChanged, [=](const ShortcutInfoList& list){
        shortcutListChanged(tmp_workspaceList, list);
    });
    connect(m_dbus, &ShortcutDbus::customListChanged, [=](const ShortcutInfoList& list){
        shortcutListChanged(tmp_customList, list);
    });

    m_layout->setSpacing(0);
    m_layout->addWidget(m_header);
    m_layout->addWidget(new DSeparatorHorizontal);
    m_layout->addWidget(new DSearchEdit);
    m_layout->addWidget(new DSeparatorHorizontal);
    m_layout->addWidget(addExpand(tr("System"), tmp_systemList));
    m_layout->addWidget(addExpand(tr("Window"), tmp_windowList));
    m_layout->addWidget(addExpand(tr("Workspace"), tmp_workspaceList));
    m_layout->addWidget(new QLabel(tr("Custom")));
    m_layout->addWidget(new DSeparatorHorizontal);
    m_layout->addWidget(tmp_customList);
    m_layout->addWidget(new DSeparatorHorizontal);
    m_layout->addStretch(1);

    setLayout(m_layout);
}

void MainWidget::shortcutListChanged(SetShortcutList *listw, const ShortcutInfoList &list)
{
    int min = qMin(listw->count(), list.count());

    for(int i=0;i<min;++i){
        const ShortcutInfo &info = list[i];
        listw->setItemId(i, info.id);
        listw->setItemTitle(i, info.title);
        listw->setItemShortcut(i, shortcutTransfrom(info.shortcut));
    }

    if(listw->count()<list.count()){
        for(int i=min;i<list.count();++i){
            const ShortcutInfo &info = list[i];
            listw->addItem(info.id, info.title, shortcutTransfrom(info.shortcut));
        }
    }else{
        for(int i=list.count();i<listw->count();++i){
            listw->removeItem(i);
        }
    }
}
