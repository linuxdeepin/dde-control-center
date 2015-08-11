#include <libdui/dtextbutton.h>
#include <libdui/dseparatorhorizontal.h>
#include <libdui/dsearchedit.h>
#include <libdui/darrowlineexpand.h>
#include <libdui/dtextbutton.h>
#include<libdui/dbuttonlist.h>

#include "mainwidget.h"

MainWidget::MainWidget(QWidget *parent):
    QFrame(parent),
    m_layout(new QVBoxLayout()),
    m_header(new DHeaderLine())
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    init();
}

MainWidget::~MainWidget()
{

}

DArrowLineExpand *addExpand(const QString &title, QWidget *widget)
{
    DArrowLineExpand *expand = new DArrowLineExpand;
    expand->setTitle(title);
    expand->setContent(widget);
    expand->setExpand(false);

    return expand;
}

void MainWidget::init()
{
    m_layout->setMargin(0);

    m_header->setTitle(tr("Keyboard Shortcuts"));
    DTextButton *re_button = new DTextButton(tr("Reset"));
    m_header->setContent(re_button);

    m_layout->setSpacing(0);
    m_layout->addWidget(m_header);
    m_layout->addWidget(new DSeparatorHorizontal);
    m_layout->addWidget(new DSearchEdit);
    m_layout->addWidget(new DSeparatorHorizontal);
    m_layout->addWidget(addExpand(tr("System"), new QWidget));
    m_layout->addWidget(addExpand(tr("Window"), new QWidget));
    m_layout->addWidget(addExpand(tr("Workspace"), new QWidget));
    m_layout->addWidget(new QLabel(tr("Custom")));
    m_layout->addWidget(new DSeparatorHorizontal);
    m_layout->addStretch(1);

    setLayout(m_layout);
}
