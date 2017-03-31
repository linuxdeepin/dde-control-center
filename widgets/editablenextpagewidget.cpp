#include "editablenextpagewidget.h"

#include <QHBoxLayout>
#include <QMouseEvent>
#include <QDebug>

namespace dcc {

namespace widgets {

EditableNextPageWidget::EditableNextPageWidget(QWidget *parent)
    : SettingsItem(parent),

      m_name(new QLabel),
      m_selectIcon(new QLabel),
      m_edit(new QLineEdit),
      m_editButton(new DImageButton),
      m_nextButton(new DImageButton)
{
    QHBoxLayout *centralLayout = new QHBoxLayout;

    m_editButton->setObjectName("EditButton");
    m_nextButton->setObjectName("NextButton");

    m_edit->setVisible(false);

    centralLayout->addWidget(m_name);
    centralLayout->addWidget(m_edit);
    centralLayout->addSpacing(5);
    centralLayout->addWidget(m_editButton);
    centralLayout->addStretch();
    centralLayout->addWidget(m_selectIcon);
    centralLayout->addSpacing(5);
    centralLayout->addWidget(m_nextButton);
    centralLayout->setSpacing(0);
    centralLayout->setContentsMargins(15, 8, 15, 8);

    connect(m_nextButton, &DImageButton::clicked, this, &EditableNextPageWidget::acceptNextPage);
    connect(m_editButton, &DImageButton::clicked, this, &EditableNextPageWidget::toEditMode, Qt::QueuedConnection);
    connect(m_edit, &QLineEdit::editingFinished, this, &EditableNextPageWidget::toEchoMode, Qt::QueuedConnection);

    setLayout(centralLayout);
}

void EditableNextPageWidget::setTitle(const QString &title)
{
    m_name->setText(title);
}

void EditableNextPageWidget::setIcon(const QPixmap &icon)
{
    m_selectIcon->setPixmap(icon);
}

void EditableNextPageWidget::mousePressEvent(QMouseEvent *e)
{
    e->accept();

    SettingsItem::mousePressEvent(e);

    // ensure is echo mode
    if (m_name->isVisible())
        emit selected();
}

void EditableNextPageWidget::keyPressEvent(QKeyEvent *e)
{
    e->accept();

    SettingsItem::keyPressEvent(e);
}

void EditableNextPageWidget::toEditMode()
{
    m_edit->setText(m_name->text());
    m_edit->setVisible(true);
    m_name->setVisible(false);
    m_editButton->setVisible(false);

    m_edit->setFocus();
}

void EditableNextPageWidget::toEchoMode()
{
    m_edit->clearFocus();

    const QString oldText = m_name->text();

    // TODO: text update should be trigger from model
//    m_name->setText(m_edit->text());
    m_name->setVisible(true);
    m_editButton->setVisible(true);
    m_edit->setVisible(false);

    emit textChanged(oldText, m_edit->text());
}

}

}

