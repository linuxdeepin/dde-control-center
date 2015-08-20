#include "confirmbuttonline.h"

ConfirmButtonLine::ConfirmButtonLine(QWidget *parent) : DHeaderLine(parent)
{
    DTextButton *cancelButton = new DTextButton("Cancel");
    connect(cancelButton, &DTextButton::clicked, this, &ConfirmButtonLine::cancel);
    DTextButton *confirmButton = new DTextButton("Confirm");
    connect(confirmButton, &DTextButton::clicked, this, &ConfirmButtonLine::confirm);

    QWidget *contentWidget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(cancelButton);
    layout->addSpacing(DUI::BUTTON_MARGIN);
    layout->addWidget(confirmButton);
    contentWidget->setLayout(layout);

    setContent(contentWidget);
}

