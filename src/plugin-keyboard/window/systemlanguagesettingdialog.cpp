//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "systemlanguagesettingdialog.h"
#include "src/plugin-keyboard/operation/keyboardmodel.h"
#include "src/plugin-keyboard/window/indexview.h"

#include <DTitlebar>

using namespace DCC_NAMESPACE;
SystemLanguageSettingDialog::SystemLanguageSettingDialog(KeyboardModel *model, QWidget *parent)
    : DAbstractDialog(parent)
    , m_searchStatus(false)
    , m_keyboardModel(model)
    , m_buttonTuple(new ButtonTuple(ButtonTuple::Save))
{
    setFixedSize(QSize(500, 644));
    QVBoxLayout *mainVLayout = new QVBoxLayout();
    mainVLayout->setContentsMargins(0, 0, 0, 5);
    mainVLayout->setSpacing(0);

    m_model = new QStandardItemModel(this);
    m_view = new DListView();
    m_view->setAccessibleName("List_languagelist");
    m_view->setFrameShape(QFrame::NoFrame);
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_view->setBackgroundType(DStyledItemDelegate::ClipCornerBackground);
    m_view->setSelectionMode(QAbstractItemView::NoSelection);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QPushButton *cancel = m_buttonTuple->leftButton();
    cancel->setText(tr("Cancel"));
    cancel->setObjectName("Cancel");
    QPushButton *ok = m_buttonTuple->rightButton();
    ok->setText(tr("Add"));
    ok->setEnabled(false);
    ok->setObjectName("Ok");

    m_search = new SearchInput();
    QVBoxLayout *listVLayout = new QVBoxLayout();
    listVLayout->setAlignment(Qt::AlignHCenter);
    DTitlebar *titleIcon = new DTitlebar();
    titleIcon->setFrameStyle(QFrame::NoFrame);//无边框
    titleIcon->setBackgroundTransparent(true);//透明
    titleIcon->setMenuVisible(false);
    titleIcon->setTitle(tr(""));

    QLabel *headTitle = new QLabel(tr("Add System Language"));
    DFontSizeManager::instance()->bind(headTitle, DFontSizeManager::T5, QFont::DemiBold); // 设置label字体
    headTitle->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    headTitle->setAlignment(Qt::AlignCenter);

    listVLayout->addSpacing(10);
    listVLayout->addWidget(m_search);
    listVLayout->addSpacing(10);
    listVLayout->addWidget(m_view);
    listVLayout->addSpacing(10);
    listVLayout->addWidget(m_buttonTuple, 0, Qt::AlignBottom);
    listVLayout->setContentsMargins(20, 10, 20, 10);

    mainVLayout->setMargin(0);
    mainVLayout->addWidget(titleIcon, Qt::AlignTop | Qt::AlignRight);
    mainVLayout->addWidget(headTitle, Qt::AlignCenter);
    mainVLayout->addLayout(listVLayout);
    setLayout(mainVLayout);
    setContentsMargins(0, 0, 0, 0);

    installEventFilter(this);

    connect(m_search, &SearchInput::textChanged, this, &SystemLanguageSettingDialog::onSearch);
    connect(m_keyboardModel, &KeyboardModel::langChanged, this, &SystemLanguageSettingDialog::setModelData);
    connect(cancel, &QPushButton::clicked, this, &SystemLanguageSettingDialog::close);
    connect(ok, &QPushButton::clicked, this, &SystemLanguageSettingDialog::onAddLanguage);
    connect(m_view, &DListView::clicked, this, &SystemLanguageSettingDialog::onLangSelect);

    setModelData(m_keyboardModel->langLists());
}

SystemLanguageSettingDialog::~SystemLanguageSettingDialog()
{
}

void SystemLanguageSettingDialog::onSearch(const QString &text)
{
    if (text.length() == 0) {
        m_searchStatus = false;
        m_view->setModel(m_model);
    } else {
        m_searchStatus = true;
        m_searchModelIndex = QModelIndex();
        m_searchModel = new QStandardItemModel(this);

        for (auto md : m_datas) {
            if (md.text().contains(text, Qt::CaseInsensitive)) {
                auto item = new DStandardItem(md.text());
                item->setText(md.text());
                item->setData(md.key(),KeyRole);
                item->setData(md.pinyin(),PingYinRole);
                m_searchModel->appendRow(item);
            }
        }
        m_view->setModel(m_searchModel);
    }
}

void SystemLanguageSettingDialog::onAddLanguage()
{
    if(m_searchStatus) {
        Q_EMIT click(m_searchModelIndex);
    } else{
        Q_EMIT click(m_modelIndex);
    }

    accept();
}

void SystemLanguageSettingDialog::onLangSelect(const QModelIndex &index)
{
    if(m_searchStatus) {
        updateDataModel(m_searchModel, m_searchModelIndex, index);
    } else {
        updateDataModel(m_model, m_modelIndex, index);
    }
}

void SystemLanguageSettingDialog::updateDataModel(QStandardItemModel *model, QModelIndex &selectedIndex, const QModelIndex &index) {

    if (selectedIndex.isValid()) {
        model->itemFromIndex(selectedIndex)->setCheckState(Qt::Unchecked);
    }

    QStandardItem *selectedItem = model->itemFromIndex(index);
    if (selectedItem) {
        selectedItem->setCheckState(Qt::Checked);
        selectedIndex = index;
        m_buttonTuple->rightButton()->setEnabled(true);
    }
}

void SystemLanguageSettingDialog::setModelData(const QList<MetaData> &datas)
{
    m_datas = datas;
    QStringList removeLangList = m_keyboardModel->localLang();
    for (QList<MetaData>::iterator iter = m_datas.begin(); iter != m_datas.end();) {
        if (removeLangList.contains(iter->text())) {
            iter = m_datas.erase(iter);
            continue;
        }
        ++iter;
    }

    for (auto md : m_datas) {
        auto item = new DStandardItem();
        item->setText(md.text());
        item->setData(md.key(),KeyRole);
        item->setData(md.pinyin(),PingYinRole);
        m_model->appendRow(item);
    }
    m_view->setModel(m_model);
}

bool SystemLanguageSettingDialog::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched)

    if (event->type() != QEvent::Move && event->type() != QEvent::Resize)
        return false;

    QRect rect = this->rect();

    rect.moveTopLeft(-pos());
    rect.setHeight(window()->height() - mapTo(window(), rect.topLeft()).y());

    QPainterPath path;
    path.addRoundedRect(rect, 5, 5);

    return false;
}
