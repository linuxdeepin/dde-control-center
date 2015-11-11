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
    m_listWidget->addWidget(widget, Qt::AlignRight);
}

void ListWidgetContainer::widgetChecke(int index)
{
    m_listWidget->setChecked(index, true);
}

int ListWidgetContainer::checkedWidgetIndex() const
{
    return m_listWidget->firstChecked();
}

void ListWidgetContainer::setCheckable(bool checkable)
{
    m_listWidget->setCheckable(checkable);
}

void ListWidgetContainer::init(const QString &text)
{
    DHeaderLine *title = new DHeaderLine;

    title->setFixedWidth(DCC::ModuleContentWidth);
    title->setTitle(text);

    QHBoxLayout *button_layout = new QHBoxLayout;
    DTextButton *button_cancel = new DTextButton(tr("Cancel"));
    DTextButton *button_connect = new DTextButton(tr("Connect"));

    button_layout->setMargin(0);
    button_layout->setSpacing(5);
    button_layout->addStretch(1);
    button_layout->addWidget(button_cancel);
    button_layout->addWidget(button_connect);
    button_layout->setContentsMargins(0, 10, 15, 0);

    DSeparatorHorizontal *separator_bottom = new DSeparatorHorizontal;
    separator_bottom->hide();

    m_listWidget = new DListWidget;
    m_listWidget->setContentsMargins(15, 5, 15, 5);
    m_listWidget->setSpacing(5);
    m_listWidget->setStyleSheet(styleSheet());

    DBoxWidget::addWidget(title);
    DBoxWidget::addWidget(new DSeparatorHorizontal);
    DBoxWidget::addWidget(m_listWidget);
    DBoxWidget::addWidget(separator_bottom);
    layout()->addLayout(button_layout);

    connect(this, &ListWidgetContainer::setTitle, title, &DHeaderLine::setTitle);
    connect(button_cancel, &DTextButton::clicked, this, &ListWidgetContainer::leftButtonClicked);
    connect(this, &ListWidgetContainer::setLeftButtonText, button_cancel, &DTextButton::setText);
    connect(button_connect, &DTextButton::clicked, this, &ListWidgetContainer::rightButtonClicked);
    connect(this, &ListWidgetContainer::setRightButtonText, button_connect, &DTextButton::setText);
    connect(m_listWidget, &DListWidget::countChanged, this, [this, separator_bottom](int count) {
        separator_bottom->setVisible(count > 0);
    });
}
