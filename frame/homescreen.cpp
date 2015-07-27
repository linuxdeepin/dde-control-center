#include <QDebug>
#include <QGridLayout>
#include <QPixmap>
#include <QLabel>
#include <QEvent>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "homescreen.h"
#include "constants.h"

HomeScreen::HomeScreen(QList<ModuleMetaData> modules, QWidget *parent) :
    QFrame(parent)
{
    QVBoxLayout * layout = new QVBoxLayout(this);
    this->setLayout(layout);

    QSpacerItem *vSpace = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);

    m_grid = new QGridLayout;
    layout->addLayout(m_grid);
    layout->addSpacerItem(vSpace);

    foreach (ModuleMetaData meta, modules) {
        ModuleButton * button = new ModuleButton(meta, this);

        m_grid->addWidget(button, m_moduleCount / 3, m_moduleCount % 3);
        ++m_moduleCount;

        connect(button, &ModuleButton::clicked, this, &HomeScreen::buttonClicked);
    }

#if 1 // just test more buttons
    for (int i(0); i != 3; ++i)
        foreach (ModuleMetaData meta, modules) {
            ModuleButton * button = new ModuleButton(meta, this);

            m_grid->addWidget(button, m_moduleCount / 3, m_moduleCount % 3);
            ++m_moduleCount;

            connect(button, &ModuleButton::clicked, this, &HomeScreen::buttonClicked);
        }
#endif
}

void HomeScreen::buttonClicked()
{
    ModuleButton * btn = qobject_cast<ModuleButton*>(sender());
    this->moduleSelected(btn->metaData());
}

// class ModuleButton
ModuleButton::ModuleButton(ModuleMetaData metaData, QWidget * parent) :
    QFrame(parent),
    m_meta(metaData)
{
    setFixedSize(96, 96);
    setMouseTracking(true);

    m_icon = new QLabel(this);
    m_text = new QLabel(this);
    m_text->setText(m_meta.name);
    m_text->setAlignment(Qt::AlignCenter);

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addStretch();
    hLayout->addWidget(m_icon);
    hLayout->addStretch();

    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addLayout(hLayout);
    vLayout->addWidget(m_text);

    this->setLayout(vLayout);

    setState(Normal);
}

ModuleMetaData ModuleButton::metaData()
{
    return m_meta;
}

void ModuleButton::enterEvent(QEvent *)
{
    this->setState(Hover);
}

void ModuleButton::leaveEvent(QEvent *)
{
    this->setState(Normal);
}

void ModuleButton::mousePressEvent(QMouseEvent *)
{
    this->clicked();
}

void ModuleButton::mouseReleaseEvent(QMouseEvent *)
{
//    this->setState(Hover);
}

void ModuleButton::setState(State state)
{
    switch (state) {
    case Normal:
        this->setStyleSheet("QFrame { background-color: transparent; border-radius: 3 }");
        m_icon->setPixmap(QPixmap(QString("modules/icons/%1").arg(m_meta.normalIcon)));
        m_text->setStyleSheet(QString("QLabel { color: %1 }").arg(DCC::TextNormalColor.name()));
        break;
    case Hover:
        this->setStyleSheet(QString("QFrame { background-color: %1; border-radius: 3 }").arg(DCC::BgDarkColor.name()));
        m_icon->setPixmap(QPixmap(QString("modules/icons/%1").arg(m_meta.hoverIcon)));
        m_text->setStyleSheet(QString("QLabel { color: %1 }").arg(DCC::TextHoverColor.name()));
        break;
    default:
        break;
    }
}
