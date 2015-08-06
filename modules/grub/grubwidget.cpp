#include "grubwidget.h"
#include <libdui/dtextbutton.h>
#include <libdui/dseparatorhorizontal.h>
#include <libdui/dcolorpicker.h>
#include <QPixmap>
#include <QDebug>
#include <libdui/dbuttonlist.h>
#include "dbustheme.h"

GrubWidget::GrubWidget(QWidget *parent):
    QFrame(parent),
    m_layout(new QVBoxLayout()),
    m_header(new DHeaderLine()),
    m_arrowDefaultBoot(new DArrowLineExpand()),
    m_arrowBootDelay(new DArrowLineExpand()),
    m_arrowTextColor(new DArrowLineExpand()),
    m_arrowSelectedTextColor(new DArrowLineExpand()),
    m_themeDbus(new GrubThemeDbus(this)),
    m_grubDbus(new GrubDbus(this)),
    m_grubBackground(new GrubBackground(m_themeDbus))
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
    DTextButton *re_button = new DTextButton(tr("Reset"));
    connect(re_button, &DTextButton::clicked, m_grubDbus, &GrubDbus::Reset);
    m_header->setContent(re_button);

    m_arrowDefaultBoot->setTitle(tr("Default Boot"));
    DButtonList *boot_list = new DButtonList;
    boot_list->addButtons(m_grubDbus->GetSimpleEntryTitles());
    boot_list->setFixedSize(300, boot_list->count()*30);
    m_arrowDefaultBoot->setContent(boot_list);
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
    picker1->setCurrentColor(m_themeDbus->itemColor());
    connect(picker1, &DColorPicker::currentColorChanged, m_themeDbus, &GrubThemeDbus::setItemColor);
    connect(m_themeDbus, &GrubThemeDbus::itemColorChanged, picker1, &DColorPicker::setCurrentColor);
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
    picker2->setCurrentColor(m_themeDbus->selectedItemColor());
    connect(picker2, &DColorPicker::currentColorChanged, m_themeDbus, &GrubThemeDbus::setSelectedItemColor);
    connect(m_themeDbus, &GrubThemeDbus::selectedItemColorChanged, picker2, &DColorPicker::setCurrentColor);
    m_arrowSelectedTextColor->setTitle("Selected Text Color");
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
