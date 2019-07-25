#include "timezonelist.h"
#include "modules/datetime/timezoneitem.h"

#include "modules/datetime/timezone_dialog/timezone.h"
#include "modules/datetime/timezone_dialog/timezonechooser.h"

dccV20::datetime::TimezoneList::TimezoneList(QWidget *parent)
    : QWidget(parent)
    , m_timezoneGroup(new SettingsGroup)
    , m_headItem(new SettingsHead)
    , m_addTimezoneButton(new QPushButton("wubowen test"))
    , m_timezoneItem(new NextPageWidget)
    , m_centralLayout(new QVBoxLayout)
{
    this->setFixedWidth(300);
    setStyleSheet("background-color: rgba(255, 255, 255, 7.65);");

    //将添加时区代码加进来

    m_centralLayout->addWidget(m_addTimezoneButton);
    setLayout(m_centralLayout);
}

dccV20::datetime::TimezoneList::~TimezoneList()
{

}
