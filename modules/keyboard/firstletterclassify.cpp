/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QDBusInterface>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QDebug>

#include <dconstants.h>
#include <dwidget_global.h>
#include <dthememanager.h>

#include "imagenamebutton.h"
#include "multiaddcheckbutton.h"

#include "firstletterclassify.h"

DWIDGET_USE_NAMESPACE

KeyboardLayoutDelegate::KeyboardLayoutDelegate(const QString &title, QWidget *parent):
    QFrame(parent),
    m_layout(new QHBoxLayout),
    m_label(new QLabel),
    m_checkButton(new MultiAddCheckButton)
{
    D_THEME_INIT_WIDGET(KeyboardLayoutDelegate);

    setTitle(title);

    m_checkButton->setCheckable(true);
    m_checkButton->setChecked(false);
    m_checked = false;

    m_layout->setMargin(0);
    m_layout->addSpacing(HEADER_LEFT_MARGIN);
    m_layout->addWidget(m_label);
    m_layout->addWidget(m_checkButton, 0, Qt::AlignVCenter|Qt::AlignRight);
    m_layout->addSpacing(HEADER_RIGHT_MARGIN);

    setLayout(m_layout);

    connect(m_checkButton, &MultiAddCheckButton::stateChanged, [&]{
        if(m_checked != m_checkButton->isChecked()){
            m_checked = m_checkButton->isChecked();
            emit checkedChanged(m_checked);
        }
    });
}

QStringList KeyboardLayoutDelegate::keyWords()
{
    return m_pinyinFirstLetterList;
}

void KeyboardLayoutDelegate::setKeyWords(const QStringList &keywords)
{
    m_pinyinFirstLetterList = keywords;
}

QString KeyboardLayoutDelegate::title() const
{
    return m_label->text();
}

bool KeyboardLayoutDelegate::checked() const
{
    return m_checkButton->isChecked();
}

void KeyboardLayoutDelegate::setChecked(bool checked)
{
    m_checkButton->setChecked(checked);
}

void KeyboardLayoutDelegate::mouseReleaseEvent(QMouseEvent *e)
{
    QFrame::mouseReleaseEvent(e);

    m_checkButton->setChecked(!m_checkButton->isChecked());
}

void KeyboardLayoutDelegate::setTitle(const QString &title)
{
    m_label->setText(title);
}

FirstLetterClassify::FirstLetterClassify(QWidget *parent) :
    QFrame(parent),
    m_layout(new QVBoxLayout),
    m_letterList(new DSegmentedControl)
{
    D_THEME_INIT_WIDGET(FirstLetterClassify);

    m_letterList->setObjectName("LetterList");

    for (int i = 'A'; i <= 'Z'; ++i) {
        ListWidget *w = new ListWidget;
        w->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        w->setItemSize(310, EXPAND_HEADER_HEIGHT);
        w->setEnableVerticalScroll(true);
        w->setMaximumHeight(200);

        m_listWidgetList << w;

        m_letterList->addSegmented(QString(i));
        m_letterList->at(i - 65)->hide();
    }
    m_currentList = m_listWidgetList.first();

    m_layout->setMargin(0);
    m_layout->addWidget(m_letterList);
    m_layout->addStretch(1);
    setLayout(m_layout);

    connect(m_letterList, &DSegmentedControl::currentTitleChanged, [&](const QString& key){
        int index = key[0].toUpper().toLatin1() - 65;
        m_currentList->setParent(0);
        /// 1 is m_currentList index for m_layout
        delete m_layout->takeAt(1);
        m_currentList = m_listWidgetList[index];
        m_layout->addWidget(m_currentList);

        emit currentLetterChanged(key);
    });

    foreach (QObject *obj, m_letterList->children()) {
        QWidget *w = qobject_cast<QWidget*>(obj);
        if(w){
            w->setObjectName("");
        }
    }
}

FirstLetterClassify::~FirstLetterClassify()
{
    qDeleteAll(m_listWidgetList.begin(), m_listWidgetList.end());
}

ListWidget *FirstLetterClassify::searchList() const
{
    return m_currentList;
}

DSegmentedControl *FirstLetterClassify::letterList() const
{
    return m_letterList;
}

void FirstLetterClassify::removeItems(QList<KeyboardLayoutDelegate *> datas)
{
    foreach (KeyboardLayoutDelegate* w, datas) {
        foreach (QString pinyin, w->keyWords()) {
            int index = pinyin[0].toUpper().toLatin1() - 65;

            ListWidget *list = m_listWidgetList[index];
            for (int i = 0; i < list->count(); ++i) {
                KeyboardLayoutDelegate* ww = qobject_cast<KeyboardLayoutDelegate*>(list->getWidget(i));
                if(ww->title() == w->title())
                    list->removeWidget(i);
            }
            if(list->count() <= 0){
                m_letterList->at(index)->hide();
                for(int i = 0; i < m_letterList->count(); ++i){
                    if(!m_letterList->at(i)->isHidden()){
                        m_letterList->setCurrentIndex(i);
                        break;
                    }
                }
            }
        }
    }
}

QString FirstLetterClassify::currentLetter() const
{
    return m_letterList->getText(m_letterList->currentIndex());
}

void FirstLetterClassify::addItem(KeyboardLayoutDelegate *item, const QChar letterFirst)
{
    int index = letterFirst.toUpper().toLatin1() - 65;
    if(index >= 0){
        m_listWidgetList[index]->addWidget(item);
        m_letterList->at(index)->show();
    }
}

void FirstLetterClassify::setCurrentLetter(QString currentLetter)
{
    m_letterList->setCurrentIndexByTitle(currentLetter);
}

void FirstLetterClassify::show()
{
    m_letterList->currentTitleChanged(m_letterList->getText(m_listWidgetList.indexOf(m_currentList)));
    QFrame::show();
}

void FirstLetterClassify::setMaximumHeight(const int height)
{
    QWidget::setMaximumHeight(height);

    for (ListWidget *w : m_listWidgetList)
        w->setMaximumHeight(height);
}
