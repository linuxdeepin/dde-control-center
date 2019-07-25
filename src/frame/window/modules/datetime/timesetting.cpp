#include "timesetting.h"

DCC_NAMESPACE::datetime::TimeSetting::TimeSetting(QWidget *parent)
    : QWidget(parent)
    , m_centerLayout(new QVBoxLayout)
    , m_cancel(new QPushButton("time setting"))
    , m_sure(new QPushButton("wubowen test2"))
{
    m_centerLayout->addWidget(m_cancel);
    m_centerLayout->addWidget(m_sure);
    setLayout(m_centerLayout);
}

DCC_NAMESPACE::datetime::TimeSetting::~TimeSetting()
{

}
