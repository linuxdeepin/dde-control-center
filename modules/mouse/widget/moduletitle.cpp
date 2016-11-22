#include "moduletitle.h"

ModuleTitle::ModuleTitle(const QString &title, QObject *parent)
{
    Q_UNUSED(parent);
    m_mainLayout = new QVBoxLayout;
    m_title = new QLabel;
    m_title->setText(title);
    m_mainLayout->addWidget(m_title);
    setLayout(m_mainLayout);
    setObjectName("ModuleTitle");
}

ModuleTitle::~ModuleTitle()
{

}
