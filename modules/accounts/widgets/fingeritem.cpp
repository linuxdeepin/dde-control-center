#include "fingeritem.h"

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::accounts;

FingerItem::FingerItem(QWidget *parent)
    : SettingsItem(parent)
    , m_layout(new QHBoxLayout)
    , m_editBtn(new DImageButton)
    , m_removeBtn(new DImageButton)
    , m_title(new QLabel)
{
    setFixedHeight(36);

    m_editBtn->setVisible(false);

    m_layout->setContentsMargins(20, 0, 10, 0);

    m_layout->addWidget(m_title);
    m_layout->addWidget(m_editBtn);
    m_layout->addStretch();
    m_layout->addWidget(m_removeBtn);

    setLayout(m_layout);
}

void FingerItem::setTitle(const QString &title)
{
    m_title->setText(title);
}

void FingerItem::setEditMode(const bool editmode)
{
    m_editMode = editmode;

    m_editBtn->setVisible(editmode);
    m_removeBtn->setVisible(editmode);
}
