#include "modulewidget.h"

#include <QEvent>
#include <QDebug>

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
    titleLayout->addWidget(m_moduleIcon);
    titleLayout->setAlignment(m_moduleIcon, Qt::AlignCenter);
    titleLayout->addWidget(m_moduleTitle);
    titleLayout->setSpacing(0);
    titleLayout->setMargin(0);

    m_centeralLayout = new QVBoxLayout;
    m_centeralLayout->addLayout(titleLayout);
    m_centeralLayout->setSpacing(8);
    m_centeralLayout->setMargin(0);

    setLayout(m_centeralLayout);

    connect(this, &ModuleWidget::objectNameChanged, [this] {
        m_moduleIcon->setObjectName(QString(ObjectNameTemplateIcon).arg(objectName()));
    });
}

void ModuleWidget::setTitle(const QString &title)
{
    m_moduleTitle->setText(title);

    setAccessibleName(title);
}

bool ModuleWidget::event(QEvent *event)
{
    if (event->type() == QEvent::LayoutRequest)
        setFixedHeight(m_centeralLayout->sizeHint().height());

    return QWidget::event(event);
}
