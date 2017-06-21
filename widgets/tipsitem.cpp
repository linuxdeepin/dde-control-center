#include "tipsitem.h"
#include "labels/normallabel.h"

#include <QVBoxLayout>

namespace dcc {

namespace widgets {

TipsItem::TipsItem(QWidget *parent)
    : SettingsItem(parent),

      m_tipsLabel(new NormalLabel)
{
    m_tipsLabel->setAlignment(Qt::AlignCenter);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addWidget(m_tipsLabel);

    setLayout(centralLayout);
}

void TipsItem::setText(const QString &txt)
{
    m_tipsLabel->setText(txt);
}

}

}

