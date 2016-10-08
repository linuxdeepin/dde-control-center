#include "modulewidget.h"

ModuleWidget::ModuleWidget(QWidget *parent)
    : QWidget(parent)
{
    m_moduleIcon = new QLabel;
    m_moduleTitle = new QLabel;

    QHBoxLayout *titleLayout = new QHBoxLayout;
    titleLayout->addWidget(m_moduleIcon);
    titleLayout->addWidget(m_moduleTitle);

    m_centeralLayout = new QVBoxLayout;
    m_centeralLayout->addLayout(titleLayout);

    setLayout(m_centeralLayout);
}

void ModuleWidget::setTitle(const QString &title)
{
    m_moduleTitle->setText(title);
}

void ModuleWidget::setIcon(const QPixmap &icon)
{
    m_moduleIcon->setPixmap(icon);
}
