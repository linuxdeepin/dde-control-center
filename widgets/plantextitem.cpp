#include "plantextitem.h"

#include <QVBoxLayout>

namespace dcc {

namespace widgets {

PlainTextItem::PlainTextItem(QWidget *parent)
    : SettingsItem(parent),

      m_plainEdit(new QPlainTextEdit)
{
    m_plainEdit->setBackgroundVisible(false);
    m_plainEdit->setFrameStyle(QFrame::NoFrame);

    QVBoxLayout *mainLayout = new QVBoxLayout;

    mainLayout->addWidget(m_plainEdit);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    setLayout(mainLayout);
}

QPlainTextEdit *PlainTextItem::plainEdit() const
{
    return m_plainEdit;
}

}

}
