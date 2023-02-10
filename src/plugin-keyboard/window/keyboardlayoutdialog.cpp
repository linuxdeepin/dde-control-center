//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "keyboardlayoutdialog.h"
#include "widgets/settingsgroup.h"
#include "widgets/settingsitem.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QEvent>
#include <QLocale>
#include <QPushButton>

#include <DTipLabel>
#include <DTitlebar>

using namespace DCC_NAMESPACE;
KeyboardLayoutDialog::KeyboardLayoutDialog(QWidget *parent)
    : DAbstractDialog(parent)
    , textLength(0)
    , searchStatus(false)
    , m_buttonTuple(new ButtonTuple(ButtonTuple::Save))
{
    setFixedSize(QSize(500, 644));
    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setMargin(0);
    hlayout->setSpacing(0);

    m_searchModel = new IndexModel();
    m_model = new IndexModel();
    m_view = new IndexView();

    m_view->setAccessibleName("List_keyboardmenulist");
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    m_view->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    m_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setSelectionMode(QAbstractItemView::NoSelection);

    QPushButton *cancel = m_buttonTuple->leftButton();
    cancel->setText(tr("Cancel"));
    QPushButton *ok = m_buttonTuple->rightButton();
    ok->setText(tr("Add"));
    ok->setEnabled(false);

    hlayout->addWidget(m_view);

    QLabel *headTitle = new QLabel(tr("Add Keyboard Layout"));
    DFontSizeManager::instance()->bind(headTitle, DFontSizeManager::T5, QFont::DemiBold); // 设置label字体
    headTitle->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    headTitle->setAlignment(Qt::AlignCenter);

    m_search = new SearchInput();
    QVBoxLayout *mainVLayout = new QVBoxLayout(this);
    QVBoxLayout *listVLayout = new QVBoxLayout();
    listVLayout->setAlignment(Qt::AlignHCenter);
    DTitlebar *titleIcon = new DTitlebar();
    titleIcon->setFrameStyle(QFrame::NoFrame);//无边框
    titleIcon->setBackgroundTransparent(true);//透明
    titleIcon->setMenuVisible(false);
    titleIcon->setTitle(tr(""));

    listVLayout->addSpacing(10);
    listVLayout->addWidget(m_search, Qt::AlignCenter);
    listVLayout->addSpacing(10);
    listVLayout->addLayout(hlayout);
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

    connect(m_search, SIGNAL(textChanged(QString)), this, SLOT(onSearch(QString)));
    connect(cancel, &QPushButton::clicked, this, &KeyboardLayoutDialog::close);
    connect(ok, &QPushButton::clicked, this, &KeyboardLayoutDialog::onAddKBLayout);
    connect(m_view, &IndexView::clicked, this, &KeyboardLayoutDialog::onKBLayoutSelect);
}

KeyboardLayoutDialog::~KeyboardLayoutDialog()
{
    m_searchModel->deleteLater();
    m_model->deleteLater();
}

void KeyboardLayoutDialog::onAddKBLayout()
{
    QVariant var;
    MetaData md;

    if (searchStatus) {
        var = m_selectSearchIndex.data(IndexModel::KBLayoutRole);
        md = var.value<MetaData>();
        if (m_searchModel->letters().contains(md.text())) {
            return;
        }
    } else {
        var = m_selectIndex.data(IndexModel::KBLayoutRole);
        md = var.value<MetaData>();
        if (m_model->letters().contains(md.text())) {
            return;
        }
    }

    Q_EMIT layoutSelected(md.text());
    accept();
}

void KeyboardLayoutDialog::onKBLayoutSelect(const QModelIndex &index)
{
    if (searchStatus) {
        setDataModel(m_searchModel, m_selectSearchIndex, index);
    } else{
        setDataModel(m_model, m_selectIndex, index);
    }
}

void KeyboardLayoutDialog::setDataModel(IndexModel *model, QModelIndex &selectedIndex, const QModelIndex &index) {

    if (selectedIndex.isValid()) {
        model->itemFromIndex(selectedIndex)->setCheckState(Qt::Unchecked);
    }

    QStandardItem *selectItem = model->itemFromIndex(index);

    if (selectItem) {
        bool addBtnEnabled = true;
        QVariant var = index.data(IndexModel::KBLayoutRole);
        MetaData md = var.value<MetaData>();
        if (md.text().isEmpty() || model->letters().contains(md.text())) {
            addBtnEnabled = false;
        } else {
            selectItem->setCheckState(Qt::Checked);
            selectedIndex = index;
        }
        m_buttonTuple->rightButton()->setEnabled(addBtnEnabled);
    }
}

void KeyboardLayoutDialog::setMetaData(const QList<MetaData> &datas)
{
    int count = datas.count();
    m_data.clear();
    for (int i = 0; i < count; i++) {
        //当前key不为空，直接添加到list
        if ("" != datas[i].key()) {
            m_data.append(datas[i]);
        } else {
            //当前key为空，但是下一个key不为空(表示这是一个字母，如"H"),需要添加到list
            //添加前要进行list数量判断， 需要满足　: i　< count -1
            if ((i < count - 1) && ("" != datas[i + 1].key())) {
                m_data.append(datas[i]);
            }
        }
    }

    m_model->setMetaData(m_data);
    m_view->setModel(m_model);
}

void KeyboardLayoutDialog::setLetters(QList<QString> letters)
{
    QLocale locale;
    if (locale.language() == QLocale::Chinese) {
        //根据有效list，决定显示右边的索引
        QList<QString> validLetters;
        //遍历有效list和letters list，遇到相同的就添加到新的valid letters list
        for (auto value : m_data) {
            for (auto letter : letters) {
                if (value.text() == letter) {
                    validLetters.append(letter);
                    break;
                }
            }
        }
        m_model->setLetters(validLetters);
        bool bVisible = m_model->getModelCount() > 1;
        m_view->setVisible(bVisible);
    }
}

void KeyboardLayoutDialog::onSearch(const QString &text)
{
    if (text.length() == 0) {
        searchStatus = false;
        m_view->setModel(m_model);
    } else {
        searchStatus = true;
        QList<MetaData> datas = m_model->metaData();
        QList<MetaData>::iterator it = datas.begin();
        QList<MetaData> sdatas;
        for (; it != datas.end(); ++it) {
            if ((*it).text().contains(text, Qt::CaseInsensitive)) {
                if (!(*it).key().isEmpty()) {
                   sdatas.append(*it);
                }
            }
        }
        m_searchModel->clear();
        m_searchModel->setMetaData(sdatas);
        m_view->setModel(m_searchModel);
        m_buttonTuple->rightButton()->setEnabled(false);
    }
}

bool KeyboardLayoutDialog::eventFilter(QObject *watched, QEvent *event)
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
