#include "presspowerbuttonaction.h"

PressPowerButtonAction::PressPowerButtonAction(QStringList powerReaction, QWidget* parent)
    : QFrame(parent)
{

    m_contentHeight = 30;
    m_pressPowerReActionLine = new DHeaderLine;
    m_pressPowerReActionLine->setFixedHeight(m_contentHeight);
    m_pressPowerReActionLine->setTitle(tr("PressPowerButton"));

    m_pressPowerReActionExpand = new DBaseExpand;
    m_pressPowerReActionExpand->setFixedHeight(m_contentHeight);
    m_pressPowerReActionExpand->setHeader(m_pressPowerReActionLine);

    m_powerActionButtonGrid = new DButtonGrid(1, 3);
    m_powerActionButtonGrid->setItemSize(100, m_contentHeight);

    m_powerActionButtonGrid->addButtons(powerReaction);
    m_powerActionButtonGrid->checkButtonByIndex(2);
    m_pressPowerReActionExpand->setContent(m_powerActionButtonGrid);
    m_pressPowerReActionExpand->setExpand(true);

    pressPowerButtonActionLayout = new QVBoxLayout;
    pressPowerButtonActionLayout->setMargin(0);
    pressPowerButtonActionLayout->setSpacing(0);
    pressPowerButtonActionLayout->addWidget(m_pressPowerReActionLine);
    pressPowerButtonActionLayout->addWidget(m_pressPowerReActionExpand);
    setLayout(pressPowerButtonActionLayout);

    initConnection();
}
void PressPowerButtonAction::setTitle(QString titleName) {
    m_pressPowerReActionLine->setTitle(titleName);
}
void PressPowerButtonAction::initConnection() {

    connect(m_powerActionButtonGrid, SIGNAL(buttonChecked(QString)),
            this, SIGNAL(powerButtonAction(QString)));
}

void PressPowerButtonAction::setPowerButtonAction(int buttonId) {
    switch (buttonId) {
        case 1: { m_powerActionButtonGrid->checkButtonByIndex(1);break;}
        case 2: { m_powerActionButtonGrid->checkButtonByIndex(0);break;}
        case 4: { m_powerActionButtonGrid->checkButtonByIndex(2);break;}
    }
}
PressPowerButtonAction::~PressPowerButtonAction() {

}
