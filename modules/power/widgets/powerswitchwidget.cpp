#include "powerswitchwidget.h"
#include <QStyle>
PowerSwitchWidget::PowerSwitchWidget(QString title, QWidget *parent)
    : QWidget(parent) {
//    setDirection(DoubleCornerWidget::Direction::None);
    setFixedSize(320, 40);
//    setRadiusMargins(0, 0);
    m_title = title;
    m_titleLabel = new QLabel(this);
    m_titleLabel->setText(QString("%1").arg(m_title));
    m_titleLabel->setStyleSheet("color:white");
    m_switchButton = new DSwitchButton(this);

    m_Layout = new QHBoxLayout;
    m_Layout->setMargin(0);
    m_Layout->setSpacing(0);
    m_Layout->addSpacing(10);
    m_Layout->addWidget(m_titleLabel);
    m_Layout->addStretch();
    m_Layout->addWidget(m_switchButton);
    m_Layout->addSpacing(10);
    setLayout(m_Layout);


    this->setStyleSheet(QString("QWidget{background:1%;}").arg(DCC::BgLightColor.name()));
    update();
    connect(m_switchButton, &DSwitchButton::checkedChanged, this, &PowerSwitchWidget::checkedChanged);
}

PowerSwitchWidget::~PowerSwitchWidget() {

}

void PowerSwitchWidget::setChecked(bool checked) {
    m_switchButton->setChecked(checked);
}
