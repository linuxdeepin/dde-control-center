//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "datewidget.h"

#include <QIntValidator>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QLineEdit>
#include <QLabel>

DWIDGET_USE_NAMESPACE

DateWidget::DateWidget(Type type, int minimum, int maximum, QWidget *parent)
    : SettingsItem (parent)
    , m_type(type)
    , m_minimum(minimum)
    , m_maximum(maximum)
    , m_lineEdit(new QLineEdit(this))
    , m_label(new QLabel(this))
    , m_addBtn(new DIconButton(this))
    , m_reducedBtn(new DIconButton(this))
{
    setFixedHeight(36);

    m_addBtn->setIcon(DStyle::StandardPixmap::SP_IncreaseElement);
    m_reducedBtn->setIcon(DStyle::StandardPixmap::SP_DecreaseElement);

    m_lineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    m_lineEdit->setObjectName("DCC-Datetime-QLineEdit");
    QPalette palette = m_lineEdit->palette();
    palette.setColor(QPalette::Button, Qt::transparent);
    m_lineEdit->setPalette(palette);
    DStyle::setFocusRectVisible(m_lineEdit, false);
    m_addBtn->setObjectName("DCC-Datetime-Datewidget-Add");
    m_reducedBtn->setObjectName("DCC-Datetime-Datewidget-Reduce");

    m_label->setParent(m_lineEdit);
    m_label->move(0, 0);
    m_addBtn->setParent(m_lineEdit);
    m_reducedBtn->setParent(m_lineEdit);

    if (m_type == Year) {
        m_addBtn->setAccessibleName("yearadd");
        m_reducedBtn->setAccessibleName("yearreduced");
        m_label->setText(tr("Year"));
        m_lineEdit->setAccessibleName(tr("Year"));
        m_lineEdit->setMaxLength(4);
    } else if (m_type == Month) {
        m_addBtn->setAccessibleName("monthadd");
        m_reducedBtn->setAccessibleName("monthreduced");
        m_label->setText(tr("Month"));
        m_lineEdit->setAccessibleName(tr("Month"));
        m_lineEdit->setMaxLength(2);
    } else {
        m_addBtn->setAccessibleName("dayadd");
        m_reducedBtn->setAccessibleName("dayreduced");
        m_label->setText(tr("Day"));
        m_lineEdit->setAccessibleName(tr("Day"));
        m_lineEdit->setMaxLength(2);
    }

    m_lineEdit->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    setRange(minimum, maximum);
    m_lineEdit->installEventFilter(this);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    QHBoxLayout *lLayout = new QHBoxLayout;
    lLayout->setMargin(0);
    lLayout->setSpacing(0);
    lLayout->addWidget(m_reducedBtn);
    lLayout->addStretch();
    lLayout->addWidget(m_lineEdit);

    QHBoxLayout *rLayout = new QHBoxLayout;
    rLayout->setMargin(0);
    rLayout->setSpacing(0);
    rLayout->addWidget(m_label);
    rLayout->addStretch();
    rLayout->addWidget(m_addBtn);

    layout->addLayout(lLayout);
    layout->addSpacing(5);
    layout->addLayout(rLayout);
    setLayout(layout);

    connect(m_addBtn, &DIconButton::clicked, this, &DateWidget::slotAdd);
    connect(m_reducedBtn, &DIconButton::clicked, this, &DateWidget::slotReduced);

    connect(m_lineEdit, &QLineEdit::editingFinished, [this] {
        fixup();
        Q_EMIT editingFinished();
    });
    connect(m_lineEdit, &QLineEdit::textChanged, this, [this] {
        Q_EMIT chenged();
    });
}

void DateWidget::setValue(const int &value)
{
    m_lineEdit->blockSignals(true);
    m_lineEdit->setText(QString::number(value));
    m_lineEdit->blockSignals(false);
}

int DateWidget::value() const
{
    return m_lineEdit->text().toInt();
}

void DateWidget::slotAdd()
{
    int value = m_lineEdit->text().toInt() + 1;

    if (value < m_minimum) {
        value = m_maximum;
    } else if (value > m_maximum) {
        value = m_minimum;
    }

    m_lineEdit->setText(QString::number(value));

    Q_EMIT notifyClickedState(true);
}

void DateWidget::slotReduced()
{
    int value = m_lineEdit->text().toInt() - 1;

    if (value < m_minimum) {
        value = m_maximum;
    } else if (value > m_maximum) {
        value = m_minimum;
    }

    m_lineEdit->setText(QString::number(value));

    Q_EMIT notifyClickedState(false);
}

void DateWidget::fixup()
{
    int value = m_lineEdit->text().toInt();
    value = qMin(m_maximum, qMax(m_minimum, value));
    m_lineEdit->setText(QString::number(value));
}

int DateWidget::maximum() const
{
    return m_maximum;
}

void DateWidget::setRange(int minimum, int maximum)
{
    m_minimum = minimum;
    m_maximum = maximum;

    QIntValidator *validator = new QIntValidator(m_minimum, m_maximum, this);
    m_lineEdit->setValidator(validator);

    fixup();
}

const QString DateWidget::getCurrentText() const
{
    return m_lineEdit->text();
}

void DateWidget::setCurrentText(QString text)
{
    m_lineEdit->setText(text);
}

bool DateWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_lineEdit && event->type() == QEvent::FocusOut) {
        fixup();
        Q_EMIT editingFinished();
    }

    return false;
}

void DateWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_lineEdit->setFocus();
    }
}

int DateWidget::minimum() const
{
    return m_minimum;
}
