// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "customitem.h"

#include <QPainter>
#include <QEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QFont>
#include <QEvent>
#include <QPen>

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::keyboard;

CustomItem::CustomItem(QWidget *parent)
    : SettingsItem(parent)
    , isAlert(false)
{
    setMouseTracking(true);
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(4, 0, 4, 0);
    layout->setSpacing(2);

    m_title = new QLabel();
    m_title->setText(tr("Shortcut"));
    m_title->setAlignment(Qt::AlignCenter);

    layout->addWidget(m_title);
    layout->setAlignment(m_title, Qt::AlignLeft);
    layout->addStretch();

    m_shortKey = new ShortcutKey;
    m_shortKey->setAccessibleName("SHORTCUT_KEY");
    layout->addWidget(m_shortKey);
    m_shortKey->setTextList(QStringList());

    m_shortcutEdit = new QLineEdit(this);
    m_shortcutEdit->setReadOnly(true);
    m_shortcutEdit->hide();
    m_shortcutEdit->installEventFilter(this);
    m_shortcutEdit->setAccessibleName("SHORTCUT_EDIT");
    if (QGuiApplication::platformName().startsWith("wayland", Qt::CaseInsensitive)) {
        connect(m_shortcutEdit, &QLineEdit::editingFinished, this, [ this ] {
            m_shortcutEdit->hide();
            m_shortKey->show();
        });
    }
    layout->addWidget(m_shortcutEdit);

    setLayout(layout);
    setFixedHeight(36);

}

void CustomItem::setTitle(const QString &title)
{
    m_title->setText(title);
}

void CustomItem::setShortcut(const QString &shortcut)
{
    m_accels = shortcut;

    QString list = shortcut;
    list = list.replace("<", "");
    list = list.replace(">", "-");
    list = list.replace("_L", "");
    list = list.replace("_R", "");
    list = list.replace("Control", "Ctrl");

    m_shortKey->setTextList(list.split("-"));
    m_shortcutEdit->hide();
    m_shortKey->show();
    Q_EMIT changeAlert();
}

QString CustomItem::text() const
{
    return m_accels;
}

void CustomItem::setAlert(bool isAlert)
{
    this->isAlert = isAlert;
    update();
}

void CustomItem::mouseReleaseEvent(QMouseEvent *e)
{
    if (!m_shortcutEdit->isVisible() && m_shortKey->rect().contains(m_shortKey->mapFromParent(e->pos()))) {
        m_shortKey->hide();
        m_shortcutEdit->clear();
        m_shortcutEdit->setFocus();
        m_shortcutEdit->show();
        m_shortcutEdit->setPlaceholderText(tr("Please enter a shortcut"));

        Q_EMIT requestUpdateKey();
    } else {
        m_shortKey->show();
        m_shortcutEdit->hide();
    }
}

void CustomItem::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    float Radius = 15.0f;
    if (isAlert) {
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(241, 57, 50, qRound(0.15 * 255)));
        p.drawRoundRect(rect(), int(Radius / rect().width() * 100), int(Radius / rect().height() * 100));
    }
    QWidget::paintEvent(event);
}

