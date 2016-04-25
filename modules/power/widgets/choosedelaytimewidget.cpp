#include "choosedelaytimewidget.h"

#include <QHBoxLayout>
#include <QDebug>

ChooseDelayTimeWidget::ChooseDelayTimeWidget(QString title, QWidget *parent)
    : DoubleCornerWidget(parent) {
    setFixedSize(320, 80);
    setRadiusMargins(0, 0);

    m_titleLabel = new QLabel(this);
    m_titleLabel->setText(title);
    m_titleLabel->setStyleSheet("QLabel{color: white;}");
    m_numLabel = new QLabel(this);
    m_numLabel->setText(QString("%1").arg(m_currentNum));
    m_numLabel->setStyleSheet("QLabel{color: white;}");
    QHBoxLayout* titleHLayout = new QHBoxLayout;
    titleHLayout->setMargin(0);
    titleHLayout->setSpacing(0);
    titleHLayout->addSpacing(10);
    titleHLayout->addWidget(m_titleLabel);
    titleHLayout->addStretch();
    titleHLayout->addWidget(m_numLabel);
    titleHLayout->addSpacing(25);

    m_timeBtnGrid = new DButtonGrid(1, 7, this);

    QStringList timeBtns;
    timeBtns << "1m" << "5m" << "10m" << "15m" << "30m" << "1h" << tr("Never");
    m_timeBtnGrid->addButtons(timeBtns);
    m_timeBtnGrid->setItemSize(33, 28);
    m_timeBtnGrid->setColumnWidth(6, 48);
    m_timeBtnGrid->checkButtonByIndex(0);

    QVBoxLayout* mLayout = new QVBoxLayout;
    mLayout->setMargin(0);
    mLayout->setSpacing(0);
    mLayout->addLayout(titleHLayout);
    mLayout->addWidget(m_timeBtnGrid);

    setContentLayout(mLayout);
    connect(m_timeBtnGrid, &DButtonGrid::buttonCheckedIndexChanged, this, &ChooseDelayTimeWidget::handleCurrentItemIndexChanged);
}

void ChooseDelayTimeWidget::setCurrentTime(int time) {

    switch(time) {
    case 60: m_timeBtnGrid->checkButtonByIndex(0); m_currentNum = tr("1 Minute"); break;
    case 300: m_timeBtnGrid->checkButtonByIndex(1);m_currentNum = tr("5 Minutes");break;
    case 600: m_timeBtnGrid->checkButtonByIndex(2);m_currentNum = tr("10 Minutes");break;
    case 900: m_timeBtnGrid->checkButtonByIndex(3);m_currentNum = tr("15 Minutes");break;
    case 1800: m_timeBtnGrid->checkButtonByIndex(4);m_currentNum = tr("30 Minutes");break;
    case 3600: m_timeBtnGrid->checkButtonByIndex(5);m_currentNum = tr("1 Hour");break;
    case 0: m_timeBtnGrid->checkButtonByIndex(6);m_currentNum = tr("Never");break;
    default: break;
    }
    m_numLabel->setText(QString("%1").arg(m_currentNum));
}

void ChooseDelayTimeWidget::handleCurrentItemIndexChanged(int index) {
    qDebug() << "handle:" << index;
    switch (index) {
    case 0:emit currentTimeChanged(60);m_currentNum = tr("1 Minute");break;
    case 1:emit currentTimeChanged(300);m_currentNum = tr("5 Minutes");break;
    case 2:emit currentTimeChanged(600);m_currentNum = tr("10 Minutes");break;
    case 3:emit currentTimeChanged(900);m_currentNum = tr("15 Minutes");break;
    case 4:emit currentTimeChanged(1800);m_currentNum = tr("30 Minutes");break;
    case 5:emit currentTimeChanged(3600);m_currentNum = tr("1 Hour");break;
    case 6:emit currentTimeChanged(0);m_currentNum = tr("Never");break;
    default:break;
    }
    m_numLabel->setText(QString("%1").arg(m_currentNum));
}
ChooseDelayTimeWidget::~ChooseDelayTimeWidget() {

}
