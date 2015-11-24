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

    QBoxLayout *iconLayout = new QHBoxLayout;
    iconLayout->addWidget(m_selectedBtn);
    iconLayout->setAlignment(m_selectedBtn, Qt::AlignCenter);
    iconLayout->setSpacing(0);
    iconLayout->setMargin(0);

    QWidget *iconWidget = new QWidget;
    iconWidget->setLayout(iconLayout);
    iconWidget->setFixedWidth(30);
    iconWidget->setAttribute(Qt::WA_TranslucentBackground);

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(iconWidget);
    hLayout->addWidget(m_mirrorName);
    hLayout->setMargin(0);
    hLayout->setSpacing(0);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(hLayout);
    mainLayout->addWidget(new DSeparatorHorizontal);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

//    setSelected(true);

    setLayout(mainLayout);
    setFixedWidth(DCC::ModuleContentWidth);
    setFixedHeight(45);

    D_THEME_INIT_WIDGET(MirrorItemWidget, checked);
}

void MirrorItemWidget::setSelected(bool state)
{
    if (state == m_selected)
        return;

    m_selected = state;
    m_selectedBtn->setVisible(state);

    emit selectStateChanged(state);
}

QSize MirrorItemWidget::sizeHint() const
{
    return QSize(width(), height());
}

void MirrorItemWidget::mouseReleaseEvent(QMouseEvent *e)
{
    emit clicked();

    QFrame::mouseReleaseEvent(e);
}
