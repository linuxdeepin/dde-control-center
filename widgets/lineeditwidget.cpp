#include "lineeditwidget.h"

#include <QVBoxLayout>

namespace dcc {
namespace widgets {

LineEditWidget::LineEditWidget(QFrame *parent)
    : SettingsItem(parent),

      m_title(new QLabel),
      m_edit(new QLineEdit)
{
    m_title->setFixedWidth(140);
    m_edit->setContextMenuPolicy(Qt::NoContextMenu);

    m_mainLayout = new QHBoxLayout;
    m_mainLayout->addSpacing(20);
    m_mainLayout->addWidget(m_title);
    m_mainLayout->addWidget(m_edit);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setMargin(0);

    setLayout(m_mainLayout);
    setObjectName("LineEditWidget");
    setFixedHeight(36);
}

void LineEditWidget::addRightWidget(QWidget *widget)
{
    m_mainLayout->addWidget(widget);
}

void LineEditWidget::setReadOnly(const bool state)
{
    m_edit->setReadOnly(state);
}

void LineEditWidget::setTitle(const QString &title)
{
    m_title->setText(title);

    setAccessibleName(title);
    m_edit->setAccessibleName(title);
}

void LineEditWidget::setText(const QString &text)
{
    m_edit->setText(text);
}

void LineEditWidget::setPlaceholderText(const QString &text)
{
    m_edit->setPlaceholderText(text);
}

void LineEditWidget::mousePressEvent(QMouseEvent *e)
{
    SettingsItem::mousePressEvent(e);

    m_edit->setFocus();
}

}
}
