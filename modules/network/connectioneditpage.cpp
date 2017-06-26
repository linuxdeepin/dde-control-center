#include "connectioneditpage.h"
#include "connectionsessionmodel.h"
#include "translucentframe.h"
#include "settingsgroup.h"
#include "settingsheaderitem.h"
#include "nextpagewidget.h"
#include "switchwidget.h"
#include "lineeditwidget.h"
#include "comboboxwidget.h"
#include "filechoosewidget.h"
#include "spinboxwidget.h"
#include "networkmodel.h"
#include "buttontuple.h"

#include <dspinbox.h>

#include <QDebug>
#include <QTimer>
#include <QVBoxLayout>
#include <QPushButton>
#include <QJsonDocument>
#include <QJsonArray>
#include <QComboBox>
#include <QApplication>

#include <cstring>

using namespace dcc::widgets;
using namespace dcc::network;

DWIDGET_USE_NAMESPACE

const QString JsonEncoding(const QString &str)
{
    QJsonObject o;
    o["v"] = str;
    QJsonDocument d;
    d.setObject(o);

    const QString encoding = d.toJson(QJsonDocument::Compact);

    return encoding.mid(5, encoding.size() - 5 + 1 - 2);
}

ConnectionEditPage::ConnectionEditPage(QWidget *parent)
    : ContentWidget(parent),

      m_disconnectBtn(new QPushButton),
      m_removeBtn(new QPushButton),
      m_buttonTuple(new ButtonTuple),
      m_sectionsLayout(new QVBoxLayout),

      m_recreateUITimer(new QTimer(this))
{
    m_sectionsLayout->setSpacing(10);

    m_disconnectBtn->setText(tr("Disconnect"));
    m_disconnectBtn->setVisible(false);
    m_removeBtn->setText(tr("Delete"));
    m_removeBtn->setVisible(false);

    m_recreateUITimer->setSingleShot(true);
    m_recreateUITimer->setInterval(100);

    QPushButton *cancelBtn = m_buttonTuple->leftButton();
    QPushButton *acceptBtn = m_buttonTuple->rightButton();
    cancelBtn->setText(tr("Cancel"));
    acceptBtn->setText(tr("Save"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addSpacing(10);
    mainLayout->addWidget(m_disconnectBtn);
    mainLayout->addWidget(m_removeBtn);
    mainLayout->addLayout(m_sectionsLayout);
    mainLayout->addWidget(m_buttonTuple);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *mainWidget = new TranslucentFrame;
    mainWidget->setLayout(mainLayout);

    setContent(mainWidget);

    connect(m_recreateUITimer, &QTimer::timeout, this, &ConnectionEditPage::recreateUI);
    connect(cancelBtn, &QPushButton::clicked, this, &ConnectionEditPage::back);
    connect(acceptBtn, &QPushButton::clicked, this, &ConnectionEditPage::accept);
    connect(m_disconnectBtn, &QPushButton::clicked, this, [this] { emit requestDisconnect(m_sessionModel->connectionUuid()); });
    connect(m_removeBtn, &QPushButton::clicked, this, [this] { emit requestRemove(m_sessionModel->connectionUuid()); });
    connect(m_removeBtn, &QPushButton::clicked, this, &ConnectionEditPage::back);
    connect(this, &ConnectionEditPage::requestNextPage, [=](ContentWidget *w) { m_nextPage = w; });
}

ConnectionEditPage::~ConnectionEditPage()
{
    emit requestCancelSession();

    for  (const auto &v : m_optionWidgets.values())
        for (auto *w : v)
            if (w)
                w->deleteLater();
    for (auto *w : m_sectionWidgets.values())
        if (w)
            w->deleteLater();
}

void ConnectionEditPage::setModel(NetworkModel *networkModel, ConnectionSessionModel *sessionModel)
{
    m_networkModel = networkModel;
    m_sessionModel = sessionModel;

    connect(m_networkModel, &NetworkModel::activeConnectionsChanged, this, &ConnectionEditPage::onActiveStateChanged);
    connect(m_sessionModel, &ConnectionSessionModel::keysChanged, m_recreateUITimer, static_cast<void (QTimer::*)()>(&QTimer::start));
    connect(m_sessionModel, &ConnectionSessionModel::deletableChanged, m_removeBtn, &QPushButton::setVisible);
    connect(m_sessionModel, &ConnectionSessionModel::visibleItemsChanged, this, &ConnectionEditPage::refershUI);
    connect(m_sessionModel, &ConnectionSessionModel::errorsChanged, this, &ConnectionEditPage::onErrorsChanged);
    connect(m_sessionModel, &ConnectionSessionModel::saveFinished, this, &ConnectionEditPage::saveFinished);
    connect(m_sessionModel, &ConnectionSessionModel::connectionUuidChanged, this, &ConnectionEditPage::onActiveStateChanged);

    m_removeBtn->setVisible(m_sessionModel->deletable());
    onActiveStateChanged();

    m_recreateUITimer->start();
}

void ConnectionEditPage::onDeviceRemoved()
{
    if (!m_nextPage.isNull())
        emit m_nextPage->back();

    emit back();
}

void ConnectionEditPage::recreateUI()
{
    // construct new widgets
    const auto keys = m_sessionModel->vkList();
    for (auto it(keys.cbegin()); it != keys.cend(); ++it)
    {
        if (!m_sectionWidgets.contains(it.key()))
            m_sectionWidgets[it.key()] = new SettingsGroup;

        SettingsGroup *grp = m_sectionWidgets[it.key()];
        grp->setVisible(false);
        grp->setHeaderVisible(true);
        grp->headerItem()->setTitle(m_sessionModel->sectionName(it.key()));
    }

    refershUI();
    initPlaceholderText(m_sessionModel->errors());
}

void ConnectionEditPage::refershUI()
{
    const QString name = m_sessionModel->keysInfo("vs-general", "id").value("Value").toString();
    setTitle(name);

    QWidget *fw = qApp->focusWidget();

    // hide all widgets
    for (auto *grp : m_sectionWidgets)
        grp->clear();

    for (const auto section : m_sessionModel->sections())
    {
        SettingsGroup *grp = m_sectionWidgets.value(section);
        if (!grp)
            continue;

        // add section widgets
        const auto visibleKeys = m_sessionModel->sectionKeys(section);
        for (const auto &vKey : visibleKeys)
        {
            const QString vk = vKey.value("Key").toString();
            SettingsItem *item = optionWidgets(section, m_sessionModel->vkInfo(section, vk));

            grp->appendItem(item);
        }

        m_sectionsLayout->addWidget(grp);
        grp->setVisible(true);
    }

    if (fw)
        fw->setFocus();
}

void ConnectionEditPage::saveFinished(const bool ret)
{
    if (ret)
        emit back();
}

void ConnectionEditPage::initPlaceholderText(const NetworkErrors &errors)
{
    for (auto it(errors.cbegin()); it != errors.cend(); ++it)
    {
        const auto section = m_sessionModel->virtualSectionName(it.key());
        const auto eItems = it.value();
        for (auto its(eItems.begin()); its != eItems.end(); ++its)
        {
            if (m_optionWidgets[section].contains(its.key()))
                setItemRequired(m_optionWidgets[section][its.key()]);
        }
    }
}

void ConnectionEditPage::onErrorsChanged(const NetworkErrors &errors)
{
    // clear old errors
    for (const auto &v : m_optionWidgets)
        for (auto *i : v)
            i->setIsErr(false);

    for (auto it(errors.cbegin()); it != errors.cend(); ++it)
    {
        const auto section = m_sessionModel->virtualSectionName(it.key());
        const auto eItems = it.value();
        for (auto its(eItems.begin()); its != eItems.end(); ++its)
        {
            if (m_optionWidgets[section].contains(its.key()))
            {
                SettingsItem *item = m_optionWidgets[section][its.key()];
                setItemRequired(item);
                item->setIsErr();
            }
        }
    }
}

void ConnectionEditPage::onActiveStateChanged()
{
    const bool connected = m_networkModel->activeConnections().contains(m_sessionModel->connectionUuid());

//    qDebug() << m_networkModel->activeConnections() << m_sessionModel->connectionUuid();

    m_disconnectBtn->setVisible(connected);
}

SettingsItem *ConnectionEditPage::optionWidgets(const QString &section, const QJsonObject &keyObject)
{
    const QString vKey = keyObject.value("Key").toString();
    const QString vType = keyObject.value("WidgetType").toString();
    const QJsonObject vInfo = m_sessionModel->keysInfo(section, vKey);

    // delete old widgets
    if (m_optionWidgets[section][vKey] != nullptr)
    {
        SettingsItem *item = m_optionWidgets[section][vKey];

        // TODO: update exist widgets value
        if (vType == "EditLineSpinner")
            updateSpinBoxWidget(item, vInfo);

        return item;
    }

    SettingsItem *item = nullptr;
    if (vType == "EditLineSwitchButton")
        item = createSwitchWidget(keyObject, vInfo);
    else if (vType == "EditLineTextInput" || vType == "EditLineIpv4Input" || vType == "EditLineMissingPackage")
        item = createEditWidget(keyObject, vInfo, false);
    else if (vType == "EditLinePasswordInput")
        item = createEditWidget(keyObject, vInfo, true);
    else if (vType == "EditLineComboBox")
        item = createComboWidget(keyObject, vInfo, false);
    else if (vType == "EditLineEditComboBox")
        item = createComboWidget(keyObject, vInfo, true);
    else if (vType == "EditLineFileChooser")
        item = createFileChooserWidget(keyObject, vInfo);
    else if (vType == "EditLineSpinner")
        item = createSpinBoxWidget(keyObject, vInfo);

    if (!item)
        qCritical() << "type not handled: " << keyObject << vInfo;

    m_optionWidgets[section][vKey] = item;
    return item;
}

SettingsItem *ConnectionEditPage::createSwitchWidget(const QJsonObject &keyObject, const QJsonObject &infoObject)
{
    SwitchWidget *w = new SwitchWidget;

    w->setTitle(keyObject.value("Name").toString());
    w->setChecked(infoObject.value("Value").toBool());

    const QString section = keyObject.value("Section").toString();
    const QString vKey = keyObject.value("Key").toString();
    connect(w, &SwitchWidget::checkedChanged, [=](const bool checked) {
        emit requestChangeSettings(section, vKey, checked ? "true" : "false");
    });

    return w;
}

SettingsItem *ConnectionEditPage::createEditWidget(const QJsonObject &keyObject, const QJsonObject &infoObject, const bool password)
{
    LineEditWidget *w = new LineEditWidget;
    QLineEdit *e = w->textEdit();

    w->setTitle(keyObject.value("Name").toString());
    e->setText(infoObject.value("Value").toString());
    e->setReadOnly(keyObject.value("Readonly").toBool());
    if (password)
        e->setEchoMode(QLineEdit::Password);

    const QString section = keyObject.value("Section").toString();
    const QString vKey = keyObject.value("Key").toString();
    connect(e, &QLineEdit::editingFinished, [=] {
        emit requestChangeSettings(section, vKey, JsonEncoding(e->text()));
    });

    if (m_sessionModel->errors()[section].contains(vKey))
        w->setPlaceholderText(tr("Required"));

    return w;
}

SettingsItem *ConnectionEditPage::createComboWidget(const QJsonObject &keyObject, const QJsonObject &infoObject, const bool editable)
{
    ComboBoxWidget *w = new ComboBoxWidget;

    for (const auto &v : infoObject.value("Values").toArray())
    {
        const auto vObject = v.toObject();
        const auto value = vObject.value("Value").toVariant();
        w->appendOption(vObject.value("Text").toString(), value);
    }

    w->setEditable(editable);
    w->setTitle(keyObject.value("Name").toString());
    w->setCurrent(infoObject.value("Value"));

    const QString section = keyObject.value("Section").toString();
    const QString vKey = keyObject.value("Key").toString();
    connect(w, &ComboBoxWidget::requestPage, this, &ConnectionEditPage::requestNextPage);
    connect(w, &ComboBoxWidget::dataChanged, [=](const QVariant &data) {
        if (!std::strcmp(data.typeName(), "double"))
            emit requestChangeSettings(section, vKey, data.toString());
        else
            emit requestChangeSettings(section, vKey, JsonEncoding(data.toString()));
    });

    return w;
}

SettingsItem *ConnectionEditPage::createFileChooserWidget(const QJsonObject &keyObject, const QJsonObject &infoObject)
{
    Q_UNUSED(infoObject);

    FileChooseWidget *w = new FileChooseWidget;

    connect(w, &FileChooseWidget::requestFrameKeepAutoHide, this, &ConnectionEditPage::requestFrameKeepAutoHide);

    w->setTitle(keyObject.value("Name").toString());
    w->edit()->setText(infoObject.value("Value").toString());

    const QString section = keyObject.value("Section").toString();
    const QString vKey = keyObject.value("Key").toString();
    connect(w->edit(), &QLineEdit::textChanged, [=] {
        emit requestChangeSettings(section, vKey, JsonEncoding(w->edit()->text()));
    });

    return w;
}

SettingsItem *ConnectionEditPage::createSpinBoxWidget(const QJsonObject &keyObject, const QJsonObject &infoObject)
{
    SpinBoxWidget *w = new SpinBoxWidget;

    w->setTitle(keyObject.value("Name").toString());
    w->spinBox()->setMinimum(keyObject.value("MinValue").toInt());
    w->spinBox()->setMaximum(keyObject.value("MaxValue").toInt());
    w->spinBox()->setValue(infoObject.value("Value").toInt());

    const QString section = keyObject.value("Section").toString();
    const QString vKey = keyObject.value("Key").toString();
    connect(w->spinBox(), static_cast<void (DSpinBox::*)(int)>(&DSpinBox::valueChanged), [=](const int value) {
        emit requestChangeSettings(section, vKey, QString::number(value));
    });

    return w;
}

void ConnectionEditPage::updateSpinBoxWidget(SettingsItem *item, const QJsonObject &infoObject)
{
    SpinBoxWidget *w = static_cast<SpinBoxWidget *>(item);

    w->spinBox()->setValue(infoObject.value("Value").toInt());
}

void ConnectionEditPage::setItemRequired(SettingsItem *item)
{
    LineEditWidget *edit = qobject_cast<LineEditWidget *>(item);
    if (edit && edit->text().isEmpty())
        edit->textEdit()->setPlaceholderText(tr("Required"));
}
