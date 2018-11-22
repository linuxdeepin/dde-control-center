/*
 * Copyright (C) 2015 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             Hualet <mr.asianwang@gmail.com>
 *             kirigaya <kirigaya@mkacg.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             Hualet <mr.asianwang@gmail.com>
 *             kirigaya <kirigaya@mkacg.com>
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

#include "fingerpage.h"
#include "widgets/settingsgroup.h"
#include "widgets/labels/tipslabel.h"
#include "widgets/translucentframe.h"
#include "widgets/fingeritem.h"
#include "widgets/settingsheaderitem.h"
#include "fingermodel.h"

#include <QVBoxLayout>
#include <QPushButton>

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::accounts;

FingerPage::FingerPage(User *user, QWidget *parent)
    : ContentWidget(parent)
    , m_listGrp(new SettingsGroup(tr("Fingerprint List")))
    , m_user(user)
{
    setTitle(tr("Fingerprint Password"));

    m_addBtn = new QPushButton;
    m_addBtn->setText(tr("Add Fingerprint"));

    m_cleanBtn = new QPushButton;
    m_cleanBtn->setText(tr("Clear Fingerprint"));
    m_cleanBtn->setObjectName("FingerClean");

    TipsLabel *tip = new TipsLabel;
    tip->setText(tr("Fingerprint password is used in system login, application authorization and other scenes. Scan the fingerprint when password needed."));
    tip->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(10);

    layout->addWidget(m_listGrp);
    layout->addWidget(m_cleanBtn);
    layout->addWidget(m_addBtn);
    layout->addWidget(tip);
    layout->addStretch();

    TranslucentFrame *w = new TranslucentFrame;
    w->setLayout(layout);

    setContent(w);

    connect(m_addBtn, &QPushButton::clicked, this, &FingerPage::addThumb);
    connect(m_cleanBtn, &QPushButton::clicked, this, &FingerPage::cleanThumbs);
}

void FingerPage::setFingerModel(FingerModel *model)
{
    m_model = model;

    connect(model, &FingerModel::thumbsListChanged, this, &FingerPage::onThumbsListChanged);

    onThumbsListChanged(model->thumbsList());
}

void FingerPage::onThumbsListChanged(const QList<FingerModel::UserThumbs> &thumbs)
{
    QStringList thumb = thumbsLists;

    m_listGrp->clear();

    for (const FingerModel::UserThumbs &u : thumbs) {
        if (u.username == m_user->name()) {
            int i = 1;
            Q_FOREACH (const QString &title, u.userThumbs) {
                FingerItem *item = new FingerItem;
                item->setTitle(tr("Fingerprint") + QString::number(i++));

                m_listGrp->appendItem(item);
                thumb.removeOne(title);
            }

            const int size = thumb.size();
            m_addBtn->setVisible(size);
            m_listGrp->headerItem()->setVisible(size);
            m_cleanBtn->setVisible(true);

            if (!thumb.isEmpty())
                m_notUseThumb = thumb.first();
            return;
        }
    }

    m_addBtn->setVisible(true);
    m_notUseThumb = thumb.first();
    m_listGrp->headerItem()->setVisible(false);
    m_cleanBtn->setVisible(false);
}

void FingerPage::addThumb()
{
    Q_EMIT requestAddThumbs(m_user->name(), m_notUseThumb);
}

void FingerPage::cleanThumbs()
{
    Q_EMIT requestCleanThumbs(m_user);
}
