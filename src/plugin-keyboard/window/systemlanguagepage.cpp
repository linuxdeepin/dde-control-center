// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "systemlanguagepage.h"

#include "itemmodule.h"
#include "src/plugin-keyboard/operation/keyboardmodel.h"
#include "src/plugin-keyboard/operation/keyboardwork.h"
#include "systemlanguagesettingdialog.h"

#include <DAnchors>
#include <DStyle>

#include <QComboBox>
#include <QDebug>
#include <QList>
#include <QStringList>
#include <QVBoxLayout>

DCORE_USE_NAMESPACE
DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;

SystemLanguagePage::SystemLanguagePage(KeyboardModel *model,
                                           KeyboardWorker *worker,
                                           QWidget *parent)
    : PageModule("keyboardLanguage", tr("Language"))
    , m_model(model)
    , m_worker(worker)
{
    m_worker->refreshLang();
    connect(this, &SystemLanguagePage::delLocalLang, m_worker, &KeyboardWorker::deleteLang);
    connect(this, &SystemLanguagePage::setCurLang, m_worker, &KeyboardWorker::setLang);
    connect(m_model, &KeyboardModel::onSetCurLangFinish, this, &SystemLanguagePage::onSetCurLang);
    connect(this,
            &SystemLanguagePage::onSystemLanguageAdded,
            this,
            &SystemLanguagePage::onPushSystemLanguageSetting);
    connect(m_model,
            &KeyboardModel::curLocalLangChanged,
            this,
            [this](const QStringList &curLocalLang) {
                for (int i = 0; i < curLocalLang.size(); i++) {
                    emit addLanguage(curLocalLang[i]);
                }
            });
    connect(m_model, &KeyboardModel::curLangChanged, this, &SystemLanguagePage::defaultChanged);
    QStringList localLangList = m_model->localLang();
    for (int i = 0; i < localLangList.size(); i++) {
        emit addLanguage(localLangList[i]);
    }
    onSetCurLang(m_model->getLangChangedState());

    initUI();
}

void SystemLanguagePage::initUI()
{
    ItemModule *item = new ItemModule("languagelist", tr("Language List"));
    item->setRightWidget([this](ModuleObject *) {
        return this->initTitleBtn();
    });
    appendChild(item);
    appendChild(new ItemModule(
            "Language",
            tr("Language List"),
            [this](ModuleObject *) -> QWidget * {
                return this->initListView();
            },
            false));
}

DCommandLinkButton *SystemLanguagePage::initTitleBtn()
{
    DCommandLinkButton *editSystemLang = new DCommandLinkButton(tr("Edit"));
    editSystemLang->setObjectName("Edit");
    bool isenabled = !m_model->getLangChangedState();
    qDebug() << "m_langListview & m_editSystemLang" << isenabled;
    editSystemLang->setEnabled(isenabled);
    connect(this,
            &SystemLanguagePage::editSystemLangeSetText,
            editSystemLang,
            [=](const QString &text) {
                editSystemLang->setText(text);
            });
    connect(this,
            &SystemLanguagePage::editSystemLangeSetVisible,
            editSystemLang,
            [=](const bool visible) {
                editSystemLang->setVisible(visible);
            });
    return editSystemLang;
}

SystemLanguageListView *SystemLanguagePage::initListView()
{
    SystemLanguageListView *langListview = new SystemLanguageListView();
    langListview->setAccessibleName("SystemLanguageWidget_langListview");
    langListview->setEditTriggers(QAbstractItemView::NoEditTriggers);
    langListview->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    langListview->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    langListview->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    langListview->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    langListview->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    langListview->setSelectionMode(QAbstractItemView::NoSelection);
    langListview->setContentsMargins(10, 0, 10, 0);

    QStandardItemModel *langItemModel = new QStandardItemModel(this);
    langListview->setModel(langItemModel);

    // add btn
    DCommandLinkButton *btn = new DCommandLinkButton(tr("Add Language") + "...", langListview);
    btn->setObjectName("AddSystemLanguage");
    DViewItemAction *addLayoutAction = new DViewItemAction(Qt::AlignLeft | Qt::AlignVCenter,
                                                           QSize(10, 10),
                                                           QSize(10, 10),
                                                           false);
    addLayoutAction->setWidget(btn);
    btn->setMaximumHeight(22);
    DStandardItem *kbLayoutItem = new DStandardItem();
    kbLayoutItem->setActionList(Qt::LeftEdge, { addLayoutAction });
    langItemModel->appendRow(kbLayoutItem);

    bool isenabled = !m_model->getLangChangedState();
    qDebug() << "m_langListview & m_editSystemLang" << isenabled;

    langListview->setEnabled(isenabled);

    // INFO: onDefault
    QString curLang = m_model->curLang();

    qDebug() << "curLang is " << curLang;
    int row_count = langItemModel->rowCount();
    for (int i = 0; i < row_count; ++i) {
        QStandardItem *item = langItemModel->item(i, 0);
        if (item && (item->text() == curLang)) {
            item->setCheckState(Qt::Checked);
        } else {
            item->setCheckState(Qt::Unchecked);
        }
    }
    connect(this, &SystemLanguagePage::defaultChanged, this, [=](const QString &curLang) {
        qDebug() << "curLang is " << curLang;
        int row_count = langItemModel->rowCount();
        for (int i = 0; i < row_count; ++i) {
            QStandardItem *item = langItemModel->item(i, 0);
            if (item && (item->text() == curLang)) {
                item->setCheckState(Qt::Checked);
            } else {
                item->setCheckState(Qt::Unchecked);
            }
        }
    });
    connect(langListview, &DListView::clicked, this, [=](const QModelIndex &index) {
        if (m_bEdit) {
            return;
        }
        int row_count = langItemModel->rowCount();
        for (int i = 0; i < row_count; ++i) {
            QStandardItem *item = langItemModel->item(i, 0);
            if (item && (index.row() == i)) {
                item->setCheckState(Qt::Checked);
                QString langKey = m_model->langFromText(item->text());
                Q_EMIT setCurLang(langKey);
            } else if (item) { // 如果不加此判断，item会出现空指针
                item->setCheckState(Qt::Unchecked);
            }
        }
    });
    connect(static_cast<DCommandLinkButton *>(addLayoutAction->widget()),
            &DFloatingButton::clicked,
            this,
            [=]() {
                m_bEdit = false;
                if (!m_bEdit) {
                    emit editSystemLangeSetText(tr("Edit"));
                    int row_count = langItemModel->rowCount();
                    for (int i = 0; i < row_count; ++i) {
                        DStandardItem *item =
                                dynamic_cast<DStandardItem *>(langItemModel->item(i, 0));
                        if (item && (item->checkState() == Qt::Unchecked)) {
                            item->setActionList(Qt::RightEdge, {});
                        }
                    }
                }
            });
    connect(static_cast<DCommandLinkButton *>(addLayoutAction->widget()),
            &DFloatingButton::clicked,
            this,
            &SystemLanguagePage::onSystemLanguageAdded);
    connect(this, &SystemLanguagePage::addLanguage, this, [=](const QString &localeLang) {
        if (m_sysLanglist.contains(localeLang))
            return;

        // 去除最后一个item
        DStandardItem *endItem = nullptr;
        if (langItemModel->rowCount() > 0) {
            endItem = dynamic_cast<DStandardItem *>(
                    langItemModel->takeItem(langItemModel->rowCount() - 1, 0));
            langItemModel->removeRow(langItemModel->rowCount() - 1);
        }

        DStandardItem *item = new DStandardItem(localeLang);
        langItemModel->appendRow(item);

        // 添加最后一个item
        if (endItem != nullptr) {
            langItemModel->appendRow(endItem);
        }

        langListview->adjustSize();
        langListview->update();
        m_sysLanglist << localeLang;
        emit editSystemLangeSetVisible(m_sysLanglist.size() > 1);
    });
    return langListview;
}

void SystemLanguagePage::onEditClicked()
{
    m_bEdit = !m_bEdit;
}

void SystemLanguagePage::onSetCurLang(int value)
{
    qDebug() << "m_langListview & m_editSystemLang" << value;
}

void SystemLanguagePage::onPushSystemLanguageSetting()
{
    SystemLanguageSettingDialog *systemLanguageSettingDialog =
            new SystemLanguageSettingDialog(m_model);
    connect(systemLanguageSettingDialog,
            &SystemLanguageSettingDialog::click,
            this,
            &SystemLanguagePage::onAddLocale);
    systemLanguageSettingDialog->setAttribute(Qt::WA_DeleteOnClose);
    systemLanguageSettingDialog->exec();
}

void SystemLanguagePage::onAddLocale(const QModelIndex &index)
{
    QVariant var = index.data(SystemLanguageSettingDialog::KeyRole);
    m_worker->addLang(var.toString());
}
