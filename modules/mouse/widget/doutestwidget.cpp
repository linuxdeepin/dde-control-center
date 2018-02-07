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

#include "doutestwidget.h"

#include <QLabel>
#include <QMouseEvent>
#include <QDebug>
#include <QTimer>

DWIDGET_USE_NAMESPACE

using namespace dcc;
using namespace dcc::mouse;
using namespace dcc::widgets;


DouTestWidget::DouTestWidget(QWidget *parent)
    : SettingsItem(parent)
{
    m_mainlayout = new QVBoxLayout;
    QLabel *title = new QLabel(tr("Double-click Test"));
    m_mainlayout->setContentsMargins(20, 10, 10, 10);
    m_mainlayout->addWidget(title, 0, Qt::AlignLeft);
    m_testWidget = new DPictureSequenceView;
    m_testWidget->setFixedSize(128, 76);
    m_testWidget->setSingleShot(true);
    m_mainlayout->addWidget(m_testWidget, 0, Qt::AlignCenter);
    setLayout(m_mainlayout);

    //raise head
    for (int i = 1; i < 38; i++) {
        QString arg = QString::asprintf("%5.d", i).replace(' ', '0');
        QString path = QString(":/mouse/themes/common/double_test/raise_head/raise_head_%1.png").arg(arg);
        m_doubleTest.double_1 << path;
    }

    //bow head
    for (int i = 1; i < 18; i++) {
        QString arg = QString::asprintf("%5.d", i).replace(' ', '0');
        QString path = QString(":/mouse/themes/common/double_test/bow_head/bow_head_%1.png").arg(arg);
        m_doubleTest.double_2 << path;
    }
    //raise head ears
    for (int i = 1; i < 10; i++) {
        QString arg = QString::asprintf("%5.d", i).replace(' ', '0');
        QString path = QString(":/mouse/themes/common/double_test/raise_head_ears/raise_head_ears_%1.png").arg(arg);
        m_doubleTest.click_2 << path;
    }
    //bow head ears
    for (int i = 1; i < 10; i++) {
        QString arg = QString::asprintf("%5.d", i).replace(' ', '0');
        QString path = QString(":/mouse/themes/common/double_test/bow_head_ears/bow_head_ears_%1.png").arg(arg);
        m_doubleTest.click_1 << path;
    }

    m_state = State::BOW;
    m_testWidget->setPictureSequence(m_doubleTest.click_1);

    connect(m_testWidget, &DPictureSequenceView::playEnd, [ = ] {
        switch (m_state)
        {
        case BOW:
            m_testWidget->setPictureSequence(m_doubleTest.double_1);
            return;
        case RAISE:
            m_testWidget->setPictureSequence(m_doubleTest.double_2);
            return;
        }
    });

}

void DouTestWidget::mousePressEvent(QMouseEvent *)
{
    switch (m_state) {
    case BOW:
        m_testWidget->setPictureSequence(m_doubleTest.click_1);
        m_testWidget->play();
        return;
    case RAISE:
        m_testWidget->setPictureSequence(m_doubleTest.click_2);
        m_testWidget->play();
        return;
    }
}

void DouTestWidget::mouseDoubleClickEvent(QMouseEvent *)
{
    switch (m_state) {
    case BOW:
        m_testWidget->setPictureSequence(m_doubleTest.double_1);
        m_state = State::RAISE;
        m_testWidget->play();
        return;
    case RAISE:
        m_testWidget->setPictureSequence(m_doubleTest.double_2);
        m_state = State::BOW;
        m_testWidget->play();
        return;
    }
}


