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
    TitleLabel *m_maxFingerTip = new TitleLabel(tr("You can add up to 10 fingerprints"));
    QFont font;
    font.setPointSizeF(10);
    m_maxFingerTip->setFont(font);

    m_listGrp->setSpacing(1);
    m_listGrp->setContentsMargins(0, 0, 0, 0);
    m_listGrp->layout()->setMargin(0);
    m_listGrp->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QHBoxLayout *headLayout = new QHBoxLayout;
    headLayout->setSpacing(0);
    headLayout->setContentsMargins(10, 0, 10, 0);
    headLayout->addWidget(fingetitleLabel, 0, Qt::AlignLeft);

    QHBoxLayout *tipLayout = new QHBoxLayout;
    tipLayout->setSpacing(10);
    tipLayout->setContentsMargins(10, 0, 10, 0);
    tipLayout->addWidget(m_maxFingerTip, 0, Qt::AlignLeft);
    tipLayout->addWidget(m_clearBtn, 0, Qt::AlignRight);
    tipLayout->addSpacing(25);

    QVBoxLayout *mainContentLayout = new QVBoxLayout;
    mainContentLayout->setSpacing(1);
    mainContentLayout->setMargin(0);
    mainContentLayout->addLayout(headLayout);
    mainContentLayout->addSpacing(2);
    mainContentLayout->addLayout(tipLayout);
    mainContentLayout->addSpacing(10);
    mainContentLayout->addWidget(m_listGrp);
    setLayout(mainContentLayout);

    //设置字体大小
    DFontSizeManager::instance()->bind(m_clearBtn, DFontSizeManager::T8);

    connect(m_clearBtn, &DCommandLinkButton::clicked, this, [ = ](bool checked) {
        if (checked) {
            m_clearBtn->setText(tr("Done"));
        } else {
            m_clearBtn->setText(tr("Edit"));
        }
        for (auto &item : m_vecItem) {
            item->setShowIcon(checked);
        }
    });
}

FingerWidget::~FingerWidget()
{

}

void FingerWidget::setFingerModel(FingerModel *model)
{
    m_model = model;
    connect(m_model, &FingerModel::enrollCompleted, this, [this] {
       Q_EMIT noticeEnrollCompleted(m_curUser->name());
    });
    connect(model, &FingerModel::thumbsListChanged, this, &FingerWidget::onThumbsListChanged);
    onThumbsListChanged(model->thumbsList());
}

void FingerWidget::onThumbsListChanged(const QStringList &thumbs)
{
    QStringList thumb = thumbsLists;
    m_vecItem.clear();
    m_listGrp->clear();
    for (int n = 0; n < 10 && n < thumbs.size(); ++n) {
        QString finger = thumbs.at(n);
        auto item = new AccounntFingeItem(this);
        item->setTitle(finger);
        item->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        DFontSizeManager::instance()->bind(item, DFontSizeManager::T6);
        m_listGrp->appendItem(item);
        connect(item, &AccounntFingeItem::removeClicked, this, [this, finger] {
            Q_EMIT requestDeleteFingerItem(m_curUser->name(), finger);
        });
        connect(item, &AccounntFingeItem::editTextFinished, this, [this, finger, item, thumbs](QString newName) {
            for (int n = 0; n < thumbs.size(); ++n) {
                if (newName == thumbs.at(n)) {
                    item->alertTitleRepeat();
                    return;
                }
            }
            item->setTitle(newName);
            Q_EMIT requestRenameFingerItem(m_curUser->name(), finger, newName);
        });

        if(m_clearBtn->isChecked())
            item->setShowIcon(true);

        m_vecItem.append(item);
        thumb.removeOne(finger);
        qDebug() << "onThumbsListChanged: " << finger;
    }

    m_clearBtn->setVisible(m_listGrp->itemCount());
    if (!thumb.isEmpty() && thumbs.size() < 10) {
        m_notUseThumb = thumb.first();
        addFingerButton();
    }
}

void FingerWidget::addFingerButton()
{
    SettingsItem* addfingerItem = new SettingsItem(this);
    DCommandLinkButton *addBtn = new DCommandLinkButton(tr("Add Fingerprint"));
    QHBoxLayout *fingerLayout = new QHBoxLayout(this);
    fingerLayout->addWidget(addBtn);
    addfingerItem->setLayout(fingerLayout);
    m_listGrp->insertItem(m_listGrp->itemCount(), addfingerItem);
    addfingerItem->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    DFontSizeManager::instance()->bind(addBtn, DFontSizeManager::T7);
    connect(addBtn, &DCommandLinkButton::clicked, this, [ = ] {
        qDebug() << "try add finger :" <<  m_curUser->name() << m_notUseThumb;
        Q_EMIT requestAddThumbs(m_curUser->name(), m_notUseThumb);
    });
}
