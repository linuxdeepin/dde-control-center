#include "grubwidget.h"
#include <libdui/dtextbutton.h>
#include <libdui/dseparatorhorizontal.h>
#include <libdui/dcolorpicker.h>
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
    m_layout->setMargin(0);

    m_header->setTitle(tr("Boot Menu"));
    m_header->setContent(new DTextButton(tr("Reset")));

    m_arrowDefaultBoot->setTitle(tr("Default Boot"));
    m_arrowDefaultBoot->setContent(new QWidget);
    m_arrowDefaultBoot->setExpand(false);

    m_arrowBootDelay->setTitle(tr("Boot delay"));
    m_arrowBootDelay->setContent(new QWidget);
    m_arrowBootDelay->setExpand(false);

    DColorPicker *picker1 = new DColorPicker(12, 19, 12, 2, 10, this);
    picker1->addColorGradient(QPoint(1, 1), QPoint(12, 1), Qt::black, Qt::white);
    picker1->addColorGradient(QPoint(1, 2), QPoint(6, 7), "#000011", "#00FFFF");
    picker1->addColorGradient(QPoint(1, 8), QPoint(6, 13), "#330000", "#33FFFF");
    picker1->addColorGradient(QPoint(1, 14), QPoint(6, 19), "#660000", "#66FFFF");
    picker1->addColorGradient(QPoint(7, 2), QPoint(12, 7), "#990000", "#99FFFF");
    picker1->addColorGradient(QPoint(7, 8), QPoint(12, 13), "#CC0000", "#CCFFFF");
    picker1->addColorGradient(QPoint(7, 14), QPoint(12, 19), "#FF0000", "#FFFFFF");
    m_arrowTextColor->setTitle(tr("Text Color"));
    m_arrowTextColor->setContent(picker1);
    m_arrowTextColor->setExpand(false);

    DColorPicker *picker2 = new DColorPicker(12, 19, 12, 2, 10, this);
    picker2->addColorGradient(QPoint(1, 1), QPoint(12, 1), Qt::black, Qt::white);
    picker2->addColorGradient(QPoint(1, 2), QPoint(6, 7), "#000011", "#00FFFF");
    picker2->addColorGradient(QPoint(1, 8), QPoint(6, 13), "#330000", "#33FFFF");
    picker2->addColorGradient(QPoint(1, 14), QPoint(6, 19), "#660000", "#66FFFF");
    picker2->addColorGradient(QPoint(7, 2), QPoint(12, 7), "#990000", "#99FFFF");
    picker2->addColorGradient(QPoint(7, 8), QPoint(12, 13), "#CC0000", "#CCFFFF");
    picker2->addColorGradient(QPoint(7, 14), QPoint(12, 19), "#FF0000", "#FFFFFF");
    m_arrowSelectedTextColor->setTitle("Selected Text Color");
    QColorDialog *dialog = new QColorDialog;
    dialog->setOptions(QColorDialog::NoButtons);
    m_arrowSelectedTextColor->setContent(picker2);
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
