/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QScrollBar>

#include <dtextbutton.h>
#include <dseparatorhorizontal.h>
#include <dsearchedit.h>
#include <dbuttonlist.h>
#include <dlineedit.h>
#include <dimagebutton.h>
#include <dthememanager.h>
#include <dwidget_global.h>

#include "imagenamebutton.h"
#include "addrmdoneline.h"
#include "moduleheader.h"
#include "searchlist.h"
#include "constants.h"

#include "mainwidget.h"
#include "tooltip.h"
#include "selectdialog.h"

DWIDGET_USE_NAMESPACE

#define LIST_MAX_HEIGHT 500

MainWidget::MainWidget(QWidget *parent):
    ScrollFrame(parent),
    m_childLayout(new QVBoxLayout),
    m_header(new ModuleHeader(tr("Keyboard Shortcuts"))),
    m_dbus(new ShortcutDbus(this)),
    m_searchList(new SearchList),
    m_expandGroup(new DExpandGroup(this))
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    init();
}

MainWidget::~MainWidget()
{

}

DArrowLineExpand *MainWidget::addExpand(const QString &title, QWidget *widget)
{
    DArrowLineExpand *expand = new DArrowLineExpand;
    expand->setTitle(title);
    expand->setContent(widget);
    m_expandGroup->addExpand(expand);
    m_expandGroupChildList << widget;

    return expand;
}

SearchList *MainWidget::addSearchList(const ShortcutInfoList &tmplist)
{
    SearchList *list = new SearchList;
    list->setFixedWidth(DCC::ModuleContentWidth);
    list->setItemSize(DCC::ModuleContentWidth, 0);
    list->setEnableVerticalScroll(true);
    connect(list, &SearchList::visibleCountChanged, [list]{
        list->setFixedHeight(qMin(LIST_MAX_HEIGHT, list->visibleCount() * RADIO_ITEM_HEIGHT));
    });

    foreach (const ShortcutInfo &info, tmplist) {
        ///非搜索状态下分类显示的快捷键对象
        ShortcutWidget *tmpw = new ShortcutWidget(m_dbus, info.id, info.title, info.shortcut);
        ///搜索状态下显示的快捷键对象
        ShortcutWidget *shortw = new ShortcutWidget(m_dbus, info.id, info.title, info.shortcut);
        m_searchList->addItem(shortw);
        list->addItem(tmpw);

        connect(tmpw, &ShortcutWidget::shortcutChanged, [=](const QString &flag, const QString &shortcut){
            editShortcut(tmpw, list, flag, shortcut);
        });
        connect(shortw, &ShortcutWidget::shortcutChanged, [=](const QString &flag, const QString &shortcut){
            editShortcut(shortw, m_searchList, flag, shortcut);
        });
        connect(shortw, &ShortcutWidget::keyWordChanged, m_searchList, &SearchList::updateItemKeyWord);
        connect(tmpw, &ShortcutWidget::removeShortcut, this, &MainWidget::removeShortcut);
        connect(this, &MainWidget::setEnableEditShortcut, tmpw, &ShortcutWidget::setEnableEdit);
        connect(this, &MainWidget::setEnableEditShortcut, shortw, &ShortcutWidget::setEnableEdit);
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
    DTextButton *button_cancel = new DTextButton(QObject::tr("Cancel"));
    DTextButton *button_add = new DTextButton(QObject::tr("Add"));
    button_cancel->setFixedSize(TEXT_BUTTON_MIN_WIDTH, BUTTON_HEIGHT);
    button_add->setFixedSize(TEXT_BUTTON_MIN_WIDTH, BUTTON_HEIGHT);

    connect(button_cancel, &DTextButton::clicked, [=](){
        edit_name->clear();
        edit_command->clear();
        w->hide();
        emit addCustomShortcutFinished();
    });

    connect(edit_name, &DLineEdit::textChanged, this, [edit_name]{
        edit_name->setAlert(false);
    });

    connect(edit_command, &DLineEdit::textChanged, this, [edit_command]{
        edit_command->setAlert(false);
    });

    connect(button_add, &DTextButton::clicked, [=](){
        bool canAdd = true;

        if(edit_name->text().isEmpty()) {
            edit_name->setAlert(true);
            canAdd = false;
        }
        if(edit_command->text().isEmpty()) {
            edit_command->setAlert(true);
            canAdd = false;
        }
        if(canAdd) {
            m_dbus->AddCustomShortcut(edit_name->text(), edit_command->text());
            w->hide();
            edit_name->clear();
            edit_command->clear();
            emit addCustomShortcutFinished();
        }
    });

    connect(this, SIGNAL(addCustomShortcut()), w, SLOT(show()));

    edit_name->setFixedWidth(200);
    edit_command->setFixedWidth(200);

    hlayout1->addSpacing(HEADER_LEFT_MARGIN);
    hlayout1->addWidget(new QLabel(tr("Name")), 0, Qt::AlignRight);
    hlayout1->addWidget(edit_name);
    hlayout1->addSpacing(HEADER_RIGHT_MARGIN);
    hlayout2->addSpacing(HEADER_RIGHT_MARGIN);
    hlayout2->addWidget(new QLabel(tr("Command")), 0, Qt::AlignRight);
    hlayout2->addWidget(edit_command);
    hlayout2->addSpacing(HEADER_RIGHT_MARGIN);
    hlayout3->addStretch(1);
    hlayout3->addWidget(button_cancel);
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

    connect(this, &MainWidget::addCustomShortcutFinished, [=]{
        w->setAddHidden(false);
        w->setRemoveHidden(m_customList->count()<1);
    });

    return w;
}

void MainWidget::init()
{
    m_layout = mainLayout();

    m_header->setFixedWidth(DCC::ModuleContentWidth);
    connect(m_header, &ModuleHeader::resetButtonClicked, m_dbus, &ShortcutDbus::Reset);

    m_searchList->hide();
    m_searchList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_searchList->setItemSize(DCC::ModuleContentWidth, 0);

    m_systemList = addSearchList(ShortcutInfoList());
    m_windowList = addSearchList(ShortcutInfoList());
    m_workspaceList = addSearchList(ShortcutInfoList());
    m_customList = addSearchList(ShortcutInfoList());

    m_systemList->setObjectName(tr("System"));
    m_windowList->setObjectName(tr("Window"));
    m_workspaceList->setObjectName(tr("Workspace"));
    m_customList->setObjectName(tr("Custom"));

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

    m_customLine = getCustomLstHeadBar();
    m_customLine->setRemoveHidden(m_customList->count()<1);

    DSeparatorHorizontal *customListSeparator = new DSeparatorHorizontal;
    customListSeparator->setHidden(m_customList->count()<1);
    connect(m_customList, &SearchList::countChanged, [=]{
        customListSeparator->setHidden(m_customList->count()<1);
        if(m_customLine->doneButton()->isHidden())
            m_customLine->setRemoveHidden(m_customList->count()<1);
    });

    DSearchEdit *edit  = new DSearchEdit;
    edit->setPlaceHolder(tr("Search"));

    edit->setFixedWidth(280);
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
            customListSeparator->setHidden(m_customList->count()<1);
        }
    });

    m_addShortcutDialog = getAddShortcutWidget();

    headerLayout()->addWidget(m_header);
    headerLayout()->addWidget(new DSeparatorHorizontal);
    headerLayout()->addSpacing(5);
    headerLayout()->addWidget(edit, 0, Qt::AlignHCenter);
    headerLayout()->addSpacing(5);
    headerLayout()->addWidget(new DSeparatorHorizontal);
    m_layout->addWidget(m_searchList, 10);
    m_childLayout->setMargin(0);
    m_childLayout->addWidget(addExpand(tr("System"), m_systemList));
    m_childLayout->addWidget(addExpand(tr("Window"), m_windowList));
    m_childLayout->addWidget(addExpand(tr("Workspace"), m_workspaceList));
    m_childLayout->addWidget(m_customLine);
    m_childLayout->addWidget(new DSeparatorHorizontal);
    m_childLayout->addWidget(m_customList);
    m_childLayout->addWidget(customListSeparator);
    m_childLayout->addSpacing(5);
    m_childLayout->addWidget(m_addShortcutDialog);
    m_layout->addLayout(m_childLayout);
    m_layout->addStretch(1);
}

void MainWidget::shortcutListChanged(SearchList *listw, const ShortcutInfoList &list, int offseIndex)
{
    int min = qMin(listw->count(), list.count());

    for(int i=0;i<min;++i){
        const ShortcutInfo &info = list[i];
        listw->setItemData(i,
                           QVariantList()<<info.id<<info.title<<info.shortcut);
        m_searchList->setItemData(offseIndex+i,
                                  QVariantList()<<info.id<<info.title<<info.shortcut);
    }

    if(listw->count()<list.count()){
        for(int i=min;i<list.count();++i){
            const ShortcutInfo &info = list[i];
            ///非搜索状态下分类显示的快捷键对象
            ShortcutWidget *tmpw = new ShortcutWidget(m_dbus, info.id, info.title, info.shortcut);
            ///搜索状态下显示的快捷键对象
            ShortcutWidget *shortw = new ShortcutWidget(m_dbus, info.id, info.title, info.shortcut);
            m_searchList->insertItem(offseIndex+i, shortw);
            listw->addItem(tmpw);
            connect(tmpw, &ShortcutWidget::shortcutChanged, [=](const QString& flag, const QString &shortcut){
                editShortcut(tmpw, listw, flag, shortcut);
            });
            connect(shortw, &ShortcutWidget::shortcutChanged, [=](const QString& flag, const QString &shortcut){
                editShortcut(shortw, m_searchList, flag, shortcut);
            });
            connect(shortw, &ShortcutWidget::keyWordChanged, m_searchList, &SearchList::updateItemKeyWord);
            connect(tmpw, &ShortcutWidget::removeShortcut, this, &MainWidget::removeShortcut);
            connect(this, &MainWidget::setEnableEditShortcut, tmpw, &ShortcutWidget::setEnableEdit);
            connect(this, &MainWidget::setEnableEditShortcut, shortw, &ShortcutWidget::setEnableEdit);
        }
    }else{
        for(int i = listw->count() - 1; i >= list.count(); --i){
            listw->removeItem(i);
            m_searchList->removeItem(offseIndex+i);
        }
    }

    update();
}

void MainWidget::editShortcut(ShortcutWidget *w, SearchList *listw, const QString &flag, QString shortcut)
{
    if(!w||!listw)
        return;

    if(flag == "Valid"){
        m_dbus->ModifyShortcut(w->id(), shortcut);
        return;
    }

    int index = listw->indexOf(w);

    if(index<0||index>listw->count()-1)
        return;

    if(flag == "Invalid"){
        ToolTip *label = new ToolTip;
        label->setText(tr("Shortcut \"%1\" is invalid, please retype new shortcut.")
                       .arg(shortcut));
        listw->insertItem(index+1, label);
        label->setTimeout(2000);
        label->expansion();
        connect(label, &ToolTip::contracted, [=]{
            listw->removeItem(listw->indexOf(label));
        });
    }else if(flag == "Conflict"){
        emit setEnableEditShortcut(false);

        QList<ShortcutWidget*> tmp_list;

        QString tmp_shortcut = shortcut;
        QString tmp_text = tr("The shortcut you set ");

        QList<SearchList*> tmp_searchlist;
        tmp_searchlist<<m_systemList<<m_windowList<<m_workspaceList<<m_customList;

        foreach (SearchList* list, tmp_searchlist) {
            for (int i=0; i < list->count(); ++i) {
                ShortcutWidget *tmp_w = qobject_cast<ShortcutWidget*>(list->getItem(i)->widget());
                if(tmp_w && tmp_w->shortcut().toLower() == tmp_shortcut.toLower()){
                    tmp_list << tmp_w;
                    tmp_text += tr("conflicts with the one used for \"%2\" in the \"%1\" category.")
                            .arg(list->objectName()).arg(tmp_w->title());
                }
            }
        }

        ///下面的代码是为了得出tmp_text在message box中的高度
        tmp_text.append(tr("Do you want to replace it?"));
        QLabel tmp_label(tmp_text);
        QFont tmp_font;
        tmp_font.setPixelSize(12);
        tmp_label.setFont(tmp_font);
        tmp_label.setWordWrap(true);
        tmp_label.setFixedWidth(DCC::ModuleContentWidth - 70);
        ///70 是 warning icon的宽
        int dialog_height = qMax(tmp_label.sizeHint().height(), 40) + 60;
        ///40是warning icon的高
        ///60是对话框中button和间隔的高

        if(!m_conflictDialog) {
            m_conflictDialog = new SelectDialog;

            m_conflictDialog->setFixedHeight(dialog_height);

            listw->insertItem(index+1, m_conflictDialog);
            listw->setFixedHeight(qMin(LIST_MAX_HEIGHT,
                                       listw->count() * RADIO_ITEM_HEIGHT + dialog_height - RADIO_ITEM_HEIGHT));

            connect(m_conflictDialog, &SelectDialog::replace, [=]{
                m_conflictDialog->setFixedHeight(dialog_height);
                listw->removeItem(listw->indexOf(m_conflictDialog));
                m_conflictDialog->deleteLater();
                m_customLine->setDisabled(false);
                m_addShortcutDialog->setDisabled(false);

                foreach (ShortcutWidget* tmp_w, tmp_list) {
                    m_dbus->ModifyShortcut(tmp_w->id(), "");
                }
                m_dbus->ModifyShortcut(w->id(), shortcut);
                emit setEnableEditShortcut(true);
            });
            connect(m_conflictDialog, &SelectDialog::cancel, [=]{
                m_conflictDialog->contraction();
            });
            connect(m_conflictDialog, &SelectDialog::contracted, [=]{
                m_conflictDialog->setFixedHeight(dialog_height);
                listw->removeItem(listw->indexOf(m_conflictDialog));
                m_conflictDialog->deleteLater();
                m_customLine->setDisabled(false);
                m_addShortcutDialog->setDisabled(false);

                emit setEnableEditShortcut(true);
            });
            connect(m_header, &ModuleHeader::resetButtonClicked,
                    m_conflictDialog.data(), &SelectDialog::contracted);

            m_customLine->setDisabled(true);
            m_addShortcutDialog->setDisabled(true);

            if(m_customLine->doneButton()->isVisible()) {
                m_customLine->doneButton()->clicked();
            }
        }

        m_conflictDialog->setText(tmp_text);

        int dialog_y = listw->getWidget(index)->geometry().bottom();
        int listw_content_y = listw->verticalScrollBar()->value();

        if(dialog_y > listw->height() + listw_content_y - dialog_height / 3) {
            m_conflictDialog->setMinimumHeight(0);
            QMetaObject::invokeMethod(listw->verticalScrollBar(), "setValue",
                                      Qt::QueuedConnection,
                                      Q_ARG(int, listw_content_y + dialog_height));
        } else {
            m_conflictDialog->expansion();
        }
    }
}

void MainWidget::removeShortcut()
{
    ShortcutWidget *w = qobject_cast<ShortcutWidget*>(sender());
    if(w){
        m_dbus->DeleteCustomShortcut(w->id());
    }
}
