#include "createuserpanel.h"

CreateUserPanel::CreateUserPanel(QWidget *parent) : QWidget(parent)
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);

    initHeader();
    initInfoLine();
    initInputLline();

    m_layout->addStretch();
}

void CreateUserPanel::initHeader()
{

    DSeparatorHorizontal *s1 = new DSeparatorHorizontal();
    DSeparatorHorizontal *s2 = new DSeparatorHorizontal();
    DHeaderLine *header = new DHeaderLine();
    header->setTitle("Create New User");

    m_layout->addWidget(s1);
    m_layout->addWidget(header);
    m_layout->addWidget(s2);
}

void CreateUserPanel::initInfoLine()
{
    QLabel *l = new QLabel();
    l->setFixedHeight(100);
    l->setStyleSheet("background-color:#1A1B1B");
    DSeparatorHorizontal *s = new DSeparatorHorizontal();

    m_layout->addWidget(l);
    m_layout->addWidget(s);
}

void CreateUserPanel::initInputLline()
{
    InputLine *nameLine = new InputLine();
    PasswdLine *passwdNew = new PasswdLine();
    PasswdLine *passwdRepeat = new PasswdLine();
    AccountTypeLine *accountType = new AccountTypeLine();
    ConfirmButtonLine *confirmLine = new ConfirmButtonLine();

    DSeparatorHorizontal *s1 = new DSeparatorHorizontal();
    DSeparatorHorizontal *s2 = new DSeparatorHorizontal();
    DSeparatorHorizontal *s3 = new DSeparatorHorizontal();
    DSeparatorHorizontal *s4 = new DSeparatorHorizontal();
    m_layout->addWidget(nameLine);
    m_layout->addWidget(s1);
    m_layout->addWidget(passwdNew);
    m_layout->addWidget(s2);
    m_layout->addWidget(passwdRepeat);
    m_layout->addWidget(s3);
    m_layout->addWidget(accountType);
    m_layout->addWidget(confirmLine);
    m_layout->addWidget(s4);
}



