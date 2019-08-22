/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LiLinling <lilinling_cm@deepin.com>
 *
 * Maintainer: LiLinling <lilinling_cm@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "defappwidget.h"

#include <DStyleOption>

#include <QListView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QVBoxLayout>
#include <QTimer>
#include <QDebug>

using namespace DCC_NAMESPACE::defapp;

DWIDGET_USE_NAMESPACE

Q_DECLARE_METATYPE(QMargins)

DefaultAppsWidget::DefaultAppsWidget(QWidget *parent)
    : QWidget(parent)
    , m_defAppCatView(new QListView(this))
    , m_centralLayout(new QVBoxLayout(this))
{
    setObjectName("Defapp");
    m_defAppCatView->setIconSize(QSize(24, 24));
    m_defAppCatView->setResizeMode(QListView::Adjust);
    m_defAppCatView->setMovement(QListView::Static);
    m_defAppCatView->setFrameShape(QFrame::NoFrame);
    m_defAppCatView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QStringList titles;
    titles << tr("Webpage")
           << tr("Mail")
           << tr("Text")
           << tr("Music")
           << tr("Video")
           << tr("Picture")
           << tr("Terminal");

    QStringList icons;
    icons << "dcc_browser"
          << "dcc_mail"
          << "dcc_text"
          << "dcc_music"
          << "dcc_video"
          << "dcc_photo"
          << "dcc_terminal";

    //Initialize second page view and model
    QStandardItemModel *model = new QStandardItemModel(this);

    for (int i = 0; i < icons.size(); i++) {
        QStandardItem* item = new QStandardItem(QIcon::fromTheme(icons.at(i)), titles.at(i));

        item->setData(Dtk::RoundedBackground, Dtk::BackgroundTypeRole);
        item->setSizeHint(QSize(230, 48));
        item->setData(QVariant::fromValue(QMargins(10, 10, 10, 0)), Dtk::MarginsRole);
        model->appendRow(item);
    }

    m_defAppCatView->setModel(model);
    //show default browser app
    m_defAppCatView->setCurrentIndex(model->indexFromItem(model->item(0)));

    m_centralLayout->addWidget(m_defAppCatView);

    connect(m_defAppCatView, &QListView::clicked, this, &DefaultAppsWidget::onCategoryClicked);

    setAccessibleName(tr("Default Applications"));
    setMinimumSize(262, 564);
}

void DefaultAppsWidget::onCategoryClicked(const QModelIndex &index) {
    qDebug() << "DefaultAppsWidget clicked row " << index.row();
    Q_EMIT requestCategoryClicked(dcc::defapp::DefAppWorker::DefaultAppsCategory(index.row()));
}
