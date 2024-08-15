//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "iriswidget.h"
#include "widgets/titlelabel.h"
#include "charamangermodel.h"

#include <DFontSizeManager>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QEvent>

#define IRISID_NUM 5

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;

IrisWidget::IrisWidget(CharaMangerModel *model, QWidget *parent)
    : QWidget (parent)
    , m_model(model)
    , m_listGrp(new SettingsGroup(nullptr, SettingsGroup::GroupBackground))
    , m_clearBtn(new DCommandLinkButton(tr("Edit"), this))
{
    initUI();
    initConnect();
}

IrisWidget::~IrisWidget()
{

}

void IrisWidget::initUI()
{
    m_clearBtn->setCheckable(true);

    TitleLabel *titleLabel = new TitleLabel(tr("Manage Irises"), this);
    TitleLabel *maxFingerTip = new TitleLabel(tr("You can add up to 5 irises"), this);

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
    headLayout->addWidget(titleLabel, 0, Qt::AlignLeft);

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

void IrisWidget::initConnect()
{
    connect(m_model, &CharaMangerModel::enrollInfoState, this, [this](){
        Q_EMIT noticeEnrollCompleted(m_model->irisDriverName(), m_model->irisCharaType());
    });
    connect(m_model, &CharaMangerModel::irisListChanged, this, &IrisWidget::onIrisListChanged);
    onIrisListChanged(m_model->irisList());

}

void IrisWidget::addIrisButton(const QString &newIrisName)
{
    AuthenticationLinkButtonItem* addItem = new AuthenticationLinkButtonItem(this);

    QString strAddIris = tr("Add Iris");
    DCommandLinkButton *addBtn = new DCommandLinkButton(strAddIris);
    QHBoxLayout *irisLayout = new QHBoxLayout(this);
    irisLayout->addWidget(addBtn, 0, Qt::AlignLeft);
    irisLayout->setContentsMargins(3, 5, 0, 5);
    addItem->setLayout(irisLayout);
    m_listGrp->insertItem(m_listGrp->itemCount(), addItem);
    addItem->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    DFontSizeManager::instance()->bind(addBtn, DFontSizeManager::T7);
    QFontMetrics fontMetrics(font());
    int nFontWidth = fontMetrics.horizontalAdvance(strAddIris);
    addBtn->setMinimumWidth(nFontWidth);
    connect(addBtn, &DCommandLinkButton::clicked, this, [ = ] {
        Q_EMIT requestAddIris(m_model->irisDriverName(), m_model->irisCharaType(), newIrisName);
    });
    connect(addItem, &AuthenticationLinkButtonItem::mousePressed, this, [ = ] {
        Q_EMIT requestAddIris(m_model->irisDriverName(), m_model->irisCharaType(), newIrisName);
    });
}

void IrisWidget::onIrisListChanged(const QStringList &irislist)
{
    m_vecItem.clear();
    m_listGrp->clear();

    for (int n = 0; n < IRISID_NUM && n < irislist.size(); ++n) {
        QString irisid = irislist.at(n);
        auto item = new AuthenticationInfoItem(this);
        item->setTitle(irisid);
        item->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        DFontSizeManager::instance()->bind(item, DFontSizeManager::T6);
        m_listGrp->appendItem(item);
        connect(item, &AuthenticationInfoItem::removeClicked, this, [this, irisid] {
            Q_EMIT requestDeleteIrisItem(m_model->irisCharaType(), irisid);
        });

        connect(item, &AuthenticationInfoItem::editTextFinished, this, [this, irisid, item, irislist, n](QString newName) {
            // 没有改名，直接返回
            if (item->getTitle() == newName) {
                return;
            }
            for (int i = 0; i < irislist.size(); ++i) {
                if (newName == irislist.at(i) && i != n) {
                    QString errMsg = tr("The name already exists");
                    item->showAlertMessage(errMsg);
                    return;
                }
            }
            item->setTitle(newName);
            Q_EMIT requestRenameIrisItem(m_model->irisCharaType(), irisid, newName);
            Q_EMIT noticeEnrollCompleted(m_model->irisDriverName(), m_model->irisCharaType());
        });

        connect(item, &AuthenticationInfoItem::editClicked, this, [this, item, irislist]() {
            for (int k = 0; k < irislist.size(); ++k) {
                static_cast<AuthenticationInfoItem *>(m_listGrp->getItem(k))->setEditTitle(item == m_listGrp->getItem(k));
            }
        });

        if (m_clearBtn->isChecked())
            item->setShowIcon(true);

        m_vecItem.append(item);
    }
    m_clearBtn->setVisible(m_listGrp->itemCount());

    if (irislist.size() >= IRISID_NUM) {
        return;
    }

    // 找到最小的名称以便作为缺省名添加
    for (int i = 0; i < IRISID_NUM; ++i) {
        bool findNotUsedThumb = false;
        QString newName(tr("Iris") + QString("%1").arg(i + 1));

        for (int n = 0; n < IRISID_NUM && n < irislist.size(); ++n) {
            if (newName == irislist.at(n)) {
                findNotUsedThumb = true;
                break;
            }
        }

        if (!findNotUsedThumb) {
            addIrisButton(newName);
            break;
        }
    }
}
