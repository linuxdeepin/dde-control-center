#include <libdui/dtextbutton.h>
#include <libdui/dseparatorhorizontal.h>
#include <libdui/dsearchedit.h>
#include <libdui/darrowlineexpand.h>
#include <libdui/dtextbutton.h>
#include<libdui/dbuttonlist.h>

#include "mainwidget.h"
#include "setshortcutlist.h"

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
    m_header->setContent(re_button);

    SetShortcutList *tmp_systemList = new SetShortcutList;
    tmp_systemList->setFixedWidth(310);
    tmp_systemList->setItemSize(280, 30);
    ShortcutInfoList tmplist = m_dbus->systemList();
    foreach (const ShortcutInfo &info, tmplist) {
        tmp_systemList->addItem(info.title, shortcutTransfrom(info.shortcut));
    }

    SetShortcutList *tmp_windowList = new SetShortcutList;
    tmp_windowList->setFixedWidth(310);
    tmp_windowList->setItemSize(280, 30);
    tmplist = m_dbus->windowList();
    foreach (const ShortcutInfo &info, tmplist) {
        tmp_windowList->addItem(info.title, shortcutTransfrom(info.shortcut));
    }

    SetShortcutList *tmp_workspaceList = new SetShortcutList;
    tmp_workspaceList->setFixedWidth(310);
    tmp_workspaceList->setItemSize(280, 30);
    tmplist = m_dbus->workspaceList();
    foreach (const ShortcutInfo &info, tmplist) {
        tmp_workspaceList->addItem(info.title, shortcutTransfrom(info.shortcut));
    }

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
    m_layout->addStretch(1);

    setLayout(m_layout);
}
