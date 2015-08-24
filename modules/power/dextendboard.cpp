#include "dextendboard.h"
#include <QtCore>
#include <QtWidgets>
#include <QtGui>

DExtendBoard::DExtendBoard(QFrame *parent)
    : QFrame(parent)
{
    m_label = new QLabel;
    value = false;
    expand = false;
    m_label->setStyleSheet(QString("QLabel{color: %1;font-size:12px;}").arg(DCC::TextNormalColor.name()));
    setStyleSheet(QString("background-color: %1;").arg(DCC::BgDarkColor.name()));
    m_animation = new QPropertyAnimation(this, "height");

    expand = false;
    connect(m_animation, &QPropertyAnimation::valueChanged, this, &DExtendBoard::adjustSize);
    m_guideExtendTriangle = new TriangleRectangle;
    m_guideExtendTriangle->setFixedHeight(8);
    m_guideExtendTriangle->fillColor = qRgb(37, 38, 39);
    m_guideExtendTriangle->strokeColor = qRgb(37, 38, 39);
    m_guideExtendTriangle->setStyleSheet(QString("background-color: %1;").arg(DCC::BgDarkColor.name()));
    m_customChooseLabel = new QLabel;
    m_customChooseLabel->setFixedHeight(30);
    m_customChooseLabel->setText("自定义");
    m_customChooseLabel->setStyleSheet(QString("background-color: %1;color:rgb(180,180,180)").arg(DCC::BgLightColor.name()));

    m_poweroffLabel = new QLabel;
    m_poweroffLabel->setText("关闭显示器");
    m_poweroffLabel->setStyleSheet("color: rgb(180, 180, 180)");

    m_poweroffButtonGrid = new DButtonGrid(1, 7);
    m_poweroffButtonGrid->setFixedHeight(30);
    m_poweroffButtonGrid->setItemSize(32, 28);

    m_poweroffButtons << "1m" << "5m" << "10m" << "15m" << "30m" << "1h" << "never";
    m_poweroffButtonGrid->addButtons(m_poweroffButtons);
    m_poweroffButtonGrid->checkButtonByIndex(2);
    m_standByLabel = new QLabel;
    m_standByLabel->setText("待机");
    m_standByLabel->setStyleSheet("color: rgb(180, 180, 180)");;
    m_standByButtonGrid = new DButtonGrid(1, 7);
    m_poweroffButtonGrid->setFixedHeight(30);
    m_standByButtonGrid->setItemSize(32, 28);

    m_standByButtons << "1m" << "5m" << "10m" << "15m" << "30m" << "1h" << "never";
    m_standByButtonGrid->addButtons(m_standByButtons);
    m_standByButtonGrid->checkButtonByIndex(2);

    m_bottomSeparator = new DSeparatorHorizontal(m_label);

    customPowerOffLayout = new QHBoxLayout;
    customPowerOffLayout->setMargin(0);
    customPowerOffLayout->setSpacing(0);
    customPowerOffLayout->addWidget(m_customChooseLabel);


    powerOffLayout = new QHBoxLayout;
    powerOffLayout->setMargin(0);
    powerOffLayout->setSpacing(0);
    powerOffLayout->addWidget(m_poweroffLabel);


    standByLayout = new QHBoxLayout;
    standByLayout->setMargin(0);
    standByLayout->setSpacing(0);
    standByLayout->addWidget(m_standByLabel);

    m_contentLayout = new QVBoxLayout;
    m_contentLayout->setMargin(0);
    m_contentLayout->setSpacing(0);
    m_contentLayout->addWidget(m_guideExtendTriangle);
    m_contentLayout->addLayout(customPowerOffLayout);
    m_contentLayout->addLayout(powerOffLayout);
    m_contentLayout->addWidget(m_poweroffButtonGrid);
    m_contentLayout->addLayout(standByLayout);
    m_contentLayout->addWidget(m_standByButtonGrid);
    m_contentLayout->addWidget(m_bottomSeparator);
    setLayout(m_contentLayout);


}

DExtendBoard::~DExtendBoard()
{
}
void DExtendBoard::setWidth(int ex_width) {
    setFixedWidth(ex_width);
}
void DExtendBoard::setHeight(int ex_height) {
    setFixedHeight(ex_height);
}

void DExtendBoard::setExpand(bool expand) {
    if (value == expand && !firstExpand) {
        value = expand;
        return;
    }
    value = expand;

    if (expand) {
        m_animation->setDuration(200);
        m_animation->setEasingCurve(QEasingCurve::InCubic);
        m_guideExtendTriangle->show();
        m_customChooseLabel->show();
        m_poweroffLabel->show();
        m_standByLabel->show();
        m_standByButtonGrid->show();
        m_poweroffButtonGrid->show();
        this->setFixedHeight(200);
        m_animation->setStartValue(2);
        m_animation->setEndValue(155);

    } else {
        m_animation->setDuration(100);

        m_animation->setStartValue(155);
        m_animation->setEndValue(2);
        m_guideExtendTriangle->hide();
        m_customChooseLabel->hide();
        m_poweroffLabel->hide();
        m_standByLabel->hide();
        m_standByButtonGrid->hide();
        m_poweroffButtonGrid->hide();
        m_bottomSeparator->move(1, 1);
    }
    firstExpand = false;
    m_animation->stop();
    m_animation->start();
}
void DExtendBoard::resizeEvent(QResizeEvent *e) {
    setFixedHeight(e->size().height());
}
