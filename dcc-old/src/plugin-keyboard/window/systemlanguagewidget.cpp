// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "systemlanguagewidget.h"

#include "src/plugin-keyboard/operation/keyboardmodel.h"
#include "src/plugin-keyboard/window/keylabel.h"
#include "systemlanguagesettingdialog.h"
#include "widgets/comboxwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/settingshead.h"
#include "widgets/titlelabel.h"

#include <DAnchors>
#include <DStyle>

#include <QComboBox>
#include <QDebug>
#include <QList>
#include <QStringList>
#include <QVBoxLayout>

#include <functional>

DCORE_USE_NAMESPACE
DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;

SystemLanguageWidget::SystemLanguageWidget(KeyboardModel *model, QWidget *parent)
    : QWidget(parent)
    , m_model(model)
    , m_langItemModel(new QStandardItemModel(this))
    , m_langListview(std::visit([this]() -> SystemLanguageListView * {
        auto langListview = new SystemLanguageListView(this);
        langListview->setAccessibleName("SystemLanguageWidget_langListview");
        langListview->setContentsMargins(10, 0, 10, 0);
        return langListview;
    }))
    , m_editSystemLang(std::visit([this]() -> DCommandLinkButton * {
        auto editSystemLang = new DCommandLinkButton(tr("Edit"), this);
        editSystemLang->setObjectName("Edit");
        return editSystemLang;
    }))
    , m_settingWidget(nullptr)
{
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setSpacing(10);

    QHBoxLayout *headLayout = new QHBoxLayout();
    TitleLabel *headTitle = new TitleLabel(tr("Language List"));
    DFontSizeManager::instance()->bind(headTitle,
                                       DFontSizeManager::T5,
                                       QFont::DemiBold); // 设置label字体
    headLayout->addWidget(headTitle);
    headTitle->setContentsMargins(10, 0, 0, 0);
    headLayout->addStretch();
    headLayout->addWidget(m_editSystemLang);

    m_langListview->setModel(m_langItemModel);

    DStandardItem *footItem = new DStandardItem(tr("Add Language") + "...");
    footItem->setTextColorRole(DPalette::Highlight);
    m_langItemModel->appendRow(footItem);

    layout->addLayout(headLayout);
    layout->addWidget(m_langListview);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    connect(m_langListview, &DListView::clicked, this, &SystemLanguageWidget::setCurLangChecked);
    connect(m_editSystemLang, &QPushButton::clicked, this, &SystemLanguageWidget::onEditClicked);

    connect(m_model,
            &KeyboardModel::curLocalLangChanged,
            this,
            [this](const QStringList &curLocalLang) {
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
        for (int i = 0; i < row_count - 1; ++i) {
            DStandardItem *item = dynamic_cast<DStandardItem *>(m_langItemModel->item(i, 0));
            if (item && (item->checkState() == Qt::Unchecked)) {
                DViewItemAction *iconAction = new DViewItemAction(Qt::AlignCenter | Qt::AlignRight,
                                                                  QSize(),
                                                                  QSize(),
                                                                  true);
                iconAction->setIcon(DStyle::standardIcon(style(), DStyle::SP_DeleteButton));
                item->setActionList(Qt::RightEdge, { iconAction });
                connect(iconAction, &DViewItemAction::triggered, this, [this, item] {
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

    // 去除最后一个item
    DStandardItem *endItem = nullptr;
    if (m_langItemModel->rowCount() > 0) {
        endItem = dynamic_cast<DStandardItem *>(
                m_langItemModel->takeItem(m_langItemModel->rowCount() - 1, 0));
        m_langItemModel->removeRow(m_langItemModel->rowCount() - 1);
    }

    DStandardItem *item = new DStandardItem(localeLang);
    m_langItemModel->appendRow(item);

    // 添加最后一个item
    if (endItem != nullptr) {
        m_langItemModel->appendRow(endItem);
    }

    m_langListview->adjustSize();
    m_langListview->update();
    m_sysLanglist << localeLang;
    m_editSystemLang->setVisible(m_sysLanglist.size() > 1);
}

void SystemLanguageWidget::setCurLangChecked(const QModelIndex &index)
{
    if (index.row() == m_langListview->count() - 1) {
        addSystemLanguage();
        return;
    }
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
        } else if (item) { // 如果不加此判断，item会出现空指针
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
    setEnabled(!value);
}

void SystemLanguageWidget::addSystemLanguage()
{
    m_bEdit = false;
    m_editSystemLang->setText(tr("Edit"));
    int row_count = m_langItemModel->rowCount();
    for (int i = 0; i < row_count; ++i) {
        DStandardItem *item = dynamic_cast<DStandardItem *>(m_langItemModel->item(i, 0));
        if (item && (item->checkState() == Qt::Unchecked)) {
            item->setActionList(Qt::RightEdge, {});
        }
    }
    emit onSystemLanguageAdded();
}
