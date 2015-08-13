#include <QTimer>

#include <libdui/dtextbutton.h>
#include <libdui/dseparatorhorizontal.h>
#include <libdui/dsearchedit.h>
#include <libdui/darrowlineexpand.h>
#include <libdui/dtextbutton.h>
#include <libdui/dbuttonlist.h>
#include <libdui/dlineedit.h>
#include <libdui/dimagebutton.h>
#include <libdui/dthememanager.h>
#include <libdui/libdui_global.h>

#include "mainwidget.h"
#include "dynamiclabel.h"

DUI_USE_NAMESPACE

MainWidget::MainWidget(QWidget *parent):
    QFrame(parent),
    m_layout(new QVBoxLayout),
    m_childLayout(new QVBoxLayout),
    m_header(new DHeaderLine),
    m_dbus(new ShortcutDbus(this)),
    m_searchList(new SearchList)
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
        return QObject::tr("None");

    QStringList shortnamelist = str.split("-");
    for(QString &keyname: shortnamelist){
        keyname[0]=keyname[0].toUpper();
    }
    return shortnamelist.join("+");
}

SetShortcutList *MainWidget::addSetShortcutList(const ShortcutInfoList &tmplist)
{
    SetShortcutList *list = new SetShortcutList;
    list->setFixedWidth(310);
    list->setItemSize(310, RADIO_ITEM_HEIGHT);
    foreach (const ShortcutInfo &info, tmplist) {
        list->addItem(info.id, info.title, shortcutTransfrom(info.shortcut));
        ShortcutWidget *shortw = new ShortcutWidget(info.id, info.title, shortcutTransfrom(info.shortcut));
        m_searchList->addItem(shortw);

        connect(list->getItemWidget(list->count()-1), &ShortcutWidget::shortcutChanged, this, &MainWidget::editShortcut);
        connect(shortw, &ShortcutWidget::shortcutChanged, this, &MainWidget::editShortcut);
        connect(list->getItemWidget(list->count()-1), &ShortcutWidget::removeShortcut, this, &MainWidget::removeShortcut);
    }

    return list;
}

QWidget *getAddShortcutWidget(ShortcutDbus *dbus)
{
    QWidget *w = new QWidget;
    w->hide();

    QHBoxLayout *hlayout1 = new QHBoxLayout;
    QHBoxLayout *hlayout2 = new QHBoxLayout;
    QHBoxLayout *hlayout3 = new QHBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;
    DLineEdit *edit_name = new DLineEdit;
    DLineEdit *edit_command= new DLineEdit;
    DTextButton *button_cancle = new DTextButton(QObject::tr("Cancle"));
    DTextButton *button_add = new DTextButton(QObject::tr("Add"));
    button_cancle->setFixedSize(TEXT_BUTTON_MIN_WIDTH, BUTTON_HEIGHT);
    button_add->setFixedSize(TEXT_BUTTON_MIN_WIDTH, BUTTON_HEIGHT);
    button_cancle->setObjectName("CanleButon");
    button_add->setObjectName("AddButon");

    QObject::connect(button_cancle, &DTextButton::clicked, [=](){
        edit_name->clear();
        edit_command->clear();
        w->hide();
    });

    QObject::connect(button_add, &DTextButton::clicked, [=](){
        if(!edit_name->text().isEmpty()&&!edit_command->text().isEmpty())
            dbus->AddCustomShortcut(edit_name->text(), edit_command->text());
        w->hide();
    });

    hlayout1->addSpacing(HEADER_LEFT_MARGIN);
    hlayout1->addStretch(1);
    hlayout1->addWidget(new QLabel(QObject::tr("Name")));
    hlayout1->addWidget(edit_name, 9);
    hlayout1->addSpacing(HEADER_RIGHT_MARGIN);
    hlayout2->addSpacing(HEADER_RIGHT_MARGIN);
    hlayout2->addWidget(new QLabel(QObject::tr("Command")));
    hlayout2->addWidget(edit_command);
    hlayout2->addSpacing(HEADER_RIGHT_MARGIN);
    hlayout3->addStretch(1);
    hlayout3->addWidget(button_cancle);
    hlayout3->addWidget(button_add);
    hlayout3->addSpacing(HEADER_RIGHT_MARGIN);

    vlayout->setMargin(0);
    vlayout->addLayout(hlayout1);
    vlayout->addLayout(hlayout2);
    vlayout->addLayout(hlayout3);
    vlayout->addWidget(new DSeparatorHorizontal);

    w->setStyleSheet("QLabel{color: #b4b4b4;font-size: 12px;}");
    w->setLayout(vlayout);

    return w;
}

QWidget *getCustomLstHeadBar(SetShortcutList *shorcutlist, QWidget *addShortcutWidget)
{
    QWidget *w = new QWidget;

    QString icon_path = "modules/icons/";
    icon_path = icon_path.append(DThemeManager::instance()->theme().append("/"));

    QHBoxLayout *layout = new QHBoxLayout;
    DynamicLabel *dl = new DynamicLabel;
    DImageButton *button_remove = new DImageButton(
                icon_path+"delete_normal.png", icon_path+"delete_hover.png", icon_path+"delete_press.png");
    DImageButton *button_add = new DImageButton(
                icon_path+"add_normal.png", icon_path+"add_hover.png", icon_path+"add_press.png");
    DTextButton *button_done = new DTextButton(QObject::tr("Done"));

    button_add->setFixedHeight(BUTTON_HEIGHT);
    button_done->hide();

    dl->setEasingType(QEasingCurve::OutQuint);

    QObject::connect(button_add, &DImageButton::clicked, [=](){
        button_add->hide();
        button_remove->hide();
        addShortcutWidget->show();
    });
    QObject::connect(button_remove, &DImageButton::clicked, [=](){
        button_add->hide();
        button_remove->hide();
        button_done->show();

        for(int i=0;i<shorcutlist->count();++i){
            shorcutlist->getItemWidget(i)->showRemoveButton();
        }
    });
    QObject::connect(button_done, &DTextButton::clicked, [=](){
        button_done->hide();
        button_add->show();
        button_remove->show();

        for(int i=0;i<shorcutlist->count();++i){
            shorcutlist->getItemWidget(i)->hideRemoveButton();
        }
    });
    QObject::connect(button_remove, &DImageButton::stateChanged, [=](){
        if(button_remove->getState() == DImageButton::Hover){
            dl->setText(QObject::tr("Delete Shortcut"));
            dl->showLabel();
        }else{
            dl->hideLabel();
        }
    });
    QObject::connect(button_add, &DImageButton::stateChanged, [=](){
        if(button_add->getState() == DImageButton::Hover){
            dl->setText(QObject::tr("Add Shortcut"));
            dl->showLabel();
        }else{
            dl->hideLabel();
        }
    });

    DTextButton *button_canle = addShortcutWidget->findChild<DTextButton*>("CanleButon");
    QObject::connect(button_canle, &DTextButton::clicked, [=](){
        button_add->show();
        button_remove->show();
    });
    DTextButton *button_ok = addShortcutWidget->findChild<DTextButton*>("AddButon");
    QObject::connect(button_ok, &DTextButton::clicked, [=](){
        button_add->show();
        button_remove->show();
    });

    layout->setMargin(0);
    layout->addSpacing(HEADER_LEFT_MARGIN);
    layout->addWidget(new QLabel(QObject::tr("Custom")));
    layout->addStretch(1);
    layout->addWidget(dl);
    layout->addWidget(button_remove);
    layout->addWidget(button_add);
    layout->addWidget(button_done);
    layout->addSpacing(HEADER_RIGHT_MARGIN);

    w->setFixedHeight(EXPAND_HEADER_HEIGHT);
    w->setLayout(layout);
    w->setStyleSheet("QLabel{color: #b4b4b4;font-size: 12px;}");

    return w;
}

void MainWidget::init()
{
    m_layout->setMargin(0);

    m_header->setTitle(tr("Keyboard Shortcuts"));
    DTextButton *re_button = new DTextButton(tr("Reset"));
    connect(re_button, &DTextButton::clicked, m_dbus, &ShortcutDbus::Reset);
    m_header->setContent(re_button);

    m_searchList->hide();
    m_searchList->listWidget()->setSpacing(10);

    SetShortcutList *tmp_systemList = addSetShortcutList(m_dbus->systemList());
    SetShortcutList *tmp_windowList = addSetShortcutList(m_dbus->windowList());
    SetShortcutList *tmp_workspaceList = addSetShortcutList(m_dbus->workspaceList());
    SetShortcutList *tmp_customList = addSetShortcutList(m_dbus->customList());

    connect(m_dbus, &ShortcutDbus::systemListChanged, [=](const ShortcutInfoList& list){
        shortcutListChanged(tmp_systemList, list, 0);
    });
    connect(m_dbus, &ShortcutDbus::windowListChanged, [=](const ShortcutInfoList& list){
        shortcutListChanged(tmp_windowList, list, tmp_systemList->count());
    });
    connect(m_dbus, &ShortcutDbus::workspaceListChanged, [=](const ShortcutInfoList& list){
        shortcutListChanged(tmp_workspaceList, list, tmp_systemList->count()+tmp_windowList->count());
    });
    connect(m_dbus, &ShortcutDbus::customListChanged, [=](const ShortcutInfoList& list){
        shortcutListChanged(tmp_customList, list,
                            tmp_systemList->count()+tmp_windowList->count()+tmp_workspaceList->count());
    });

    QLineEdit *edit  = new QLineEdit;
    connect(edit, &QLineEdit::textChanged, m_searchList, [=](const QString &str){
        if(!str.isEmpty()){
            if(!m_searchList->isSearching()){
                m_searchList->beginSearch();
                for(int i=0;i<m_childLayout->count();++i){
                    m_childLayout->itemAt(i)->widget()->hide();
                }
                m_searchList->show();
            }

            m_searchList->setKeyWord(str);
        }else{
            m_searchList->endSearch();
            m_searchList->hide();
            for(int i=0;i<m_childLayout->count();++i){
                m_childLayout->itemAt(i)->widget()->show();
            }
        }
    });

    QWidget *addShortcutWidget = getAddShortcutWidget(m_dbus);

    m_layout->setSpacing(0);
    m_layout->addWidget(m_header);
    m_layout->addWidget(new DSeparatorHorizontal);
    m_layout->addWidget(edit);
    m_layout->addWidget(new DSeparatorHorizontal);
    m_layout->addWidget(m_searchList, 10);
    m_childLayout->setMargin(0);
    m_childLayout->addWidget(addExpand(tr("System"), tmp_systemList));
    m_childLayout->addWidget(addExpand(tr("Window"), tmp_windowList));
    m_childLayout->addWidget(addExpand(tr("Workspace"), tmp_workspaceList));
    m_childLayout->addWidget(getCustomLstHeadBar(tmp_customList, addShortcutWidget));
    m_childLayout->addWidget(new DSeparatorHorizontal);
    m_childLayout->addWidget(tmp_customList);
    m_childLayout->addWidget(new DSeparatorHorizontal);
    m_childLayout->addWidget(addShortcutWidget);
    m_layout->addLayout(m_childLayout);
    m_layout->addStretch(1);

    setLayout(m_layout);
}

void MainWidget::shortcutListChanged(SetShortcutList *listw, const ShortcutInfoList &list, int offseIndex)
{
    int min = qMin(listw->count(), list.count());

    for(int i=0;i<min;++i){
        const ShortcutInfo &info = list[i];
        listw->setItemId(i, info.id);
        listw->setItemTitle(i, info.title);
        listw->setItemShortcut(i, shortcutTransfrom(info.shortcut));
        m_searchList->setItemData(offseIndex+i,
                                  QVariantList()<<info.id<<info.title<<shortcutTransfrom(info.shortcut));
    }

    if(listw->count()<list.count()){
        for(int i=min;i<list.count();++i){
            const ShortcutInfo &info = list[i];
            listw->addItem(info.id, info.title, shortcutTransfrom(info.shortcut));
            m_searchList->insertItem(offseIndex+i,
                                      new ShortcutWidget(info.id, info.title, shortcutTransfrom(info.shortcut)));
        }
    }else{
        for(int i=list.count();i<listw->count();++i){
            listw->removeItem(i);
            m_searchList->removeItem(offseIndex+i);
        }
    }
}

void MainWidget::removeShortcut()
{
    qDebug()<<"fdasssssssssss";

    ShortcutWidget *w = qobject_cast<ShortcutWidget*>(sender());
    qDebug()<<w->id();
    if(w){
        m_dbus->DeleteCustomShortcut(w->id());
    }
}

void MainWidget::editShortcut(const QString &shortcut)
{
    ShortcutWidget *w = qobject_cast<ShortcutWidget*>(sender());
    if(w){
        m_dbus->ModifyShortcut(w->id(), shortcut);
    }
}
