#include <QPixmap>
#include <QDebug>
#include <QTimer>

#include <libdui/dtextbutton.h>
#include <libdui/dseparatorhorizontal.h>
#include <libdui/dcolorpicker.h>
#include <libdui/dsegmentedcontrol.h>
#include <libdui/dbuttonlist.h>

#include "grubwidget.h"
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
    m_grubBackground(new GrubBackground(m_themeDbus)),
    m_bootEntryList(new DButtonList()),
    m_selectTimeoutLayout(new QHBoxLayout)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    init();
    connect(m_themeDbus, &GrubThemeDbus::updatingChanged, this, &GrubWidget::updatingChanged);
    connect(m_grubDbus, &GrubDbus::UpdatingChanged, this, &GrubWidget::updatingChanged);
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

    m_tooltip = new QLabel(m_grubBackground);
    m_tooltip->setStyleSheet("QLabel{background:rbga(0,0,0,110);color:white;}");
    m_tooltip->resize(310, 20);
    m_tooltip->setText(tr("Drag and drop an image to change background."));
    m_tooltip->setAlignment(Qt::AlignCenter);
    m_tooltip->move(0, 167);

    QStringList title_list = m_grubDbus->GetSimpleEntryTitles();
    m_bootEntryList->setItemWidth(310);
    m_bootEntryList->addButtons(title_list);
    m_bootEntryList->setFixedSize(300, m_bootEntryList->count()*30);
    m_bootMenuTitle = new DButtonList(m_grubBackground);
    m_bootMenuTitle->setStyleSheet("DUI--DButtonList {background-color: transparent;}\
                                    DUI--DButtonList QPushButton{text-align: left center;\
                                    background-color: transparent;border:none;\
                                    color:white;font-size:7px;border-width:1px;padding-left:20px;}\
                                    DUI--DButtonList QPushButton:checked{\
                                    background-color:#20FFFFFF;\
                                    color: #19A9F9;border-radius:5px;}");
    m_bootMenuTitle->setItemSize(210, 13);
    m_bootMenuTitle->addButtons(title_list);
    m_bootMenuTitle->move(50, 50);
    setDefaultEntry(m_grubDbus->defaultEntry());
    connect(m_grubDbus, &GrubDbus::DefaultEntryChanged, this, &GrubWidget::setDefaultEntry);
    connect(m_bootEntryList, &DButtonList::buttonChecked, m_grubDbus, &GrubDbus::setDefaultEntry);

    m_arrowDefaultBoot->setTitle(tr("Default Boot"));
    m_arrowDefaultBoot->setContent(m_bootEntryList);
    m_arrowDefaultBoot->setExpand(false);

    QWidget *bg_ts = new QWidget;
    bg_ts->setFixedSize(310, 30);
    DSegmentedControl *timeout_select = new DSegmentedControl(bg_ts);
    timeout_select->setStyleSheet(timeout_select->styleSheet()
                                  +"DUI--DSegmentedControl#DSegmentedControl\
    {color:#f0f0f0;border:none;background: transparent;}");
    timeout_select->addSegmented(QStringList()<<"1s"<<"5s"<<"10s"<<"15s"<<"20s"<<"25s"<<"30s");
    timeout_select->setFixedSize(bg_ts->width()*0.9, 24);
    timeout_select->move(bg_ts->rect().center()-timeout_select->rect().center());
    timeout_select->setCurrentIndexByTitle(QString("%1s").arg(m_grubDbus->timeout()));
    connect(m_grubDbus, &GrubDbus::TimeoutChanged, [=](int timeout){
        timeout_select->setCurrentIndexByTitle(QString("%1s").arg(timeout));
    });
    connect(timeout_select, &DSegmentedControl::currentTitleChanged, [=](QString title){
        m_grubDbus->setTimeout(title.replace('s', "").toInt());
    });

    m_arrowBootDelay->setTitle(tr("Boot delay"));
    m_arrowBootDelay->setContent(bg_ts);
    m_arrowBootDelay->setExpand(false);

    DColorPicker *picker1 = new DColorPicker(10, this);
    picker1->setCurrentColor(m_themeDbus->itemColor());
    connect(picker1, &DColorPicker::currentColorChanged, m_themeDbus, &GrubThemeDbus::setItemColor);
    connect(m_themeDbus, &GrubThemeDbus::itemColorChanged, picker1, &DColorPicker::setCurrentColor);

    m_arrowTextColor->setTitle(tr("Text Color"));
    m_arrowTextColor->setContent(picker1);
    m_arrowTextColor->setExpand(false);

    DColorPicker *picker2 = new DColorPicker(10, this);
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

void GrubWidget::setDefaultEntry(const QString &entry)
{
    QStringList title_list = m_grubDbus->GetSimpleEntryTitles();
    int default_index = title_list.indexOf(entry);
    QWidget * tmpw = m_bootEntryList->itemWidget(m_bootEntryList->item(default_index));
    QPushButton *tmpb = qobject_cast<QPushButton*>(tmpw);
    if(tmpb){
        tmpb->setChecked(true);
    }

    tmpw = m_bootMenuTitle->itemWidget(m_bootMenuTitle->item(default_index));
    tmpb = qobject_cast<QPushButton*>(tmpw);
    if(tmpb){
        tmpb->setChecked(true);
    }
}

void GrubWidget::updatingChanged(bool updating)
{
    m_tooltip->setStyleSheet(m_tooltip->styleSheet()+"QLabel{color:#DF8000;}");
    if(updating){
        m_tooltip->setText(tr("Updating..."));
    }else{
        m_tooltip->setText(tr("Successfully updated,reboot to view."));
        QTimer::singleShot(2000, this, &GrubWidget::resetTooltip);
    }
}

void GrubWidget::resetTooltip()
{
    m_tooltip->setStyleSheet(m_tooltip->styleSheet()+"QLabel{color:white;}");
    m_tooltip->setText(tr("Drag and drop an image to change background."));
}
