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

#include <DFontSizeManager>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>

DWIDGET_USE_NAMESPACE
using namespace dcc::accounts;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::accounts;

#define TEST true

FingerWidget::FingerWidget(User *user, QWidget *parent)
    : QWidget(parent)
    , m_curUser(user)
    , m_listGrp(new SettingsGroup(nullptr, SettingsGroup::GroupBackground))
    , m_clearBtn(nullptr)
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    m_clearBtn = new DCommandLinkButton(tr("Edit"));
    m_clearBtn->setCheckable(true);

    TitleLabel *fingetitleLabel = new TitleLabel(tr("Fingerprint Password"));

    m_listGrp->setSpacing(1);
    m_listGrp->setContentsMargins(0, 0, 0, 0);
    m_listGrp->layout()->setMargin(0);
    m_listGrp->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QHBoxLayout *headLayout = new QHBoxLayout;
    headLayout->setSpacing(0);
    headLayout->setContentsMargins(10, 0, 10, 0);
    headLayout->addWidget(fingetitleLabel, 0, Qt::AlignLeft);
    headLayout->addWidget(m_clearBtn, 0, Qt::AlignRight);

    QVBoxLayout *mainContentLayout = new QVBoxLayout;
    mainContentLayout->setSpacing(1);
    mainContentLayout->setMargin(0);
    mainContentLayout->addLayout(headLayout);
    mainContentLayout->addSpacing(10);
    mainContentLayout->addWidget(m_listGrp);
    setLayout(mainContentLayout);

    //设置字体大小
    DFontSizeManager::instance()->bind(m_clearBtn, DFontSizeManager::T8);

    connect(m_clearBtn, &DCommandLinkButton::clicked, this, [ = ](bool checked) {
        if (checked) {
            m_clearBtn->setText(tr("Cancel"));
        } else {
            m_clearBtn->setText(tr("Edit"));
        }
        for (auto &item : m_vecItem) {
            item->setShowIcon(checked);
        }
        Q_EMIT requestCleanThumbs(m_curUser);
    });
//    connect(m_curUser)
}

FingerWidget::~FingerWidget()
{

}

void FingerWidget::setFingerModel(FingerModel *model)
{
    m_model = model;
    m_model->createTestThumbsbList();
    connect(model, &FingerModel::thumbsListChanged, this, &FingerWidget::onThumbsListChanged);
    onThumbsListChanged(model->thumbsList());
}

void FingerWidget::onThumbsListChanged(const QList<dcc::accounts::FingerModel::UserThumbs> &thumbs)
{
    QStringList thumb = thumbsLists;
    bool isAddFingeBtn = true;
    m_listGrp->clear();
    for (int n = 0; n < 10 && n < thumbs.size(); ++n) {
        auto u = thumbs.at(n);
        if (!TEST) {
            if (u.username != m_curUser->name()) {
                continue;
            }
        }

        int i = 1; // 记录指纹列表项编号
        qDebug() << "user thumb count: " << u.userThumbs.size();
        if(!m_vecItem.empty())
            m_vecItem.clear();
        for (const QString &title : u.userThumbs) {
            QString number = QString::number(i++);
            QString finger = tr("Fingerprint") + number;
            auto item = new AccounntFingeItem(this);
            item->setTitle(finger);
            item->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            DFontSizeManager::instance()->bind(item, DFontSizeManager::T6);
            m_listGrp->appendItem(item);
            connect(item, &AccounntFingeItem::removeClicked, this, [this, finger, title] {
                Q_EMIT requestDeleteFingerItem(m_curUser->name(), title);
            });
            connect(item, &AccounntFingeItem::editTextFinished, this, [item](QString finger) {
               item->setTitle(finger);

            });
            if(m_clearBtn->isChecked())
                item->setShowIcon(true);
            m_vecItem.append(item);
            thumb.removeOne(title);
            qDebug() << "onThumbsListChanged: " << finger;
        }

        if (!thumb.isEmpty()) {
            m_notUseThumb = thumb.first();
        }

        if (i == 11) {
            isAddFingeBtn = false;
        }
    }
    if(!TEST)
        m_clearBtn -> setVisible(m_listGrp->itemCount());
    else
        m_clearBtn->setVisible(m_listGrp->itemCount());
    if (!thumb.isEmpty()) {
        m_notUseThumb = thumb.first();
    }
    if (isAddFingeBtn) {
        addFingerButton();
    }
}

void FingerWidget::addFingerButton()
{
    SettingsItem* addfingerItem = new SettingsItem(this);
    DCommandLinkButton *addBtn = new DCommandLinkButton(tr("Add fingerprint"));
    QHBoxLayout *fingerLayout = new QHBoxLayout(this);
    fingerLayout->addWidget(addBtn);
    addfingerItem->setLayout(fingerLayout);
    m_listGrp->insertItem(m_listGrp->itemCount(), addfingerItem);
    addfingerItem->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    DFontSizeManager::instance()->bind(addBtn, DFontSizeManager::T7);
    connect(addBtn, &DCommandLinkButton::clicked, this, [ = ] {
        qDebug() << m_curUser->name() << m_notUseThumb;
        Q_EMIT requestAddThumbs(m_curUser->name(), m_notUseThumb);
    });
}
