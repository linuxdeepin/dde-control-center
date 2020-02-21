/*
 * Copyright (C) 2015 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             Hualet <mr.asianwang@gmail.com>
 *             kirigaya <kirigaya@mkacg.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             Hualet <mr.asianwang@gmail.com>
 *             kirigaya <kirigaya@mkacg.com>
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

#include "widgets/loadingnextpagewidget.h"

#include <QBoxLayout>

namespace dcc {
namespace widgets {

LoadingNextPageWidget::LoadingNextPageWidget(QFrame *parent)
    : NextPageWidget(parent),

      m_loadingIndicator(new DPictureSequenceView(this))
{
    m_loadingIndicator->setPictureSequence(":/frame/themes/dark/icons/dark_loading/loading_%1.png", QPair<int, int>(0, 90), 3);
    m_loadingIndicator->setFixedSize(30, 30);
    m_loadingIndicator->stop();
    m_loadingIndicator->setVisible(false);

    QHBoxLayout *mainLayout = static_cast<QHBoxLayout *>(layout());
    mainLayout->insertWidget(4, m_loadingIndicator);
    mainLayout->setAlignment(m_loadingIndicator, Qt::AlignVCenter);
}

void LoadingNextPageWidget::setLoading(const bool loading)
{
    m_loadingIndicator->setVisible(loading);

    if (loading)
        m_loadingIndicator->play();
    else
        m_loadingIndicator->stop();
}

}

}
