#include "grubwidget.h"
#include <libdui/dtextbutton.h>
#include <libdui/dseparatorhorizontal.h>
#include <QPixmap>
#include <QDebug>
#include <QColorDialog>

GrubWidget::GrubWidget(QWidget *parent):
    QFrame(parent),
    m_layout(new QVBoxLayout()),
    m_header(new DHeaderLine()),
    m_grubBackground(new GrubBackground()),
    m_arrowDefaultBoot(new DArrowLineExpand()),
    m_arrowBootDelay(new DArrowLineExpand()),
    m_arrowTextColor(new DArrowLineExpand()),
    m_arrowSelectedTextColor(new DArrowLineExpand())
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    init();
}

GrubWidget::~GrubWidget()
{

}

void GrubWidget::init()
{
    m_header->setTitle(tr("Boot Menu"));
    m_header->setContent(new DTextButton(tr("Reset")));

    m_arrowDefaultBoot->setTitle(tr("Default Boot"));
    m_arrowDefaultBoot->setContent(new QWidget);
    m_arrowDefaultBoot->setExpand(false);

    m_arrowBootDelay->setTitle(tr("Boot delay"));
    m_arrowBootDelay->setContent(new QWidget);
    m_arrowBootDelay->setExpand(false);

    m_arrowTextColor->setTitle(tr("Text Color"));
    m_arrowTextColor->setContent(new QColorDialog);
    m_arrowTextColor->setExpand(false);

    m_arrowSelectedTextColor->setTitle("Selected Text Color");
    QColorDialog *dialog = new QColorDialog;
    dialog->setOptions(QColorDialog::NoButtons);
    m_arrowSelectedTextColor->setContent(dialog);
    m_arrowSelectedTextColor->setExpand(false);

    m_layout->setSpacing(0);
    m_layout->addWidget(m_header);
    m_layout->addWidget(new DSeparatorHorizontal());
    m_layout->addWidget(m_grubBackground);
    m_layout->addWidget(new DSeparatorHorizontal());
    m_layout->addWidget(m_arrowDefaultBoot);
    m_layout->addWidget(m_arrowBootDelay);
    m_layout->addWidget(m_arrowTextColor);
    m_layout->addWidget(m_arrowSelectedTextColor);
    m_layout->addStretch(1);

    setLayout(m_layout);
}
