//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "widgets/titlevalueitem.h"
#include "widgets/accessibleinterface.h"

#include <DPaletteHelper>

#include <QHBoxLayout>
#include <QEvent>
#include <QPushButton>
#include <QLabel>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;
SET_FORM_ACCESSIBLE(TitleValueItem,"TitleValueItem")
ResizeEventFilter::ResizeEventFilter(QObject *parent) : QObject(parent) {}

bool ResizeEventFilter::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::Resize) {
        QLabel* l = qobject_cast<QLabel*>(watched);
        if (l) {
            //as we are not interested in the y offset of the rendered text, height of the rectangle doesn't matter.
            QRect r = l->fontMetrics().boundingRect(QRect(0, 0, l->width(), 100), Qt::TextWordWrap, l->text());
            l->setMinimumHeight(r.height());
            return true;
        }
    }
    return QObject::eventFilter(watched, event);
}

TitleValueItem::TitleValueItem(QFrame *parent)
    : SettingsItem(parent),
      m_title(new QLabel),
      m_value(new ItemTitleTipsLabel(""))
{
    QHBoxLayout* layout = new QHBoxLayout(this);

    m_value->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_value->setWordWrap(true);

    layout->setContentsMargins(10, 10, 10, 10);

    layout->addWidget(m_title);
    layout->addWidget(m_value);

    m_value->installEventFilter(new ResizeEventFilter(this));

    setLayout(layout);
}

TitleValueItem::~TitleValueItem()
{
    delete m_title;
    delete m_value;
}

void TitleValueItem::setTitle(const QString &title)
{
    m_title->setText(title);
}

void TitleValueItem::setValue(const QString &value)
{
    m_value->setText(value);
}

void TitleValueItem::setWordWrap(const bool enable)
{
    m_value->setWordWrap(enable);
}

QString TitleValueItem::value() const
{
    return m_value->text();
}

void TitleValueItem::setValueAligment(const Qt::Alignment aligment)
{
    m_value->setAlignment(aligment);
}

void TitleValueItem::setValueBackground(bool showBackground)
{
    if (showBackground) {
        m_value->addBackground();
        layout()->setContentsMargins(5, 5, 5, 5);
    } else {
        m_value->removeBackground();
        layout()->setContentsMargins(10, 10, 10, 10);
    }
}

void TitleValueItem::resizeEvent(QResizeEvent *event)
{
    if (m_value->hasBackground()) {
        // 如果存在背景色，则让背景色的部分占满右侧的部分
        m_value->setFixedWidth(static_cast<int>(width() * 0.62));
    }
    SettingsItem::resizeEvent(event);
}


TitleAuthorizedItem::TitleAuthorizedItem(QFrame *parent)
    : SettingsItem(parent)
    , m_title(new QLabel)
    , m_value(new DTipLabel(""))
    , m_pActivatorBtn(new QPushButton)
{
    QHBoxLayout* layout = new QHBoxLayout;

    m_value->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_value->setWordWrap(true);

    layout->setContentsMargins(10, 10, 10, 10);

    layout->addWidget(m_title);
    layout->addWidget(m_value);
    layout->addWidget(m_pActivatorBtn);
    m_pActivatorBtn->setFocusPolicy(Qt::NoFocus);

    m_value->installEventFilter(new ResizeEventFilter(this));

    setLayout(layout);

    //传递button的点击信号
    connect(m_pActivatorBtn, SIGNAL(clicked()), this, SIGNAL(clicked()));
}

void TitleAuthorizedItem::setTitle(const QString& title)
{
    m_title->setText(title);
}

void TitleAuthorizedItem::setValue(const QString& value)
{
    m_value->setText(value);
}

void TitleAuthorizedItem::setWordWrap(bool enable)
{
    m_value->setWordWrap(enable);
}

void TitleAuthorizedItem::setButtonText(const QString &str)
{
    m_pActivatorBtn->setText(str);
}

void TitleAuthorizedItem::setVisable(bool value)
{
    m_pActivatorBtn->setVisible(value);
}

void TitleAuthorizedItem::setValueForegroundRole(const QColor &color)
{
    auto pa = DPaletteHelper::instance()->palette(m_value);
    pa.setBrush(DPalette::TextTips, color);
    DPaletteHelper::instance()->setPalette(m_value, pa);
}

/**
 * @brief ItemTitleTipsLabel::ItemTitleTipsLabel
 * @param text
 * @param parent
 */
ItemTitleTipsLabel::ItemTitleTipsLabel(const QString &text, QWidget *parent)
    : DTipLabel (text, parent)
    , m_hasBackground(false)
{
}

ItemTitleTipsLabel::~ItemTitleTipsLabel()
{
}

void ItemTitleTipsLabel::addBackground()
{
    m_hasBackground = true;
    update();
}

void ItemTitleTipsLabel::removeBackground()
{
    m_hasBackground = false;
    update();
}

bool ItemTitleTipsLabel::hasBackground() const
{
    return m_hasBackground;
}

void ItemTitleTipsLabel::paintEvent(QPaintEvent *event)
{
    if (m_hasBackground) {
        const DPalette &dp = DPaletteHelper::instance()->palette(this);
        QPainter p(this);
        p.setPen(Qt::NoPen);
        p.setBrush(dp.brush(DPalette::FrameShadowBorder));
        p.drawRoundedRect(rect(), 8, 8);
    }
    DTipLabel::paintEvent(event);
}
