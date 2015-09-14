#include "dextendboard.h"

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
    m_customChooseLabel->setText(tr("Custom options"));
    m_customChooseLabel->setStyleSheet(QString("background-color: %1;color:rgb(180,180,180)").arg(DCC::BgLightColor.name()));

    m_poweroffLabel = new QLabel;
    m_poweroffLabel->setText(tr("Turn off the display"));
    m_poweroffLabel->setStyleSheet("color: rgb(180, 180, 180)");

    m_poweroffButtonGrid = new DButtonGrid(1, 7);
    m_poweroffButtonGrid->setFixedHeight(30);
    m_poweroffButtonGrid->setItemSize(32, 28);

    m_poweroffButtons << "1m" << "5m" << "10m" << "15m" << "30m" << "1h" << tr("never");
    m_poweroffButtonGrid->addButtons(m_poweroffButtons);
    m_poweroffButtonGrid->checkButtonByIndex(2);
    m_suspendLabel = new QLabel;
    m_suspendLabel->setText(tr("Suspend"));
    m_suspendLabel->setStyleSheet("color: rgb(180, 180, 180)");;
    m_suspendButtonGrid = new DButtonGrid(1, 7);
    m_poweroffButtonGrid->setFixedHeight(30);
    m_suspendButtonGrid->setItemSize(32, 28);

    m_suspendButtons << "1m" << "5m" << "10m" << "15m" << "30m" << "1h" << tr("never");
    m_suspendButtonGrid->addButtons(m_suspendButtons);
    m_suspendButtonGrid->checkButtonByIndex(2);

    m_bottomSeparator = new DSeparatorHorizontal(m_label);

    m_customLabel = new QLabel;
    m_customLabel->setStyleSheet(QString("background-color: %1;color:rgb(180,180,180)").arg(DCC::BgLightColor.name()));

    customPowerOffLayout = new QHBoxLayout;
    customPowerOffLayout->setMargin(0);
    customPowerOffLayout->setSpacing(0);
    customPowerOffLayout->addSpacing(DUI::HEADER_LEFT_MARGIN);
    customPowerOffLayout->addWidget(m_customChooseLabel);
    customPowerOffLayout->addStretch();
    m_customLabel->setLayout(customPowerOffLayout);

    powerOffLayout = new QHBoxLayout;
    powerOffLayout->setMargin(0);
    powerOffLayout->setSpacing(0);
    powerOffLayout->addSpacing(DUI::HEADER_LEFT_MARGIN);
    powerOffLayout->addWidget(m_poweroffLabel);


    suspendLayout = new QHBoxLayout;
    suspendLayout->setMargin(0);
    suspendLayout->setSpacing(0);
    suspendLayout->addSpacing(DUI::HEADER_LEFT_MARGIN);
    suspendLayout->addWidget(m_suspendLabel);

    m_contentLayout = new QVBoxLayout;
    m_contentLayout->setMargin(0);
    m_contentLayout->setSpacing(0);
    m_contentLayout->addWidget(m_guideExtendTriangle);
    m_contentLayout->addWidget(m_customLabel);
    m_contentLayout->addLayout(powerOffLayout);
    m_contentLayout->addWidget(m_poweroffButtonGrid);
    m_contentLayout->addLayout(suspendLayout);
    m_contentLayout->addWidget(m_suspendButtonGrid);
    m_contentLayout->addWidget(m_bottomSeparator);
    setLayout(m_contentLayout);

}

DExtendBoard::~DExtendBoard()
{
    delete m_animation;
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
        m_suspendLabel->show();
        m_suspendButtonGrid->show();
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
        m_suspendLabel->hide();
        m_suspendButtonGrid->hide();
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
