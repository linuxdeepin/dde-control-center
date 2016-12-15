#include "accesspointeditpage.h"
#include "connectionsessionmodel.h"
#include "translucentframe.h"
#include "settingsgroup.h"
#include "settingsheaderitem.h"

#include <QDebug>
#include <QTimer>
#include <QVBoxLayout>
#include <QPushButton>

using namespace dcc::widgets;
using namespace dcc::network;

const QString sectionHeader(const QString &section)
{
    if (section == "ipv4")
        return QStringLiteral("IPV4");
    if (section == "ipv6")
        return QStringLiteral("IPV6");

    return QString();
}

AccessPointEditPage::AccessPointEditPage(QWidget *parent)
    : ContentWidget(parent),

      m_cancelBtn(new QPushButton),
      m_acceptBtn(new QPushButton),
      m_sectionsLayout(new QVBoxLayout),

      m_recreateUITimer(new QTimer(this))
{
    m_sectionsLayout->setSpacing(10);

    m_cancelBtn->setText(tr("Cancel"));
    m_acceptBtn->setText(tr("Save"));

    m_recreateUITimer->setSingleShot(true);
    m_recreateUITimer->setInterval(100);

    QHBoxLayout *btnsLayout = new QHBoxLayout;
    btnsLayout->addWidget(m_cancelBtn);
    btnsLayout->addWidget(m_acceptBtn);
    btnsLayout->setSpacing(0);
    btnsLayout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(m_sectionsLayout);
    mainLayout->addLayout(btnsLayout);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *mainWidget = new TranslucentFrame;
    mainWidget->setLayout(mainLayout);

    setContent(mainWidget);

    connect(m_recreateUITimer, &QTimer::timeout, this, &AccessPointEditPage::recreateUI);
}

AccessPointEditPage::~AccessPointEditPage()
{
    emit requestCancelSession();
}

void AccessPointEditPage::setModel(ConnectionSessionModel *model)
{
    m_sessionModel = model;

    connect(m_sessionModel, &ConnectionSessionModel::keysChanged, m_recreateUITimer, static_cast<void (QTimer::*)()>(&QTimer::start));
    connect(m_sessionModel, &ConnectionSessionModel::sectionsChanged, this, &AccessPointEditPage::refershUI);

    m_recreateUITimer->start();
}

void AccessPointEditPage::onDeviceRemoved()
{
    emit back();
}

void AccessPointEditPage::recreateUI()
{
    // delete all widgets
    qDeleteAll(m_sectionWidgets.values());

    // construct new widgets
    const auto keys = m_sessionModel->keys();
    for (auto it(keys.cbegin()); it != keys.cend(); ++it)
    {
        SettingsGroup *grp = new SettingsGroup;
        grp->setHeaderVisible(true);
        grp->headerItem()->setTitle(it.key());

        m_sectionWidgets[it.key()] = grp;

        const auto keys = it.value();
        for (auto its(keys.cbegin()); its != keys.cend(); ++its)
        {
            qDebug() << its.key() << its.value();
        }
    }

    refershUI();
}

void AccessPointEditPage::refershUI()
{
    // hide all widgets
    while (QLayoutItem *item = m_sectionsLayout->takeAt(0))
    {
        item->widget()->hide();
        delete item;
    }

    for (const auto section : m_sessionModel->sections())
    {
        SettingsGroup *grp = m_sectionWidgets.value(section);
        if (!grp)
            continue;

        m_sectionsLayout->addWidget(grp);
    }
}
