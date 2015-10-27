#include "mirroritemwidget.h"
#include "constants.h"

#include <QHBoxLayout>

#include <libdui/dthememanager.h>
#include <libdui/dseparatorhorizontal.h>

MirrorItemWidget::MirrorItemWidget(QWidget *parent)
    : QFrame(parent)
{
    m_selectedBtn = new DImageButton;
    m_selectedBtn->setNormalPic(":/images/images/select_active.png");
    m_selectedBtn->setVisible(false);
    m_mirrorName = new QLabel;
    m_mirrorName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_mirrorName->setObjectName("MirrorName");
    m_mirrorUrl = new QLabel;
    m_mirrorUrl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    // TODO: remove
    m_mirrorName->setText("官方稳定源");
    m_mirrorUrl->setText("http://packages.linuxdeepin.com");

    QBoxLayout *iconLayout = new QHBoxLayout;
    iconLayout->addWidget(m_selectedBtn);
    iconLayout->setAlignment(m_selectedBtn, Qt::AlignCenter);

    QWidget *iconWidget = new QWidget;
    iconWidget->setLayout(iconLayout);
    iconWidget->setFixedWidth(45);
    iconWidget->setAttribute(Qt::WA_TranslucentBackground);

    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addStretch();
    vLayout->addWidget(m_mirrorName);
    vLayout->addWidget(m_mirrorUrl);
    vLayout->addStretch();
    vLayout->setMargin(0);

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(iconWidget);
    hLayout->addLayout(vLayout);
    hLayout->setMargin(0);
    hLayout->setSpacing(0);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(new DSeparatorHorizontal);
    mainLayout->addLayout(hLayout);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    setSelected(true);

    setLayout(mainLayout);
    setFixedWidth(DCC::ModuleContentWidth);
    setFixedHeight(50);

    D_THEME_INIT_WIDGET(MirrorItemWidget, selected);
}

void MirrorItemWidget::setSelected(bool state)
{
    if (state == m_selected)
        return;

    m_selected = state;
    m_selectedBtn->setVisible(state);

    emit selectStateChanged(state);
}
