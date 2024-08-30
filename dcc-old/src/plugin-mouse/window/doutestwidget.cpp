//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "doutestwidget.h"

#include <QLabel>
#include <QMouseEvent>
#include <QDebug>
#include <QTimer>
#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE

using namespace DCC_NAMESPACE;

DouTestWidget::DouTestWidget(QWidget *parent)
    : SettingsItem(parent)
    , m_mainlayout(nullptr)
    , m_testWidget(nullptr)
    , m_state(State::BOW)
{
    m_mainlayout = new QVBoxLayout;
    QLabel *title = new QLabel(tr("Double-click Test"));
//    m_mainlayout->setContentsMargins(20, 10, 10, 10);
    m_mainlayout->addWidget(title, 0, Qt::AlignLeft);
    m_testWidget = new DPictureSequenceView;
    m_testWidget->setAccessibleName("DouTestWidget_testWidget");
    m_testWidget->setFixedSize(128, 76);
    m_testWidget->setSingleShot(true);
    m_mainlayout->addWidget(m_testWidget, 0, Qt::AlignCenter);
    setLayout(m_mainlayout);

    //raise head
    for (int i = 1; i < 38; i++) {
        QString arg = QString::asprintf("%5.d", i).replace(' ', '0');
        QString path = QString(":/mouse/double_test/raise_head/raise_head_%1.png").arg(arg);
        m_doubleTest.double_1 << path;
    }

    //bow head
    for (int i = 1; i < 18; i++) {
        QString arg = QString::asprintf("%5.d", i).replace(' ', '0');
        QString path = QString(":/mouse/double_test/bow_head/bow_head_%1.png").arg(arg);
        m_doubleTest.double_2 << path;
    }
    //raise head ears
    for (int i = 1; i < 10; i++) {
        QString arg = QString::asprintf("%5.d", i).replace(' ', '0');
        QString path = QString(":/mouse/double_test/raise_head_ears/raise_head_ears_%1.png").arg(arg);
        m_doubleTest.click_2 << path;
    }
    //bow head ears
    for (int i = 1; i < 10; i++) {
        QString arg = QString::asprintf("%5.d", i).replace(' ', '0');
        QString path = QString(":/mouse/double_test/bow_head_ears/bow_head_ears_%1.png").arg(arg);
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


