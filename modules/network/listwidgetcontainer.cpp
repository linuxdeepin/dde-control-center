#include <libdui/dheaderline.h>
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

QString ListWidgetContainer::leftButtonText() const
{
    return m_cancelButton->text();
}

QString ListWidgetContainer::rightButtonText() const
{
    return m_connectButton->text();
}

QLayout *ListWidgetContainer::boxWidgetLayout() const
{
    return m_boxWidget->layout();
}

void ListWidgetContainer::init(const QString &text)
{
    DHeaderLine *title = new DHeaderLine;

    title->setFixedWidth(DCC::ModuleContentWidth);
    title->setTitle(text);
    title->setHidden(text.isEmpty());

    m_buttonLayout = new QHBoxLayout;
    m_cancelButton = new DTextButton(tr("Cancel"));
    m_connectButton = new DTextButton(tr("Connect"));

    m_buttonLayout->setMargin(0);
    m_buttonLayout->setSpacing(5);
    m_buttonLayout->addStretch(1);
    m_buttonLayout->addWidget(m_cancelButton);
    m_buttonLayout->addWidget(m_connectButton);
    m_buttonLayout->setContentsMargins(0, 10, 15, 0);

    DSeparatorHorizontal *separator_bottom = new DSeparatorHorizontal;
    DSeparatorHorizontal *separator_top = new DSeparatorHorizontal;
    separator_top->setHidden(title->isHidden());

    m_boxWidget = new DVBoxWidget;
    m_boxWidget->setObjectName("MainBoxWidget");
    m_boxWidget->layout()->setContentsMargins(0, 5, 0, 5);
    m_boxWidget->layout()->setSpacing(5);
    m_boxWidget->setStyleSheet(styleSheet());

    DBoxWidget::addWidget(title);
    DBoxWidget::addWidget(separator_top);
    DBoxWidget::addWidget(m_boxWidget);
    DBoxWidget::addWidget(separator_bottom);
    layout()->addLayout(m_buttonLayout);

    connect(this, &ListWidgetContainer::setTitle, title, [separator_top, title](const QString &text) {
        title->setHidden(text.isEmpty());
        separator_top->setHidden(title->isHidden());
        title->setText(text);
    });
    connect(m_cancelButton, &DTextButton::clicked, this, &ListWidgetContainer::leftButtonClicked);
    connect(this, &ListWidgetContainer::setLeftButtonText, m_cancelButton, &DTextButton::setText);
    connect(this, &ListWidgetContainer::setLeftButtonVisible, m_cancelButton, &DTextButton::setVisible);
    connect(m_connectButton, &DTextButton::clicked, this, &ListWidgetContainer::rightButtonClicked);
    connect(this, &ListWidgetContainer::setRightButtonText, m_connectButton, &DTextButton::setText);
    connect(this, &ListWidgetContainer::setRightButtonVisible, m_connectButton, &DTextButton::setVisible);
    connect(this, &ListWidgetContainer::setBottomSeparatorVisible, separator_bottom, &DSeparatorHorizontal::setVisible);
}
