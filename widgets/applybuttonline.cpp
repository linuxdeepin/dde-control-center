/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

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
    layout->addSpacing(DTK_WIDGET_NAMESPACE::BUTTON_MARGIN);
    layout->addWidget(confirmButton);
    contentWidget->setLayout(layout);

    setContent(contentWidget);
}
