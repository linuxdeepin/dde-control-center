#include "inputitem.h"
#include <QHBoxLayout>
#include <QFileDialog>
#include <QSpacerItem>
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
    layout->addSpacerItem(new QSpacerItem(30,height()));
    layout->addWidget(m_input);
    layout->addWidget(m_choose);
    m_choose->hide();
    setLayout(layout);

    connect(m_choose, SIGNAL(clicked()), this, SLOT(onClick()));
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

void InputItem::onClick()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Choose File"), tr("/usr/bin"));
    m_input->setText(file);
}

TitleValueItem::TitleValueItem(QFrame *parent)
    :SettingsItem(parent)
{
    QHBoxLayout* layout =new QHBoxLayout();
    m_title = new QLabel();
    m_value = new QLabel();

    layout->addWidget(m_title);
    layout->addWidget(m_value);

    setLayout(layout);
}

void TitleValueItem::setTitle(const QString &title)
{
    m_title->setText(title);
}

void TitleValueItem::setValue(const QString &value)
{
    m_value->setText(value);
}
