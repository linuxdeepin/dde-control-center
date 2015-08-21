#include "applybuttonline.h"

ApplyButtonLine::ApplyButtonLine(QWidget *parent) : DHeaderLine(parent)
{
    DTextButton *cancelButton = new DTextButton(tr("Cancel"));
    connect(cancelButton, &DTextButton::clicked, this, &ApplyButtonLine::cancel);
    DTextButton *confirmButton = new DTextButton(tr("Apply"));
    connect(confirmButton, &DTextButton::clicked, this, &ApplyButtonLine::cancel);

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
