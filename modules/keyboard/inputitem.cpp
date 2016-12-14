#include "inputitem.h"
#include <QHBoxLayout>
#include <QFileDialog>
#include <QSpacerItem>

namespace dcc {
namespace keyboard{
InputItem::InputItem(QFrame *parent)
    :SettingsItem(parent)
{
    setFixedHeight(36);

    QHBoxLayout* layout = new QHBoxLayout();
    m_choose = new QPushButton(tr(" ... "));
    m_title = new QLabel();
    m_title->setFixedWidth(60);
    m_input = new QLineEdit();

    layout->addWidget(m_title);
    layout->addSpacerItem(new QSpacerItem(10,height()));
    layout->addWidget(m_input);
    layout->addWidget(m_choose);
    m_choose->hide();
    setLayout(layout);

    connect(m_choose, SIGNAL(clicked()), this, SIGNAL(click()));
}

void InputItem::setTitle(const QString &title)
{
    m_title->setText(title);
}

void InputItem::setValue(const QString &value)
{
    m_input->setText(value);
}

void InputItem::setChooseVisible(bool visible)
{
    m_choose->setVisible(visible);
}

void InputItem::setRightText(const QString &text)
{
    if(m_choose)
    {
        m_choose->setText(text);
    }
}

QString InputItem::value() const
{
    return m_input->text();
}

void InputItem::setReadOnly(bool read)
{
    m_input->setReadOnly(read);
}

void InputItem::onClick()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Choose File"), tr("/usr/bin"));
    m_input->setText(file);
}

TitleButtonItem::TitleButtonItem(QFrame *parent)
    :SettingsItem(parent)
{
    QHBoxLayout* layout =new QHBoxLayout();
    m_title = new QLabel();
    m_button = new QPushButton();

    layout->addWidget(m_title);
    layout->addStretch();
    layout->addWidget(m_button);

    setLayout(layout);

    connect(m_button, SIGNAL(clicked()), this, SIGNAL(click()));
}

void TitleButtonItem::setTitle(const QString &title)
{
    m_title->setText(title);
}

void TitleButtonItem::setValue(const QString &value)
{
    m_button->setText(value);
}
}
}
