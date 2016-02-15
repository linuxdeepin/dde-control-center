/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QVariant>
#include <QMouseEvent>

#include <libdui_global.h>
#include <dthememanager.h>
#include <dconstants.h>
#include <dloadingindicator.h>

#include "genericlistitem.h"
#include "imagenamebutton.h"

DWIDGET_USE_NAMESPACE

GenericListItem::GenericListItem(bool showRmButton, QWidget *parent) :
    QFrame(parent),
    m_widget(this),
    m_label(new QLabel),
    m_checked(false),
    m_layout(new QHBoxLayout),
    m_showBgColor(false),
    m_loading(false),
    m_showClearButton(false),
    m_showCheckedIcon(true)
{
    D_THEME_INIT_WIDGET(GenericListItem, checked, showBgColor, showCheckedIcon, loading);

    m_deleteButton = new ImageNameButton("list_remove", this);
    m_deleteButton->setFixedSize(15, 15);
    m_deleteButton->setHidden(!showRmButton);

    connect(m_deleteButton, &ImageNameButton::clicked, this, &GenericListItem::removeButtonClicked);
    connect(this, &GenericListItem::showRemoveButton, [ = ] {
        if (!m_checked)
        {
            m_deleteButton->show();
        }
    });
    connect(this, &GenericListItem::hideRemoveButton, m_deleteButton, &ImageNameButton::hide);

    DLoadingIndicator *indicator = new DLoadingIndicator(this);
    indicator->setFixedSize(20, 20);
    indicator->setImageSource(QPixmap(checkedIcon()));
    indicator->lower();

    ImageNameButton *clear_button = new ImageNameButton("clear", this);
    clear_button->setHoverPic(clear_button->getNormalPic());
    clear_button->setPressPic(clear_button->getNormalPic());
    clear_button->setFixedSize(15, 15);
    clear_button->hide();

    auto updateShowCheckedIcon = [this, indicator](bool checked) {
        if (checked && showCheckedIcon()) {
            indicator->setLoading(false);
            setShowClearButton(false);
            indicator->setImageSource(QPixmap(checkedIcon()));
            indicator->move(10, height() / 2 - indicator->height() / 2);
        } else {
            indicator->setImageSource(QPixmap());
        }
    };

    connect(this, &GenericListItem::checkedChanged, this, updateShowCheckedIcon);

    connect(this, &GenericListItem::checkedIconChanged, this, [this, indicator](const QString & str) {
        if (checked() && showCheckedIcon()) {
            indicator->setImageSource(QPixmap(str));
            indicator->move(10, height() / 2 - indicator->height() / 2);
        }
    });

    connect(this, &GenericListItem::loadingChanged, this, [this, indicator](bool loading) {
        if (loading) {
            setShowClearButton(false);
            indicator->setImageSource(QPixmap(loadingIcon()));
            indicator->move(10, height() / 2 - indicator->height() / 2);
        } else {
            indicator->setImageSource(QPixmap());
        }
        indicator->setLoading(loading);
    });
    connect(this, &GenericListItem::showClearButtonChanged, this, [this, indicator, clear_button](bool showClear) {
        clear_button->move(10, height() / 2 - clear_button->height() / 2);
        indicator->setVisible(!showClear);
        clear_button->setVisible(showClear);
    });
    connect(this, &GenericListItem::showCheckedIconChanged, this, updateShowCheckedIcon);
    connect(this, &GenericListItem::checkedIconChanged, this, [this, indicator] {
        if (checked() && showCheckedIcon())
        {
            indicator->setImageSource(QPixmap(checkedIcon()));
            indicator->move(10, height() / 2 - indicator->height() / 2);
        }
    });
    connect(this, &GenericListItem::loadingIconChanged, this, [this, indicator] {
        if (loading())
        {
            indicator->setImageSource(QPixmap(loadingIcon()));
            indicator->move(10, height() / 2 - indicator->height() / 2);
        }
    });
    connect(clear_button, &ImageNameButton::clicked, this, &GenericListItem::clearButtonClicked);

    m_layout->setMargin(0);
    m_layout->setContentsMargins(22, 0, 0, 0);
    m_layout->addWidget(m_label);
    setLayout(m_layout);
}

QStringList GenericListItem::keyWords() const
{
    return m_keyWords;
}

void GenericListItem::setData(const QVariant &datas)
{
    if (datas.type() == QVariant::String) {
        m_label->setText(datas.toString());
    }
}

QVariant GenericListItem::getData()
{
    return m_label->text();
}

QWidget *GenericListItem::widget() const
{
    return m_widget;
}

void GenericListItem::setListWidget(SearchList *list)
{
    SearchItem::setListWidget(list);

    connect(list, &SearchList::countChanged, this, [&] {
        if (m_list)
            setShowBgColor(m_list->count() > 1);
    }, Qt::QueuedConnection);

    if (m_list) {
        setShowBgColor(m_list->count() > 1);
    }
}

void GenericListItem::addWidgetToRight(QWidget *widget)
{
    m_layout->addWidget(widget, 0, Qt::AlignRight);
}

QBoxLayout *GenericListItem::rightLayout() const
{
    return m_layout;
}

bool GenericListItem::checked() const
{
    return m_checked;
}

QString GenericListItem::title() const
{
    return m_label->text();
}

QString GenericListItem::imagePress() const
{
    return m_imagePress;
}

QString GenericListItem::imageHover() const
{
    return m_imageHover;
}

QString GenericListItem::imageNormal() const
{
    return m_imageNormal;
}

QString GenericListItem::imageChecked() const
{
    return m_imageChecked;
}

QString GenericListItem::checkedIcon() const
{
    return m_leftIconSource;
}

QString GenericListItem::loadingIcon() const
{
    return m_loadingIcon;
}

bool GenericListItem::showBgColor() const
{
    return m_showBgColor;
}

bool GenericListItem::loading() const
{
    return m_loading;
}

bool GenericListItem::showClearButton() const
{
    return m_showClearButton;
}

bool GenericListItem::showCheckedIcon() const
{
    return m_showCheckedIcon;
}

void GenericListItem::setChecked(bool checked)
{
    if (m_checked == checked) {
        return;
    }

    m_checked = checked;

    if (!m_imageChecked.isEmpty()/* && !m_imageChecked.isEmpty()*/) {
        if (checked) {
            m_label->removeEventFilter(this);
            m_label->setPixmap(QPixmap(m_imageChecked));
        } else if (!m_imageNormal.isEmpty()) {
            m_label->installEventFilter(this);
            m_label->setPixmap(QPixmap(m_imageNormal));
        }
    }

    emit checkedChanged(checked);
}

void GenericListItem::setTitle(QString title)
{
    m_label->setText(title);
}

void GenericListItem::setImagePress(QString imagePress)
{
    m_imagePress = imagePress;
}

void GenericListItem::setImageHover(QString imageHover)
{
    m_imageHover = imageHover;
}

void GenericListItem::setImageNormal(QString imageNormal)
{
    m_imageNormal = imageNormal;
    if (imageNormal.isEmpty()) {
        m_label->removeEventFilter(this);
    } else {
        if (!m_checked) {
            m_label->setPixmap(QPixmap(imageNormal));
        }
        m_label->installEventFilter(this);
    }
}

void GenericListItem::setImageChecked(QString imageChecked)
{
    m_imageChecked = imageChecked;
    if (!imageChecked.isEmpty() && m_checked) {
        m_label->setPixmap(QPixmap(imageChecked));
    }
}

void GenericListItem::setKeyWords(QStringList keyWords)
{
    m_keyWords = keyWords;
}

void GenericListItem::setCheckedIcon(QString leftIconSource)
{
    if (m_leftIconSource == leftIconSource) {
        return;
    }

    m_leftIconSource = leftIconSource;
    emit checkedIconChanged(leftIconSource);
}

void GenericListItem::setLoadingIcon(QString loadingIcon)
{
    if (m_loadingIcon == loadingIcon) {
        return;
    }

    m_loadingIcon = loadingIcon;
    emit loadingIconChanged(loadingIcon);
}

void GenericListItem::setShowBgColor(bool showBgColor)
{
    if (m_showBgColor == showBgColor) {
        return;
    }

    m_showBgColor = showBgColor;

    emit showBgColorChanged(showBgColor);
}

void GenericListItem::setLoading(bool loading)
{
    if (m_loading == loading) {
        return;
    }

    m_loading = loading;
    emit loadingChanged(loading);
}

void GenericListItem::setShowClearButton(bool showClearButton)
{
    if (m_showClearButton == showClearButton) {
        return;
    }

    m_showClearButton = showClearButton;
    emit showClearButtonChanged(showClearButton);
}

void GenericListItem::setShowCheckedIcon(bool showCheckedIcon)
{
    if (m_showCheckedIcon == showCheckedIcon) {
        return;
    }

    m_showCheckedIcon = showCheckedIcon;
    emit showCheckedIconChanged(showCheckedIcon);
}

bool GenericListItem::eventFilter(QObject *obj, QEvent *e)
{
    if (obj == m_label) {
        switch (e->type()) {
        case QEvent::Enter: {
            QPixmap *pixmap = const_cast<QPixmap *>(m_label->pixmap());
            pixmap->load(m_imageHover);
            break;
        }
        case QEvent::Leave: {
            QPixmap *pixmap = const_cast<QPixmap *>(m_label->pixmap());
            pixmap->load(m_imageNormal);
            break;
        }
        case QEvent::MouseButtonPress: {
            QPixmap *pixmap = const_cast<QPixmap *>(m_label->pixmap());
            pixmap->load(m_imagePress);
            break;
        }
        case QEvent::MouseButtonRelease: {
            QPixmap *pixmap = const_cast<QPixmap *>(m_label->pixmap());
            pixmap->load(m_imageHover);
            break;
        }
        default:
            break;
        }
    }

    return false;
}

void GenericListItem::resizeEvent(QResizeEvent *e)
{
    m_deleteButton->move(8, height() / 2 - m_deleteButton->height() / 2);

    QFrame::resizeEvent(e);
}

void GenericListItem::mouseReleaseEvent(QMouseEvent *e)
{
    QFrame::mouseReleaseEvent(e);

    emit clicked();
}

void GenericListItem::enterEvent(QEvent *e)
{
    QFrame::enterEvent(e);

    emit mouseEnter();
}

void GenericListItem::leaveEvent(QEvent *e)
{
    QFrame::leaveEvent(e);

    emit mouseLeave();
}
