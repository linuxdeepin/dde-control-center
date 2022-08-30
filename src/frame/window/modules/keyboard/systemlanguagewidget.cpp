// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "systemlanguagewidget.h"
#include "checkitem.h"
#include "systemlanguagesettingwidget.h"
#include "window/utils.h"
#include "widgets/settingsgroup.h"
#include "widgets/translucentframe.h"
#include "widgets/settingshead.h"
#include "widgets/comboxwidget.h"
#include "modules/keyboard/checkitem.h"
#include "modules/keyboard/keylabel.h"
#include "modules/keyboard/keyboardmodel.h"

#include <DFloatingButton>
#include <DAnchors>

#include <QStringList>
#include <QVBoxLayout>
#include <QComboBox>
#include <QDebug>
#include <QList>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::keyboard;
using namespace dcc;
using namespace dcc::keyboard;
using namespace dcc::widgets;

SystemLanguageWidget::SystemLanguageWidget(KeyboardModel *model, QWidget *parent)
    : QWidget(parent)
    , m_model(model)
    , m_settingWidget(nullptr)
{
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setSpacing(List_Interval);

    QHBoxLayout *headLayout = new QHBoxLayout();
    //~ contents_path /keyboard/System Language
    //~ child_page System Language
    TitleLabel *headTitle = new TitleLabel(tr("Language List"));
    DFontSizeManager::instance()->bind(headTitle, DFontSizeManager::T5, QFont::DemiBold); // 设置label字体
    m_editSystemLang = new DCommandLinkButton(tr("Edit"));
    m_editSystemLang->setObjectName("Edit");
    headLayout->addWidget(headTitle);
    headTitle->setContentsMargins(10, 0, 0, 0);
    headLayout->addStretch();
    headLayout->addWidget(m_editSystemLang);

    m_langListview = new DListView();
    m_langListview->setAccessibleName("SystemLanguageWidget_langListview");
    m_langListview->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_langListview->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    m_langListview->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    m_langListview->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_langListview->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_langListview->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_langListview->setSelectionMode(QAbstractItemView::NoSelection);
    m_langListview->setContentsMargins(ListViweItemMargin);

    m_langItemModel = new QStandardItemModel(this);
    m_langListview->setModel(m_langItemModel);

    layout->addWidget(m_langListview);
    layout->setContentsMargins(0, 0, 10, 0);

    QWidget *widget = new QWidget(this);
    widget->setAccessibleName("SystemLanguageWidget_QWidget");
    widget->setLayout(layout);
    ContentWidget *contentWidget = new ContentWidget(this);
    contentWidget->setAccessibleName("SystemLanguageWidget_Content");
    contentWidget->setContent(widget);

    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->addLayout(headLayout);
    vLayout->setSpacing(List_Interval);
    vLayout->addWidget(contentWidget);
    vLayout->setAlignment(Qt::AlignTop);

    DFloatingButton *addSystemLanguage = new DFloatingButton(DStyle::SP_IncreaseElement, this);
    addSystemLanguage->setObjectName("AddSystemLanguage");
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->setMargin(0);
    btnLayout->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
    btnLayout->addWidget(addSystemLanguage);
    vLayout->addLayout(btnLayout);
    vLayout->setContentsMargins(10, 10, 10, 5);
    setLayout(vLayout);

    connect(m_langListview, &DListView::clicked, this, &SystemLanguageWidget::setCurLangChecked);
    connect(addSystemLanguage, &DFloatingButton::clicked, this, &SystemLanguageWidget::onSystemLanguageAdded);
    connect(m_editSystemLang, &QPushButton::clicked, this, &SystemLanguageWidget::onEditClicked);

    connect(m_model, &KeyboardModel::curLocalLangChanged, this, [this](const QStringList &curLocalLang) {
        for (int i = 0; i < curLocalLang.size(); i++) {
            onAddLanguage(curLocalLang[i]);
        }
    });
    connect(m_model, &KeyboardModel::curLangChanged, this, &SystemLanguageWidget::onDefault);
    QStringList localLangList = m_model->localLang();
    for (int i = 0; i < localLangList.size(); i++) {
        onAddLanguage(localLangList[i]);
    }
    onDefault(m_model->curLang());
    onSetCurLang(m_model->getLangChangedState());
}

void SystemLanguageWidget::onEditClicked()
{
    m_bEdit = !m_bEdit;
    if (m_bEdit) {
        m_editSystemLang->setText(tr("Done"));
        int row_count = m_langItemModel->rowCount();
        for (int i = 0; i < row_count; ++i) {
            DStandardItem *item = dynamic_cast<DStandardItem *>(m_langItemModel->item(i, 0));
            if (item && (item->checkState() == Qt::Unchecked)) {
                DViewItemAction *iconAction = new DViewItemAction(Qt::AlignCenter | Qt::AlignRight, QSize(), QSize(), true);
                iconAction->setIcon(DStyle::standardIcon(style(), DStyle::SP_DeleteButton));
                item->setActionList(Qt::RightEdge, {iconAction});
                connect(iconAction, &DViewItemAction::triggered, this, [this,item] {
                    m_sysLanglist.removeOne(item->text());
                    int idx = m_langItemModel->indexFromItem(item).row();
                    Q_EMIT delLocalLang(item->text());
                    m_langItemModel->removeRow(idx);
                    m_langListview->adjustSize();
                    m_langListview->update();
                    m_editSystemLang->setVisible(m_sysLanglist.size() > 1);
                });
            }
        }
    } else {
        m_editSystemLang->setText(tr("Edit"));
        int row_count = m_langItemModel->rowCount();
        for (int i = 0; i < row_count; ++i) {
            DStandardItem *item = dynamic_cast<DStandardItem *>(m_langItemModel->item(i, 0));
            if (item && (item->checkState() == Qt::Unchecked)) {
                item->setActionList(Qt::RightEdge, {});
            }
        }
    }
}

void SystemLanguageWidget::onAddLanguage(const QString &localeLang)
{
    if (m_sysLanglist.contains(localeLang))
        return;

    DStandardItem *item = new DStandardItem(localeLang);
    m_langItemModel->appendRow(item);
    m_langListview->adjustSize();
    m_langListview->update();
    m_sysLanglist << localeLang;
    m_editSystemLang->setVisible(m_sysLanglist.size() > 1);
}

void SystemLanguageWidget::setCurLangChecked(const QModelIndex &index)
{
    if (m_bEdit) {
        return;
    }
    int row_count = m_langItemModel->rowCount();
    for (int i = 0; i < row_count; ++i) {
        QStandardItem *item = m_langItemModel->item(i, 0);
        if (item && (index.row() == i)) {
            item->setCheckState(Qt::Checked);
            QString langKey = m_model->langFromText(item->text());
            Q_EMIT setCurLang(langKey);
        } else if (item) {               //如果不加此判断，item会出现空指针
            item->setCheckState(Qt::Unchecked);
        }
    }
}

void SystemLanguageWidget::onDefault(const QString &curLang)
{
    qDebug() << "curLang is " << curLang;
    int row_count = m_langItemModel->rowCount();
    for (int i = 0; i < row_count; ++i) {
        QStandardItem *item = m_langItemModel->item(i, 0);
        if (item && (item->text() == curLang)) {
            item->setCheckState(Qt::Checked);
        } else {
            item->setCheckState(Qt::Unchecked);
        }
    }
}

void SystemLanguageWidget::onSetCurLang(int value)
{
    qDebug() << "m_langListview & m_editSystemLang" << value;
    m_langListview->setEnabled(!value);
    m_editSystemLang->setEnabled(!value);
}
