//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "facewidget.h"
#include "widgets/titlelabel.h"
#include "charamangermodel.h"

#include <DFontSizeManager>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QEvent>

#define FACEID_NUM 5

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;

FaceWidget::FaceWidget(CharaMangerModel *model, QWidget *parent)
    : QWidget (parent)
    , m_model(model)
    , m_listGrp(new SettingsGroup(nullptr, SettingsGroup::GroupBackground))
    , m_clearBtn(new DCommandLinkButton(tr("Edit"), this))
{
    initUI();
    initConnect();
}

FaceWidget::~FaceWidget()
{

}

void FaceWidget::initUI()
{
    m_clearBtn->setCheckable(true);
    TitleLabel *facetitleLabel = new TitleLabel(tr("Manage Faces"), this);
    TitleLabel *maxFingerTip = new TitleLabel(tr("You can add up to 5 faces"), this);

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
    headLayout->addWidget(facetitleLabel, 0, Qt::AlignLeft);

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

void FaceWidget::initConnect()
{
    connect(m_model, &CharaMangerModel::enrollInfoState, this, [this](){
        Q_EMIT noticeEnrollCompleted(m_model->faceDriverName(), m_model->faceCharaType());
    });
    connect(m_model, &CharaMangerModel::facesListChanged, this, &FaceWidget::onFaceidListChanged);
    onFaceidListChanged(m_model->facesList());

}

void FaceWidget::addFaceButton(const QString &newFaceName)
{
    AuthenticationLinkButtonItem* addfaceItem = new AuthenticationLinkButtonItem(this);
    QString strAddFace = tr("Add Face");
    DCommandLinkButton *addBtn = new DCommandLinkButton(strAddFace);
    QHBoxLayout *faceLayout = new QHBoxLayout();
    faceLayout->addWidget(addBtn, 0, Qt::AlignLeft);
    // DCommandLinkButton 本身有2px 的间距 按ui要求上下保持一致
    faceLayout->setContentsMargins(3, 5, 0, 5);
    addfaceItem->setLayout(faceLayout);
    m_listGrp->insertItem(m_listGrp->itemCount(), addfaceItem);
    addfaceItem->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    DFontSizeManager::instance()->bind(addBtn, DFontSizeManager::T7);
    QFontMetrics fontMetrics(font());
    int nFontWidth = fontMetrics.horizontalAdvance(strAddFace);
    addBtn->setMinimumWidth(nFontWidth);
    connect(addBtn, &DCommandLinkButton::clicked, this, [ = ] {
        Q_EMIT requestAddFace(m_model->faceDriverName(), m_model->faceCharaType(), newFaceName);
    });
    connect(addfaceItem, &AuthenticationLinkButtonItem::mousePressed, this, [ = ] {
        Q_EMIT requestAddFace(m_model->faceDriverName(), m_model->faceCharaType(), newFaceName);
    });
}

void FaceWidget::onFaceidListChanged(const QStringList &facelist)
{
    m_vecItem.clear();
    m_listGrp->clear();

    for (int n = 0; n < FACEID_NUM && n < facelist.size(); ++n) {
        QString faceid = facelist.at(n);
        auto item = new AuthenticationInfoItem(this);
        item->setTitle(faceid);
        item->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        DFontSizeManager::instance()->bind(item, DFontSizeManager::T6);
        m_listGrp->appendItem(item);
        connect(item, &AuthenticationInfoItem::removeClicked, this, [this, faceid] {
            Q_EMIT requestDeleteFaceItem(m_model->faceCharaType(), faceid);
        });

        connect(item, &AuthenticationInfoItem::editTextFinished, this, [this, faceid, item, facelist, n](QString newName) {
            // 没有改名，直接返回
            if (item->getTitle() == newName) {
                return;
            }
            for (int i = 0; i < facelist.size(); ++i) {
                if (newName == facelist.at(i) && i != n) {
                    QString errMsg = tr("The name already exists");
                    item->showAlertMessage(errMsg);
                    return;
                }
            }
            item->setTitle(newName);
            Q_EMIT requestRenameFaceItem(m_model->faceCharaType(), faceid, newName);
            Q_EMIT noticeEnrollCompleted(m_model->faceDriverName(), m_model->faceCharaType());
        });

        connect(item, &AuthenticationInfoItem::editClicked, this, [this, item, facelist]() {
            for (int k = 0; k < facelist.size(); ++k) {
                static_cast<AuthenticationInfoItem *>(m_listGrp->getItem(k))->setEditTitle(item == m_listGrp->getItem(k));
            }
        });

        if (m_clearBtn->isChecked())
            item->setShowIcon(true);

        m_vecItem.append(item);
    }
    m_clearBtn->setVisible(m_listGrp->itemCount());

    if (facelist.size() >= FACEID_NUM) {
        return;
    }

    // 找到最小的名称以便作为缺省名添加
    for (int i = 0; i < FACEID_NUM; ++i) {
        bool findNotUsedThumb = false;
        QString newName(tr("Faceprint") + QString("%1").arg(i + 1));

        for (int n = 0; n < FACEID_NUM && n < facelist.size(); ++n) {
            if (newName == facelist.at(n)) {
                findNotUsedThumb = true;
                break;
            }
        }

        if (!findNotUsedThumb) {
            addFaceButton(newName);
            break;
        }
    }
}
