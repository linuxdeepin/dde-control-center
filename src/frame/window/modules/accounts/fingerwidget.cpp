/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
 *
 * Maintainer: liuhong <liuhong_cm@deepin.com>
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

#include "fingerwidget.h"
#include "widgets/titlelabel.h"

#include <DCommandLinkButton>
#include <DFontSizeManager>

#include <QHBoxLayout>
#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE
using namespace dcc::accounts;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::accounts;

FingerWidget::FingerWidget(User *user, QWidget *parent)
    : QWidget(parent)
    , m_curUser(user)
    , m_listGrp(new SettingsGroup)
    , m_addfingeGrp(new SettingsGroup)
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    AccounntFingeItem *addfingeItem = new AccounntFingeItem;
    DCommandLinkButton *addBtn = new DCommandLinkButton(tr("Add fingerprint"));
    DCommandLinkButton *clearBtn = new DCommandLinkButton(tr("Delete fingerprint"));
    TitleLabel *fingetitleLabel = new TitleLabel(tr("Fingerprint Password"));

    m_listGrp->setSpacing(2);
    m_addfingeGrp->setSpacing(2);
    addfingeItem->setTitle("");
    addfingeItem->appendItem(addBtn);
    m_addfingeGrp->appendItem(addfingeItem);

    QHBoxLayout *headLayout = new QHBoxLayout;
    headLayout->setContentsMargins(0, 0, 0, 0);
    headLayout->setSpacing(0);
    headLayout->setMargin(0);
    headLayout->addWidget(fingetitleLabel, 0, Qt::AlignLeft);
    headLayout->addWidget(clearBtn, 0, Qt::AlignRight);

    QVBoxLayout *mainContentLayout = new QVBoxLayout;
    mainContentLayout->setContentsMargins(0, 0, 0, 0);
    mainContentLayout->setSpacing(2);
    mainContentLayout->setMargin(0);
    mainContentLayout->addLayout(headLayout);
    mainContentLayout->addWidget(m_listGrp);
    mainContentLayout->addWidget(m_addfingeGrp);
    setLayout(mainContentLayout);

    //设置字体大小
    DFontSizeManager::instance()->bind(fingetitleLabel, DFontSizeManager::T5);
    DFontSizeManager::instance()->bind(addBtn, DFontSizeManager::T7);
    DFontSizeManager::instance()->bind(clearBtn, DFontSizeManager::T7);

    connect(addBtn, &DCommandLinkButton::clicked, this, [ = ] {
        Q_EMIT requestAddThumbs(m_curUser->name(), m_notUseThumb);
    });
    connect(clearBtn, &DCommandLinkButton::clicked, this, [ = ] {
        Q_EMIT requestCleanThumbs(m_curUser);
        addBtn->setVisible(true);
    });
}

FingerWidget::~FingerWidget()
{

}

void FingerWidget::setFingerModel(FingerModel *model)
{
    m_model = model;
    connect(model, &FingerModel::thumbsListChanged, this, &FingerWidget::onThumbsListChanged);
    onThumbsListChanged(model->thumbsList());
}

void FingerWidget::onThumbsListChanged(const QList<dcc::accounts::FingerModel::UserThumbs> &thumbs)
{
    QStringList thumb = thumbsLists;

    m_listGrp->clear();

    for (int n = 0; n < 10 && n < thumbs.size(); ++n) {
        auto u = thumbs.at(n);
        if (u.username != m_curUser->name()) {
            continue;
        }

        int i = 1;//记录指纹列表项编号
        Q_FOREACH (const QString &title, u.userThumbs) {
            AccounntFingeItem *item = new AccounntFingeItem(this);
            item->setTitle(tr("Fingerprint") + QString::number(i++));
            DFontSizeManager::instance()->bind(item, DFontSizeManager::T6);
            m_listGrp->appendItem(item);
            thumb.removeOne(title);
        }

        if (!thumb.isEmpty()) {
            m_notUseThumb = thumb.first();
        }

        if (i == 11) {
            m_addfingeGrp->setVisible(false);
        }
        return;
    }

    m_notUseThumb = thumb.first();
}
