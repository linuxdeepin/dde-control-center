/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#include "keyboardlayoutwidget.h"
#include "window/utils.h"
#include "widgets/settingsgroup.h"
#include "widgets/settingsitem.h"
#include "widgets/translucentframe.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QEvent>
#include <QLocale>

using namespace dcc;

namespace dcc {
namespace keyboard {

KeyboardLayoutWidget::KeyboardLayoutWidget(QWidget *parent)
    : ContentWidget(parent)
    , textLength(0)
    , searchStatus(false)
    , m_buttonTuple(new ButtonTuple(ButtonTuple::Save))
{
    //~ contents_path /keyboard/Keyboard Layout
    setTitle(tr("Add Keyboard Layout"));

    m_mainWidget = new TranslucentFrame();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setMargin(0);
    hlayout->setSpacing(0);

    m_searchModel = new IndexModel();

    m_model = new IndexModel();
    m_view = new IndexView();

    m_view->setAccessibleName("List_keyboardmenulist");
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    //m_view->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    m_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setSelectionMode(QAbstractItemView::NoSelection);

    QPushButton *cancel = m_buttonTuple->leftButton();
    cancel->setText(tr("Cancel"));
    QPushButton *ok = m_buttonTuple->rightButton();
    ok->setText(tr("Add"));
    ok->setEnabled(false);

    m_indexframe = nullptr;

    hlayout->addWidget(m_view);

    QLocale locale;
    if (locale.language() == QLocale::Chinese) {
        m_indexframe = new IndexFrame();
        hlayout->addWidget(m_indexframe);
        connect(m_indexframe, SIGNAL(click(QString)), m_view, SLOT(onClick(QString)));
    }

    m_search = new SearchInput();
    m_contentTopLayout->addSpacing(10);
    m_contentTopLayout->addWidget(m_search);
    m_contentTopLayout->addSpacing(10);

    QVBoxLayout *mainVLayout = new QVBoxLayout();
    mainVLayout->addLayout(hlayout);
    mainVLayout->addSpacing(10);
    mainVLayout->addWidget(m_buttonTuple, 0, Qt::AlignBottom);

    m_mainWidget->setLayout(mainVLayout);
    m_mainWidget->setContentsMargins(DCC_NAMESPACE::ListViweItemMargin);

    m_clipEffectWidget = new DGraphicsClipEffect(m_mainWidget);
    m_mainWidget->installEventFilter(this);
    m_mainWidget->setGraphicsEffect(m_clipEffectWidget);

    setContent(m_mainWidget);

    m_mainWidget->setAttribute(Qt::WA_TranslucentBackground);

    connect(m_search, SIGNAL(textChanged(QString)), this, SLOT(onSearch(QString)));
    connect(cancel, &QPushButton::clicked, this, &KeyboardLayoutWidget::back);
    connect(ok, &QPushButton::clicked, this, &KeyboardLayoutWidget::onAddKBLayout);
    connect(m_view, &IndexView::clicked, this, &KeyboardLayoutWidget::onKBLayoutSelect);
}

KeyboardLayoutWidget::~KeyboardLayoutWidget()
{
    m_searchModel->deleteLater();
    m_model->deleteLater();
}

void KeyboardLayoutWidget::onAddKBLayout()
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
    Q_EMIT back();
}

void KeyboardLayoutWidget::onKBLayoutSelect(const QModelIndex &index)
{
    if (searchStatus) {
        setDataModel(m_searchModel, m_selectSearchIndex, index);
    } else{
        setDataModel(m_model, m_selectIndex, index);
    }
}

void KeyboardLayoutWidget::setDataModel(IndexModel *model, QModelIndex &selectedIndex, const QModelIndex &index) {

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

void KeyboardLayoutWidget::setMetaData(const QList<MetaData> &datas)
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

void KeyboardLayoutWidget::setLetters(QList<QString> letters)
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
        m_indexframe->setLetters(validLetters);
        bool bVisible = m_model->getModelCount() > 1;
        m_view->setVisible(bVisible);
        m_indexframe->setVisible(bVisible);
    }
}

void KeyboardLayoutWidget::onSearch(const QString &text)
{
    if (text.length() == 0) {
        searchStatus = false;
        m_view->setModel(m_model);
        if (m_indexframe)
            m_indexframe->show();
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
        if (m_indexframe)
            m_indexframe->hide();
        m_buttonTuple->rightButton()->setEnabled(false);
    }
}

void KeyboardLayoutWidget::onItemClicked(const QModelIndex &index)
{
    QVariant var = index.data(IndexModel::KBLayoutRole);
    MetaData md = var.value<MetaData>();

    if (m_model->letters().contains(md.text())) {
        m_buttonTuple->rightButton()->setEnabled(true);
        return;
    }

    Q_EMIT layoutSelected(md.text());
    Q_EMIT back();
}

bool KeyboardLayoutWidget::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched)

    if (event->type() != QEvent::Move && event->type() != QEvent::Resize)
        return false;

    QRect rect = m_mainWidget->rect();

    rect.moveTopLeft(-m_mainWidget->pos());
    rect.setHeight(m_mainWidget->window()->height() - m_mainWidget->mapTo(window(), rect.topLeft()).y());

    QPainterPath path;
    path.addRoundedRect(rect, 5, 5);
    m_clipEffectWidget->setClipPath(path);

    return false;
}

}
}
