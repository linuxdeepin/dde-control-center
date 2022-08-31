// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "updatehistorybutton.h"

#include <QVBoxLayout>
#include <QDebug>
#include <QPainter>
#include <QPainterPath>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::update;
using namespace dcc::widgets;

UpdateHistoryButton::UpdateHistoryButton(QFrame *parent)
    : SettingsItem(parent)
    , m_messageLabel(new QLabel)
    , m_label(new QLabel)
    , m_mouseType(MOUSE_DEFAULT)
    , m_bIsRecentUpdate(false)
{
    this->setFixedSize(160, 60);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_label, 0, Qt::AlignHCenter);
    layout->addWidget(m_messageLabel, 0, Qt::AlignHCenter);
    layout->addStretch(0);

    m_picture.load(":/datetime/themes/light/icons/add_normal.png");
    m_label->setPixmap(m_picture);

    m_messageLabel->setStyleSheet("color: #319ab9;");
    m_messageLabel->setText(tr("Update History"));

    setLayout(layout);

    connect(this, &UpdateHistoryButton::notifyBtnClicked, this, &UpdateHistoryButton::onProcessMouseEvent);
    connect(this, &UpdateHistoryButton::notifyBtnRelease, this, &UpdateHistoryButton::onProcessMouseEvent);
}

void UpdateHistoryButton::setImage(const QString &fileName)
{
    m_picture.load(fileName);
}

void UpdateHistoryButton::setLabelText(QString text)
{
    m_messageLabel->setText(text);
}

void UpdateHistoryButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_mouseType = MOUSE_PRESS;
        Q_EMIT notifyBtnClicked();
    }
}

void UpdateHistoryButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_mouseType = MOUSE_RELEASE;
        m_bIsRecentUpdate = !m_bIsRecentUpdate;
        Q_EMIT notifyBtnRelease(m_bIsRecentUpdate);
    }
}

//rewrite the function, not allow to move parent widget
void UpdateHistoryButton::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
}

void UpdateHistoryButton::onProcessMouseEvent()
{
    switch (m_mouseType) {
    case MOUSE_PRESS:
        m_picture.load(":/datetime/themes/light/icons/add_normal.png");
        break;
    case MOUSE_RELEASE:

        break;
    case MOUSE_MOVE:

        break;
    default:
        break;
    }

    m_label->setPixmap(m_picture);
    update();
}

void UpdateHistoryButton::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    QPainterPath path;

    if (m_mouseType == MOUSE_PRESS) {
        path.addRoundedRect(rect(), 5, 5);
        painter.fillPath(path, QColor(255, 255, 255, static_cast<int>(0.2 * 255)));
        m_mouseType = MOUSE_DEFAULT;
    }
}
