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

#include "imagenamebutton.h"
#include "addrmdoneline.h"
#include "moduleheader.h"
#include "searchlist.h"

#include "mainwidget.h"
#include "tooltip.h"
#include "selectdialog.h"

DUI_USE_NAMESPACE

MainWidget::MainWidget(QWidget *parent):
    QFrame(parent),
    m_layout(new QVBoxLayout),
    m_childLayout(new QVBoxLayout),
    m_header(new ModuleHeader(tr("Keyboard Shortcuts"))),
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

    return expand;
}

QString shortcutTransfrom(const QString& str)
{
    if(str == "")
        return QObject::tr("None");

    QStringList shortnamelist = str.split("-");
    for(QString &keyname: shortnamelist){
        if(keyname.count()>0)
            keyname[0]=keyname[0].toUpper();
    }
    return shortnamelist.join("+");
}

SearchList *MainWidget::addSearchList(const ShortcutInfoList &tmplist)
{
    SearchList *list = new SearchList;
    list->setFixedWidth(310);
    list->setItemSize(310, RADIO_ITEM_HEIGHT);
    foreach (const ShortcutInfo &info, tmplist) {
        ShortcutWidget *tmpw = new ShortcutWidget(info.id, info.title, shortcutTransfrom(info.shortcut));
        ShortcutWidget *shortw = new ShortcutWidget(info.id, info.title, shortcutTransfrom(info.shortcut));
        m_searchList->addItem(shortw);
        list->addItem(tmpw);

        connect(tmpw, &ShortcutWidget::shortcutChanged, [=](const QString &flag, const QString &shortcut){
            editShortcut(tmpw, list, flag, shortcut);
        });
        connect(shortw, &ShortcutWidget::shortcutChanged, [=](const QString &flag, const QString &shortcut){
            editShortcut(shortw, m_searchList, flag, shortcut);
        });
        connect(tmpw, &ShortcutWidget::removeShortcut, this, &MainWidget::removeShortcut);
    }

    return list;
}

QWidget *MainWidget::getAddShortcutWidget()
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

    connect(button_cancle, &DTextButton::clicked, [=](){
        edit_name->clear();
        edit_command->clear();
        w->hide();
        emit addCustomShortcutFinished();
    });

    connect(button_add, &DTextButton::clicked, [=](){
        if(!edit_name->text().isEmpty()&&!edit_command->text().isEmpty()){
            m_dbus->AddCustomShortcut(edit_name->text(), edit_command->text());
            w->hide();
            emit addCustomShortcutFinished();
        }
    });

    connect(this, SIGNAL(addCustomShortcut()), w, SLOT(show()));

    edit_name->setFixedWidth(200);
    edit_command->setFixedWidth(200);

    hlayout1->addSpacing(HEADER_LEFT_MARGIN);
    hlayout1->addStretch(1);
    hlayout1->addWidget(new QLabel(QObject::tr("Name")));
    hlayout1->addWidget(edit_name);
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

AddRmDoneLine *MainWidget::getCustomLstHeadBar()
{
    AddRmDoneLine *w = new AddRmDoneLine;
    w->setRmButtonToolTip(tr("Delete Shortcut"));
    w->setAddButtonToolTip(tr("Add Shortcut"));
    w->setTitle(tr("Custom"));


    connect(w, &AddRmDoneLine::addClicked, [=](){
        w->setAddHidden(true);
        w->setRemoveHidden(true);
        emit addCustomShortcut();
    });
    connect(w, &AddRmDoneLine::removeClicked, [=](){
        w->setAddHidden(true);
        w->setRemoveHidden(true);
        w->setDoneHidden(false);

        for(int i=0;i<m_customList->count();++i){
            ShortcutWidget *w = qobject_cast<ShortcutWidget*>(m_customList->getItem(i)->widget());
            if(w){
                w->showRemoveButton();
            }
        }
    });
    connect(w, &AddRmDoneLine::doneClicked, [=](){
        w->setAddHidden(false);
        w->setRemoveHidden(m_customList->count()<1);
        w->setDoneHidden(true);

        for(int i=0;i<m_customList->count();++i){
            ShortcutWidget *w = qobject_cast<ShortcutWidget*>(m_customList->getItem(i)->widget());
            if(w){
                w->hideRemoveButton();
            }
        }
    });

    connect(this, &AccountMainWidget::addCustomShortcutFinished, [=]{
        w->setAddHidden(false);
        w->setRemoveHidden(m_customList->count()<1);
    });

    return w;
}

void MainWidget::init()
{
    m_layout->setMargin(0);

    connect(m_header, &ModuleHeader::resetButtonClicked, m_dbus, &ShortcutDbus::Reset);

    m_searchList->hide();
    m_searchList->setItemSize(310, RADIO_ITEM_HEIGHT);

    m_systemList = addSearchList(m_dbus->systemList());
    m_windowList = addSearchList(m_dbus->windowList());
    m_workspaceList = addSearchList(m_dbus->workspaceList());
    m_customList = addSearchList(m_dbus->customList());

    m_systemList->setObjectName("System");
    m_windowList->setObjectName("Window");
    m_workspaceList->setObjectName("Workspace");
    m_customList->setObjectName("Custom");

    connect(m_dbus, &ShortcutDbus::systemListChanged, [=](const ShortcutInfoList& list){
        shortcutListChanged(m_systemList, list, 0);
    });
    connect(m_dbus, &ShortcutDbus::windowListChanged, [=](const ShortcutInfoList& list){
        shortcutListChanged(m_windowList, list, m_systemList->count());
    });
    connect(m_dbus, &ShortcutDbus::workspaceListChanged, [=](const ShortcutInfoList& list){
        shortcutListChanged(m_workspaceList, list, m_systemList->count()+m_windowList->count());
    });
    connect(m_dbus, &ShortcutDbus::customListChanged, [=](const ShortcutInfoList& list){
        shortcutListChanged(m_customList, list,
                            m_systemList->count()+m_windowList->count()+m_workspaceList->count());
    });

    DSearchEdit *edit  = new DSearchEdit;
    connect(edit, &DSearchEdit::textChanged, m_searchList, [=]{
        QString str = edit->text();

        if(!str.isEmpty()){
            if(!m_searchList->isSearching()){
                m_searchList->beginSearch();
                for(int i=0;i<m_childLayout->count();++i){
                    QWidget *w = m_childLayout->itemAt(i)->widget();
                    if(w)
                        w->hide();
                }
                emit addCustomShortcutFinished();
                m_searchList->show();
            }

            m_searchList->setKeyWord(str);
        }else{
            m_searchList->endSearch();
            m_searchList->hide();
            for(int i=0;i<m_childLayout->count()-1;++i){
                QWidget *w = m_childLayout->itemAt(i)->widget();
                if(w)
                    w->show();
            }
        }
    });

    AddRmDoneLine *customLine = getCustomLstHeadBar();
    customLine->setRemoveHidden(m_customList->count()<1);

    DSeparatorHorizontal *customListSeparator = new DSeparatorHorizontal;
    customListSeparator->setHidden(m_customList->count()<1);
    connect(m_customList, &SearchList::countChanged, [=]{
        customListSeparator->setHidden(m_customList->count()<1);
        if(customLine->doneButton()->isHidden())
            customLine->setRemoveHidden(m_customList->count()<1);
    });

    m_layout->setSpacing(0);
    m_layout->addWidget(m_header);
    m_layout->addWidget(new DSeparatorHorizontal);
    m_layout->addWidget(edit);
    m_layout->addWidget(new DSeparatorHorizontal);
    m_layout->addWidget(m_searchList, 10);
    m_childLayout->setMargin(0);
    m_childLayout->addWidget(addExpand(tr("System"), m_systemList));
    m_childLayout->addWidget(addExpand(tr("Window"), m_windowList));
    m_childLayout->addWidget(addExpand(tr("Workspace"), m_workspaceList));
    m_childLayout->addWidget(customLine);
    m_childLayout->addWidget(new DSeparatorHorizontal);
    m_childLayout->addWidget(m_customList);
    m_childLayout->addWidget(customListSeparator);
    m_childLayout->addSpacing(5);
    m_childLayout->addWidget(getAddShortcutWidget());
    m_layout->addLayout(m_childLayout);
    m_layout->addStretch(1);

    setLayout(m_layout);
}

void MainWidget::shortcutListChanged(SearchList *listw, const ShortcutInfoList &list, int offseIndex)
{
    int min = qMin(listw->count(), list.count());

    for(int i=0;i<min;++i){
        const ShortcutInfo &info = list[i];
        listw->setItemData(i,
                           QVariantList()<<info.id<<info.title<<shortcutTransfrom(info.shortcut));
        m_searchList->setItemData(offseIndex+i,
                                  QVariantList()<<info.id<<info.title<<shortcutTransfrom(info.shortcut));
    }

    if(listw->count()<list.count()){
        for(int i=min;i<list.count();++i){
            const ShortcutInfo &info = list[i];
            ShortcutWidget *tmpw = new ShortcutWidget(info.id, info.title, shortcutTransfrom(info.shortcut));
            ShortcutWidget *shortw = new ShortcutWidget(info.id, info.title, shortcutTransfrom(info.shortcut));
            m_searchList->insertItem(offseIndex+i, shortw);
            listw->addItem(tmpw);
            connect(tmpw, &ShortcutWidget::shortcutChanged, [=](const QString& flag, const QString &shortcut){
                editShortcut(tmpw, listw, flag, shortcut);
            });
            connect(shortw, &ShortcutWidget::shortcutChanged, [=](const QString& flag, const QString &shortcut){
                editShortcut(shortw, m_searchList, flag, shortcut);
            });
            connect(tmpw, &ShortcutWidget::removeShortcut, this, &MainWidget::removeShortcut);
        }
    }else{
        for(int i=list.count();i<listw->count();++i){
            listw->removeItem(i);
            m_searchList->removeItem(offseIndex+i);
        }
    }
}

void MainWidget::editShortcut(ShortcutWidget *w, SearchList *listw, const QString &flag, QString shortcut)
{
    if(!w||!listw)
        return;

    if(flag == "Valid"){
        m_dbus->ModifyShortcut(w->id(), shortcut);
        return;
    }

    int index = listw->indexOf(qobject_cast<QWidget*>(w));

    if(index<0||index>listw->count()-1)
        return;

    if(flag == "Invalid"){
        ToolTip *label = new ToolTip;
        label->setText(tr("Shortcut \"%1\" is invalid, please retype new shortcut.")
                       .arg(shortcut));
        listw->insertItem(index+1, label);
        listw->setMinimumHeight(listw->minimumHeight()+label->sizeHint().height());
        label->setTimeout(2000);
        label->expansion();
        connect(label, &ToolTip::contracted, [=]{
            listw->removeItem(listw->indexOf(qobject_cast<QWidget*>(label)));
        });
    }else if(flag == "Conflict"){
        QList<ShortcutWidget*> tmp_list;

        QString tmp_shortcut = shortcutTransfrom(shortcut);
        QString tmp_text = tr("The shortcut you set ");

        QList<SearchList*> tmp_searchlist;
        tmp_searchlist<<m_systemList<<m_windowList<<m_workspaceList<<m_customList;

        foreach (SearchList* list, tmp_searchlist) {
            for (int i=0; i<list->count(); ++i) {
                ShortcutWidget *tmp_w = qobject_cast<ShortcutWidget*>(list->getItem(i)->widget());
                if(tmp_w&&tmp_w->shortcut() == tmp_shortcut){
                    tmp_list << tmp_w;
                    tmp_text += tr("conflicts with the one used for \"%2\" in the \"%1\" category.")
                            .arg(list->objectName()).arg(tmp_w->title());
                }
            }
        }

        tmp_text.append(tr("Do you want to replace it?"));

        SelectDialog *dialog = new SelectDialog;
        dialog->setText(tmp_text);
        listw->insertItem(index+1, dialog);
        listw->setMinimumHeight(listw->minimumHeight()+dialog->sizeHint().height());

        connect(dialog, &SelectDialog::replace, [=]{
            listw->removeItem(listw->indexOf(qobject_cast<QWidget*>(dialog)));

            foreach (ShortcutWidget* tmp_w, tmp_list) {
                m_dbus->ModifyShortcut(tmp_w->id(), "");
            }
            m_dbus->ModifyShortcut(w->id(), shortcut);
        });
        connect(dialog, &SelectDialog::cancel, [=]{
            dialog->contraction();
        });
        connect(dialog, &SelectDialog::contracted, [=]{
            listw->removeItem(listw->indexOf(qobject_cast<QWidget*>(dialog)));
        });
        dialog->expansion();
    }
}

void MainWidget::removeShortcut()
{
    ShortcutWidget *w = qobject_cast<ShortcutWidget*>(sender());
    if(w){
        m_dbus->DeleteCustomShortcut(w->id());
    }
}
