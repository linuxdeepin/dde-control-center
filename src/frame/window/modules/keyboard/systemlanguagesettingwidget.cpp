/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <andywang_cm@deepin.com>
 *
 * Maintainer: andywang <andywang_cm@deepin.com>
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

#include "systemlanguagesettingwidget.h"
#include "modules/keyboard/keyboardmodel.h"
#include "modules/keyboard/langwidget.h"
#include "modules/keyboard/indexdelegate.h"
#include "modules/keyboard/indexview.h"
#include "modules/keyboard/indexmodel.h"
#include "widgets/searchinput.h"
#include "widgets/translucentframe.h"

#include <QVBoxLayout>
#include <QEvent>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::keyboard;
using namespace dcc;
using namespace dcc::keyboard;

SystemLanguageSettingWidget::SystemLanguageSettingWidget(KeyboardModel *model, QWidget *parent)
    : ContentWidget(parent)
    , m_keyboardModel(model)
{
    m_contentWidget = new TranslucentFrame();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);

    m_searchModel = new IndexModel();

    m_model = new IndexModel();
    m_view = new IndexView();
    m_delegate = new IndexDelegate();

    m_search = new SearchInput();

    m_title = new QLabel(tr("Add System Language"));
    m_title->setAlignment(Qt::AlignCenter | Qt::AlignHCenter | Qt::AlignVCenter);
    m_contentTopLayout->addSpacing(10);
    m_contentTopLayout->addWidget(m_title);
    m_contentTopLayout->addSpacing(10);
    m_contentTopLayout->addWidget(m_search);
    m_contentTopLayout->addSpacing(10);

    layout->addWidget(m_view);
    m_contentWidget->setLayout(layout);

    // 矩形圆角
    m_clipEffectWidget = new DGraphicsClipEffect(m_contentWidget);
    m_contentWidget->installEventFilter(this);
    m_contentWidget->setGraphicsEffect(m_clipEffectWidget);

    setContent(m_contentWidget);
    m_contentWidget->setAttribute(Qt::WA_TranslucentBackground);

    connect(m_search, &SearchInput::textChanged, this, &SystemLanguageSettingWidget::onSearch);
    connect(m_view, &IndexView::clicked, this, &SystemLanguageSettingWidget::click);
    connect(m_view, &IndexView::clicked, this, &SystemLanguageSettingWidget::back);
    connect(m_keyboardModel, &KeyboardModel::langChanged, this, &SystemLanguageSettingWidget::setModelData);

    setModelData(m_keyboardModel->langLists());
}

void SystemLanguageSettingWidget::onSearch(const QString &text)
{
    if (text.length() == 0) {
        m_view->setModel(m_model);
    } else {
        QList<MetaData> datas = m_model->metaData();
        QList<MetaData>::iterator it = datas.begin();
        QList<MetaData> sdatas;
        for (; it != datas.end(); ++it) {
            if ((*it).text().contains(text, Qt::CaseInsensitive)) {
                sdatas.append(*it);
            }
        }
        m_searchModel->setMetaData(sdatas);
        m_view->setModel(m_searchModel);
    }
}

void SystemLanguageSettingWidget::setModelData(const QList<MetaData> &datas)
{
    m_datas = datas;
    QStringList removeLangList = m_keyboardModel->localLang();
    for (QList<MetaData>::iterator iter = m_datas.begin(); iter != m_datas.end();) {
        if (removeLangList.contains(iter->text())) {
            m_datas.erase(iter);
        } else {
            ++iter;
        }
    }

    m_model->setMetaData(m_datas);
    m_view->setModel(m_model);
    m_view->setItemDelegate(m_delegate);
}

bool SystemLanguageSettingWidget::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched)

    if (event->type() != QEvent::Move && event->type() != QEvent::Resize)
        return false;

    QRect rect = m_contentWidget->rect();

    rect.moveTopLeft(-m_contentWidget->pos());
    rect.setHeight(m_contentWidget->window()->height() - m_contentWidget->mapTo(window(), rect.topLeft()).y());

    QPainterPath path;
    path.addRoundedRect(rect, 5, 5);
    m_clipEffectWidget->setClipPath(path);

    return false;
}
