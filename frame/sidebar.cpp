#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QPropertyAnimation>

#include "sidebar.h"
#include "constants.h"
#include "sidebarmodel.h"
#include "sidebardelegate.h"

SideBar::SideBar(QWidget *parent)
    : QFrame(parent)
{
    m_sidebarView = new SidebarView;
    m_sidebarView->setModel(new SidebarModel);
    m_sidebarView->setItemDelegate(new SidebarDelegate);
    m_sidebarView->installEventFilter(this);

    m_sidebarLayout = new QVBoxLayout;
    m_sidebarLayout->addWidget(m_sidebarView, 1);
    m_sidebarLayout->setAlignment(m_sidebarView, Qt::AlignHCenter);
    m_sidebarLayout->addStretch();
    m_sidebarLayout->setContentsMargins(0, 24, 0, 0);
    m_sidebarLayout->setSpacing(24);

    setFixedWidth(DCC::SideBarWidth);
    setLayout(m_sidebarLayout);

    connect(m_sidebarView, &SidebarView::moduleSelected, this, &SideBar::moduleSelected);
}

void SideBar::switchToModule(const ModuleMetaData &meta)
{
    SidebarModel *model = qobject_cast<SidebarModel *>(m_sidebarView->model());
    model->setSelectedPluginId(meta.id);
}

bool SideBar::eventFilter(QObject *o, QEvent *e)
{
    if (o == m_sidebarView && e->type() == QEvent::Resize)
        m_sidebarView->setMaximumSize(m_sidebarView->sizeHint());

    return false;
}
