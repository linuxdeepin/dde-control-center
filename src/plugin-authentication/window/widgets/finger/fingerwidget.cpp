//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "charamangermodel.h"
#include "fingerwidget.h"
#include "widgets/titlelabel.h"
#include "widgets/settingsgroup.h"

#include <DFontSizeManager>

#include <DCommandLinkButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QEvent>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;

FingerWidget::FingerWidget(QWidget *parent)
    : QWidget(parent)
    , m_listGrp(new SettingsGroup(nullptr, SettingsGroup::GroupBackground))
    , m_clearBtn(nullptr)
{
	//注册所有的事件
    installEventFilter(this);

    m_clearBtn = new DCommandLinkButton(tr("Edit"));
    m_clearBtn->setCheckable(true);

    TitleLabel *fingetitleLabel = new TitleLabel(tr("Fingerprint Password"));
    TitleLabel *maxFingerTip = new TitleLabel(tr("You can add up to 10 fingerprints"));
    QFont font;
    font.setPointSizeF(10);
    maxFingerTip->setFont(font);

    m_listGrp->setSpacing(1);
    m_listGrp->setContentsMargins(10, 0, 10, 0);
    m_listGrp->layout()->setMargin(0);
    m_listGrp->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QHBoxLayout *headLayout = new QHBoxLayout;
    headLayout->setSpacing(0);
    headLayout->setContentsMargins(10, 0, 10, 0);
    headLayout->addWidget(fingetitleLabel, 0, Qt::AlignLeft);

    QHBoxLayout *tipLayout = new QHBoxLayout;
    tipLayout->setSpacing(10);
    tipLayout->setContentsMargins(10, 0, 10, 0);
    tipLayout->addWidget(maxFingerTip, 0, Qt::AlignLeft);
    tipLayout->addWidget(m_clearBtn, 0, Qt::AlignRight);
    tipLayout->addSpacing(5);

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
            //添加一个空白区域
            mainContentLayout->addSpacing(20);
        } else {
            m_clearBtn->setText(tr("Edit"));
            //把之前添加的空白区域移除
            mainContentLayout->removeItem(mainContentLayout->itemAt(mainContentLayout->count() - 1));
        }
        for (auto &item : m_vecItem) {
            item->setShowIcon(checked);
        }
    });
}

FingerWidget::~FingerWidget()
{

}

void FingerWidget::setFingerModel(CharaMangerModel *model)
{
    m_model = model;
    m_currentUserName = model->userName();
    connect(m_model, &CharaMangerModel::enrollCompleted, this, [this] {
        Q_EMIT noticeEnrollCompleted(m_currentUserName);
    });
    connect(model, &CharaMangerModel::thumbsListChanged, this, &FingerWidget::onThumbsListChanged);
    onThumbsListChanged(model->thumbsList());
}

bool FingerWidget::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched);
    //删除指纹提权超时的时候,再次输入密码需要通知界面刷新
    if(event->type() == QEvent::WindowActivate) {
        Q_EMIT noticeEnrollCompleted(m_currentUserName);
    }
    return  true;
}

void FingerWidget::onThumbsListChanged(const QStringList &thumbs)
{
    m_vecItem.clear();
    m_listGrp->clear();

    for (int n = 0; n < 10 && n < thumbs.size(); ++n) {
        QString finger = thumbs.at(n);
        auto item = new AuthenticationInfoItem(this);
        item->setTitle(finger);
        item->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        DFontSizeManager::instance()->bind(item, DFontSizeManager::T6);
        m_listGrp->appendItem(item);
        connect(item, &AuthenticationInfoItem::removeClicked, this, [this, finger] {
            Q_EMIT requestDeleteFingerItem(m_currentUserName, finger);
        });
        connect(item, &AuthenticationInfoItem::editTextFinished, this, [this, finger, item, thumbs, n](QString newName) {
            // 没有改名，直接返回
            if (item->getTitle() == newName) {
                return;
            }
            for (int i = 0; i < thumbs.size(); ++i) {
                if (newName == thumbs.at(i) && i != n) {
                    QString errMsg = tr("The name already exists");
                    item->showAlertMessage(errMsg);
                    return;
                }
            }
            item->setTitle(newName);
            Q_EMIT requestRenameFingerItem(m_currentUserName, finger, newName);
        });

        connect(item, &AuthenticationInfoItem::editClicked, this, [this, item, thumbs]() {
            for (int k = 0; k < thumbs.size(); ++k) {
                static_cast<AuthenticationInfoItem *>(m_listGrp->getItem(k))->setEditTitle(item == m_listGrp->getItem(k));
            }
        });

        if(m_clearBtn->isChecked())
            item->setShowIcon(true);

        m_vecItem.append(item);
    }

    m_clearBtn->setVisible(m_listGrp->itemCount());
    // 如果指纹数量大于等于10,则不显示添加指纹按钮
    if (thumbs.size() >= 10) {
        return;
    }
    // 找到最小的指纹名以便作为缺省名添加
    for (int i = 0; i < m_model->getPredefineThumbsName().size(); ++i) {
        bool findNotUsedThumb = false;
        QString newFingerName = m_model->getPredefineThumbsName().at(i);
        for (int n = 0; n < 10 && n < thumbs.size(); ++n) {
            if (newFingerName == thumbs.at(n)) {
                findNotUsedThumb = true;
                break;
            }
        }
        if (!findNotUsedThumb) {
            addFingerButton(newFingerName);
            break;
        }
    }
}

void FingerWidget::addFingerButton(const QString &newFingerName)
{
    AuthenticationLinkButtonItem* addfingerItem = new AuthenticationLinkButtonItem(this);

    QString strAddFinger = tr("Add Fingerprint");
    DCommandLinkButton *addBtn = new DCommandLinkButton(strAddFinger);
    QHBoxLayout *fingerLayout = new QHBoxLayout(this);
    fingerLayout->addWidget(addBtn, 0, Qt::AlignLeft);
    fingerLayout->setContentsMargins(3, 5, 0, 5);
    addfingerItem->setLayout(fingerLayout);
    m_listGrp->insertItem(m_listGrp->itemCount(), addfingerItem);
    addfingerItem->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    DFontSizeManager::instance()->bind(addBtn, DFontSizeManager::T7);
    QFontMetrics fontMetrics(font());
    int nFontWidth = fontMetrics.horizontalAdvance(strAddFinger);
    addBtn->setMinimumWidth(nFontWidth);
    connect(addBtn, &DCommandLinkButton::clicked, this, [ = ] {
        Q_EMIT requestAddThumbs(m_currentUserName, newFingerName);
    });
    connect(addfingerItem, &AuthenticationLinkButtonItem::mousePressed, this, [ = ] {
        Q_EMIT requestAddThumbs(m_currentUserName, newFingerName);
    });
}
