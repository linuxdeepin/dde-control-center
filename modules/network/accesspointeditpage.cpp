#include "accesspointeditpage.h"
#include "connectionsessionmodel.h"
#include "translucentframe.h"
#include "settingsgroup.h"
#include "settingsheaderitem.h"
#include "nextpagewidget.h"
#include "switchwidget.h"
#include "lineeditwidget.h"

#include <QDebug>
#include <QTimer>
#include <QVBoxLayout>
#include <QPushButton>

using namespace dcc::widgets;
using namespace dcc::network;

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

    for (auto *w : m_sectionWidgets.values())
        if (w)
            w->deleteLater();
    for  (const auto v : m_optionWidgets.values())
        for (auto *w : v)
            if (w)
                w->deleteLater();
}

void AccessPointEditPage::setModel(ConnectionSessionModel *model)
{
    m_sessionModel = model;

    connect(m_sessionModel, &ConnectionSessionModel::keysChanged, m_recreateUITimer, static_cast<void (QTimer::*)()>(&QTimer::start));
    connect(m_sessionModel, &ConnectionSessionModel::visibleItemsChanged, this, &AccessPointEditPage::refershUI);

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
        grp->headerItem()->setTitle(m_sessionModel->sectionName(it.key()));

        m_sectionWidgets[it.key()] = grp;

        const auto keys = it.value();
        for (auto its(keys.cbegin()); its != keys.cend(); ++its)
            createOptionWidgets(it.key(), its.value());
    }

    refershUI();
}

void AccessPointEditPage::refershUI()
{
    // hide all widgets
    while (QLayoutItem *item = m_sectionsLayout->takeAt(0))
    {
        SettingsGroup *grp = qobject_cast<SettingsGroup *>(item->widget());
        Q_ASSERT(grp);

        grp->hide();
        grp->clear();

        delete item;
    }

    for (const auto section : m_sessionModel->sections())
    {
        SettingsGroup *grp = m_sectionWidgets.value(section);
        if (!grp)
            continue;

        // add section widgets
        const auto visibleKeys = m_sessionModel->sectionKeys(section);
        for (const auto &vKey : visibleKeys)
        {
            SettingsItem *item = m_optionWidgets[section][vKey.value("Key").toString()];
            if (item)
                grp->appendItem(item);
        }


        m_sectionsLayout->addWidget(grp);
    }
}

void AccessPointEditPage::createOptionWidgets(const QString &section, const QJsonObject &keyObject)
{
    const QString vKey = keyObject.value("Key").toString();
    const QString vType = keyObject.value("WidgetType").toString();
    const QJsonObject vInfo = m_sessionModel->keysInfo(section, vKey);

    // delete old widgets
    if (m_optionWidgets[section][vKey] != nullptr)
    {
        m_optionWidgets[section][vKey]->deleteLater();
        m_optionWidgets[section].remove(vKey);
    }

    SettingsItem *item = nullptr;
    if (vType == "EditLineSwitchButton")
        item = createSwitchWidget(keyObject, vInfo);
    else if (vType == "EditLineTextInput")
        item = createEditWidget(keyObject, vInfo);

    if (item)
        m_optionWidgets[section][vKey] = item;
    else
        qWarning() << "type not handled: " << keyObject << vInfo;
}

SettingsItem *AccessPointEditPage::createSwitchWidget(const QJsonObject &keyObject, const QJsonObject &infoObject)
{
    SwitchWidget *w = new SwitchWidget;

    w->setTitle(keyObject.value("Name").toString());
    w->setChecked(infoObject.value("Value").toBool());

    return w;
}

SettingsItem *AccessPointEditPage::createEditWidget(const QJsonObject &keyObject, const QJsonObject &infoObject)
{
    LineEditWidget *w = new LineEditWidget;

    w->setTitle(keyObject.value("Name").toString());
    w->textEdit()->setText(infoObject.value("Value").toString());

    return w;
}
