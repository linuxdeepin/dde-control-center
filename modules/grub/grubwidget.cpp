#include "grubwidget.h"
#include <libdui/dtextbutton.h>
#include <libdui/dseparatorhorizontal.h>
#include <QDebug>

GrubWidget::GrubWidget(QWidget *parent):
    QFrame(parent),
    m_layout(new QVBoxLayout()),
    m_header(new DHeaderLine())
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_header->setTitle(tr("Grub"));
    m_header->setContent(new DTextButton(tr("Reset")));

    m_layout->addWidget(m_header);
    m_layout->addWidget(new DSeparatorHorizontal());
    m_layout->addStretch();

    setLayout(m_layout);
}

GrubWidget::~GrubWidget()
{

}
