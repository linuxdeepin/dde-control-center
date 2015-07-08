#include <QDebug>
#include <QGridLayout>
#include <QToolButton>
#include <QPixmap>

#include "homescreen.h"

HomeScreen::HomeScreen(QList<ModuleMetaData> modules, QWidget *parent) :
    QFrame(parent)
{
    QHBoxLayout * layout = new QHBoxLayout(this);
    this->setLayout(layout);

    m_grid = new QGridLayout;
    layout->addStretch();
    layout->addLayout(m_grid);
    layout->addStretch();

    foreach (ModuleMetaData meta, modules) {
        QPixmap icon(QString("modules/icons/%1").arg(meta.normalIcon));

        QToolButton * button = new QToolButton(this);
        button->setIcon(icon);
        button->setIconSize(QSize(38, 38));
        button->setText(meta.name);
        button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        button->setFixedSize(96, 96);
        button->setStyleSheet("QToolButton { background-color: grey }");

        m_grid->addWidget(button);
    }
}
