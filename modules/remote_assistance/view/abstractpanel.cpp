#include <dtextbutton.h>

#include "abstractpanel.h"
#include "moduleheader.h"
#include "constants.h"

#include "errorview.h"


#include <QDebug>

AbstractPanel::AbstractPanel(const QString& title, QWidget *parent)
    : QWidget(parent),
      m_viewLayout(new QVBoxLayout),
      m_view(new QWidget),
      m_headerLine(new ModuleHeader(title, false))
{
    m_viewLayout->setSpacing(0);
    m_viewLayout->setMargin(0);

    setLayout(m_viewLayout);
    addWidget(m_headerLine);
    m_viewLayout->addStretch();

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    setFixedWidth(DCC::ModuleContentWidth);
    setFixedHeight(800);

}

void AbstractPanel::setTitle(const QString &title)
{
    m_headerLine->setTitle(title);
}

AbstractPanel* AbstractPanel::addWidget(QWidget*w)
{
    m_viewLayout->insertWidget(m_viewLayout->count()-1, w);
    return this;
}

AbstractPanel* AbstractPanel::addLayout(QLayout* l, int stretch)
{
    m_viewLayout->addLayout(l, stretch);
    return this;
}

void AbstractPanel::setWidget(QWidget *w)
{
    m_view->hide();
    m_view->deleteLater();
    m_view = w;
    addWidget(w);
}

void AbstractPanel::onNoNetwork()
{
    setTitle(tr("Remote Accessitance"));
    qDebug ()<< "no network";
    auto view = new ErrorView;
    auto button = new DTextButton(tr("Ok"));
    QObject::connect(button, &DTextButton::clicked, [this] {
        emit changePanel(ViewPanel::Main);
    });
    view->addButton(button);

    setWidget(view->setText(tr("No network connections are available, please retry after connected")));
}
