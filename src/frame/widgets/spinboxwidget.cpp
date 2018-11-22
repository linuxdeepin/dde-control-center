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

#include "spinboxwidget.h"

#include <dspinbox.h>

#include <QLabel>
#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE

namespace dcc {

namespace widgets {

SpinBoxWidget::SpinBoxWidget(QWidget *parent)
    : SettingsItem(parent),
      m_title(new QLabel),
      m_spinBox(new DSpinBox)
{
    m_spinBox->setContextMenuPolicy(Qt::NoContextMenu);
    m_spinBox->setStyleSheet("DSpinBox {"
                             "background-color: transparent;"
                             "}"
                             ""
                             "QWidget {"
                             "border: none;"
                             "background-color: transparent;"
                             "}"
                             ""
                             "QAbstractSpinBox::up-button {"
                             "border: none;"
                             "background: transparent;"
                             "}"
                             ""
                             "QAbstractSpinBox::up-arrow {"
                             "border: none;"
                             "background: transparent;"
                             "}"
                             ""
                             "QAbstractSpinBox::down-button {"
                             "border: none;"
                             "background: transparent;"
                             "}"
                             ""
                             "QAbstractSpinBox::down-arrow {"
                             "border: none;"
                             "background: transparent;"
                             "}");

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(20, 7, 10, 7);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(m_title);
    mainLayout->addWidget(m_spinBox);

    setLayout(mainLayout);
}

void SpinBoxWidget::setTitle(const QString &title)
{
    m_title->setText(title);
}

}

}
