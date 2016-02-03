/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <dtextbutton.h>

#include "abstractpanel.h"
#include "moduleheader.h"
#include "constants.h"

#include "errorview.h"


#include <QDebug>

AbstractPanel::AbstractPanel(const QString& title, QWidget *parent)
    : QWidget(parent),
      m_viewLayout(new QVBoxLayout),
      m_view(new QWidget(this)),
      m_headerLine(new ModuleHeader(title, false))
{
    m_viewLayout->setSpacing(0);
    m_viewLayout->setMargin(0);

    setLayout(m_viewLayout);
    addWidget(m_headerLine);
    m_viewLayout->addStretch();

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    setFixedWidth(DCC::ModuleContentWidth);
    setFixedHeight(800);

}

void AbstractPanel::emitChangePanel()
{}

void AbstractPanel::setTitle(const QString &title)
{
    m_headerLine->setTitle(title);
}

AbstractPanel* AbstractPanel::addWidget(QWidget*w)
{
    m_viewLayout->insertWidget(m_viewLayout->count()-1, w);
    return this;
}

AbstractPanel* AbstractPanel::addLayout(QLayout* l, int stretch)
{
    m_viewLayout->addLayout(l, stretch);
    return this;
}

void AbstractPanel::setWidget(QWidget *w)
{
    m_view->hide();
    m_view->deleteLater();
    m_view = w;
    addWidget(w);
}

void AbstractPanel::abort()
{
}

void AbstractPanel::onNoNetwork()
{
    setTitle(tr("Remote Assistance"));
    qDebug ()<< "no network";
    auto view = new ErrorView;
    auto button = new DTextButton(tr("Ok"));
    QObject::connect(button, &DTextButton::clicked, [this] {
        abort();
    });
    view->addButton(button);

    setWidget(view->setText(tr("No network connections are available, please retry after connected")));
}
