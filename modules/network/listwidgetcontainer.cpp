#include <libdui/dheaderline.h>
#include <libdui/dtextbutton.h>
#include <libdui/dseparatorhorizontal.h>
#include <libdui/dthememanager.h>

#include "constants.h"

#include "listwidgetcontainer.h"

ListWidgetContainer::ListWidgetContainer(const QString &title, QWidget *parent) :
    DVBoxWidget(parent)
{
    D_THEME_INIT_WIDGET(ListWidgetContainer)

    init(title);
}

void ListWidgetContainer::addWidget(QWidget *widget)
{
    m_boxWidget->layout()->addWidget(widget, 0, Qt::AlignRight);
}

void ListWidgetContainer::setBoxWidgetContentsMargins(int left, int top, int right, int bottom)
{
    m_boxWidget->layout()->setContentsMargins(left, top, right, bottom);
}

void ListWidgetContainer::setButtonsVisible(bool visible)
{
    emit setLeftButtonVisible(visible);
    emit setRightButtonVisible(visible);

    if(visible) {
        m_buttonLayout->setContentsMargins(0, 10, 0, 0);
    } else {
        m_buttonLayout->setContentsMargins(0, 0, 0, 0);
    }
}

void ListWidgetContainer::init(const QString &text)
{
    DHeaderLine *title = new DHeaderLine;

    title->setFixedWidth(DCC::ModuleContentWidth);
    title->setTitle(text);

    m_buttonLayout = new QHBoxLayout;
    DTextButton *button_cancel = new DTextButton(tr("Cancel"));
    DTextButton *button_connect = new DTextButton(tr("Connect"));

    m_buttonLayout->setMargin(0);
    m_buttonLayout->setSpacing(5);
    m_buttonLayout->addStretch(1);
    m_buttonLayout->addWidget(button_cancel);
    m_buttonLayout->addWidget(button_connect);
    m_buttonLayout->setContentsMargins(0, 10, 15, 0);

    DSeparatorHorizontal *separator_bottom = new DSeparatorHorizontal;
    //separator_bottom->hide();

    m_boxWidget = new DVBoxWidget;
    m_boxWidget->setObjectName("MainBoxWidget");
    m_boxWidget->layout()->setContentsMargins(0, 5, 0, 5);
    m_boxWidget->layout()->setSpacing(5);
    m_boxWidget->setStyleSheet(styleSheet());

    DBoxWidget::addWidget(title);
    DBoxWidget::addWidget(new DSeparatorHorizontal);
    DBoxWidget::addWidget(m_boxWidget);
    DBoxWidget::addWidget(separator_bottom);
    layout()->addLayout(m_buttonLayout);

    connect(this, &ListWidgetContainer::setTitle, title, &DHeaderLine::setTitle);
    connect(button_cancel, &DTextButton::clicked, this, &ListWidgetContainer::leftButtonClicked);
    connect(this, &ListWidgetContainer::setLeftButtonText, button_cancel, &DTextButton::setText);
    connect(this, &ListWidgetContainer::setLeftButtonVisible, button_cancel, &DTextButton::setVisible);
    connect(button_connect, &DTextButton::clicked, this, &ListWidgetContainer::rightButtonClicked);
    connect(this, &ListWidgetContainer::setRightButtonText, button_connect, &DTextButton::setText);
    connect(this, &ListWidgetContainer::setRightButtonVisible, button_connect, &DTextButton::setVisible);
//    connect(m_boxWidget, &DVBoxWidget::countChanged, this, [this, separator_bottom](int count) {
//        separator_bottom->setVisible(count > 0);
//    });
}
