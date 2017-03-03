#include "modulewidget.h"
#include "modulewidgetheader.h"

#include <QEvent>
#include <QDebug>

using namespace dcc::widgets;

namespace dcc {

static const char *ObjectNameTitle = "ModuleHeaderTitle";
static const char *ObjectNameTemplateIcon = "ModuleHeaderIcon%1";

ModuleWidget::ModuleWidget()
    : QWidget(nullptr)
{
    m_moduleIcon = new QLabel;
    m_moduleIcon->setFixedSize(24, 24);

    m_moduleTitle = new LargeLabel;
    m_moduleTitle->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    m_moduleTitle->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_moduleTitle->setObjectName(ObjectNameTitle);

    QHBoxLayout *titleLayout = new QHBoxLayout;
    titleLayout->setSpacing(0);
    titleLayout->setContentsMargins(11, 0, 0, 0);
    titleLayout->addWidget(m_moduleIcon);
    titleLayout->setAlignment(m_moduleIcon, Qt::AlignCenter);
    titleLayout->addWidget(m_moduleTitle);

    ModuleWidgetHeader *headerWidget = new ModuleWidgetHeader;
    headerWidget->setLayout(titleLayout);

    m_centralLayout = new QVBoxLayout;
    m_centralLayout->addWidget(headerWidget);
    m_centralLayout->setSpacing(10);
    m_centralLayout->setMargin(0);

    setLayout(m_centralLayout);

    connect(headerWidget, &ModuleWidgetHeader::clicked, this, &ModuleWidget::headerClicked);
    connect(this, &ModuleWidget::objectNameChanged, [this] {
        m_moduleIcon->setObjectName(QString(ObjectNameTemplateIcon).arg(objectName()));
    });
}

const QString ModuleWidget::title() const
{
    return m_moduleTitle->text();
}

void ModuleWidget::setTitle(const QString &title)
{
    m_moduleTitle->setText(title);

    setAccessibleName(title);
}

bool ModuleWidget::event(QEvent *event)
{
    if (event->type() == QEvent::LayoutRequest)
        setFixedHeight(m_centralLayout->sizeHint().height());

    return QWidget::event(event);
}

}
