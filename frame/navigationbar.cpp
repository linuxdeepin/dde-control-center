#include "navigationbar.h"

#include <QVBoxLayout>
#include <QDebug>
#include <QSignalMapper>

static const QStringList ModuleList = { "accounts",
                                        "display",
                                        "defapp",
                                        "personalization",
                                        "network",
                                        "bluetooth",
                                        "sound",
                                        "datetime",
                                        "power",
                                        "mouse",
                                        "keyboard",
                                        "wacom",
                                        "update",
                                        "systeminfo",
                                      };

NavigationBar::NavigationBar(QWidget *parent)
    : QWidget(parent)

{
    QVBoxLayout *buttonsLayout = new QVBoxLayout;
    buttonsLayout->setContentsMargins(0, 0, 0, 0);
    buttonsLayout->setSpacing(20);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addStretch();
    centralLayout->addLayout(buttonsLayout);
    centralLayout->addStretch();
    centralLayout->setContentsMargins(0, 0, 0, 0);
    centralLayout->setSpacing(20);

    setLayout(centralLayout);

    // init buttons
    QSignalMapper *moduleMapper = new QSignalMapper(this);

    for (const auto &module : ModuleList)
    {
        DImageButton *b = new DImageButton;
        b->setNormalPic(QString(":/%1/themes/dark/icons/nav_%1.png").arg(module));
        b->setCheckable(true);

        moduleMapper->setMapping(b, module);
        buttonsLayout->addWidget(b);
        m_navigationButtons.insert(module, b);

        connect(b, &DImageButton::clicked, moduleMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
    }

    connect(moduleMapper, static_cast<void (QSignalMapper::*)(const QString &)>(&QSignalMapper::mapped), this, &NavigationBar::requestModule);
}

void NavigationBar::setModuleVisible(const QString &module, bool visible)
{
    if (m_navigationButtons.contains(module))
        m_navigationButtons[module]->setVisible(visible);
}
