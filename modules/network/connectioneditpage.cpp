/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
#include "passwdeditwidget.h"

#include <QTimer>
#include <QVBoxLayout>
#include <QPushButton>
#include <QJsonDocument>
#include <QJsonArray>
#include <QComboBox>
#include <QApplication>
#include <QFileDialog>
#include <QProcess>
#include <QDebug>

#include <cstring>

#include <dspinbox.h>

using namespace dcc::widgets;
using namespace dcc::network;

DWIDGET_USE_NAMESPACE

void processConfigCA(const QString &file)
{
    QFile f(file);
    f.open(QIODevice::ReadWrite);
    const QString data = f.readAll();
    f.seek(0);

    const QRegularExpression regex("^(?:ca\\s'(.+)'\\s*)$");
//    const QRegularExpression regex("^(?:ca\\s'(.+)'\\s*|CACert=(.+)|UserCertificate=(.+))$");
    QStringList ca_list;
    for (const auto &line : data.split('\n'))
    {
        const auto match = regex.match(line);
        if (match.hasMatch())
        {
            for (int i(1); i != match.capturedLength(); ++i)
            {
                const auto cap = match.captured(i);
                if (cap.isNull() || cap.isEmpty())
                    continue;
                ca_list << cap;
            }
        } else {
            f.write(line.toStdString().c_str());
            f.write("\n");
        }
    }
    f.write("\n");

    if (!ca_list.isEmpty())
    {
        // write ca
        f.write("<ca>\n");
        for (const auto ca : ca_list)
        {
            QFile caf(ca);
            caf.open(QIODevice::ReadOnly);
            f.write(caf.readAll());
            f.write("\n");
        }
        f.write("</ca>\n");
    }

    f.flush();
    f.close();
}

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
      m_exportBtn(new QPushButton),
      m_buttonTuple(new ButtonTuple),
      m_sectionsLayout(new QVBoxLayout),

      m_recreateUITimer(new QTimer(this))
{
    m_sectionsLayout->setSpacing(10);

    m_disconnectBtn->setText(tr("Disconnect"));
    m_disconnectBtn->setVisible(false);
    m_removeBtn->setText(tr("Delete"));
    m_removeBtn->setVisible(false);
    m_exportBtn->setText(tr("Export"));
    m_exportBtn->setVisible(false);

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
    mainLayout->addWidget(m_exportBtn);
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
    connect(m_exportBtn, &QPushButton::clicked, this, &ConnectionEditPage::exportConnConfig);
    connect(m_disconnectBtn, &QPushButton::clicked, this, [this] { emit requestDisconnect(m_sessionModel->uuid()); });
    connect(m_removeBtn, &QPushButton::clicked, this, [this] { emit requestRemove(m_sessionModel->uuid()); });
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
    connect(m_sessionModel, &ConnectionSessionModel::uuidChanged, this, &ConnectionEditPage::onActiveStateChanged);
    connect(m_sessionModel, &ConnectionSessionModel::typeChanged, this, &ConnectionEditPage::onTypeChanged);

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

void ConnectionEditPage::exportConnConfig()
{
    Q_ASSERT_X(m_sessionModel->type().startsWith("vpn"), Q_FUNC_INFO, "only vpn connection export is supported.");

    const QString uuid = m_sessionModel->uuid();

    emit requestFrameKeepAutoHide(false);
    const QUrl u = QFileDialog::getSaveFileUrl(nullptr, QString(), QUrl(), "Config File (*.conf)");
    emit requestFrameKeepAutoHide(true);

    if (u.isEmpty() || !u.isLocalFile())
        return;

    QString file = u.path();
    if (!file.endsWith(".conf"))
        file.append(".conf");

    const auto args = QStringList() << "connection" << "export" << uuid << file;
    qDebug() << Q_FUNC_INFO << args;

    QProcess p;
    p.start("nmcli", args);
    p.waitForFinished();
    qDebug() << p.readAllStandardOutput();
    qDebug() << p.readAllStandardError();

    // process ca
    processConfigCA(file);
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

///
/// \brief ConnectionEditPage::onTypeChanged
/// \param type VPN type
///
/// if editing connection is VPN connection. Need to
/// add export button.
///
void ConnectionEditPage::onTypeChanged(const QString &type)
{
    if (!type.startsWith("vpn"))
        return;

    const QStringList suppertedList = { "vpn-l2tp", "vpn-openvpn" };

    if (suppertedList.contains(type))
        m_exportBtn->setVisible(true);
}

void ConnectionEditPage::onActiveStateChanged()
{
    const bool connected = m_networkModel->activeConnections().contains(m_sessionModel->uuid());

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
    connect(w, &SwitchWidget::checkedChanged, this, [=](const bool checked) {
        emit requestChangeSettings(section, vKey, checked ? "true" : "false");
    });

    return w;
}

SettingsItem *ConnectionEditPage::createEditWidget(const QJsonObject &keyObject, const QJsonObject &infoObject, const bool password)
{
    PasswdEditWidget *w = new PasswdEditWidget;

    QLineEdit *e = w->textEdit();

    w->setTitle(keyObject.value("Name").toString());
    e->setText(infoObject.value("Value").toString());
    e->setReadOnly(keyObject.value("Readonly").toBool());
    if (password)
        e->setEchoMode(QLineEdit::Password);

    const bool isHostapd = m_sessionModel->type() == "wireless-hotspot";

    if (isHostapd && password)
        w->setSwitchBtnVisible(true);
    else
        w->setSwitchBtnVisible(false);

    const QString section = keyObject.value("Section").toString();
    const QString vKey = keyObject.value("Key").toString();
    connect(e, &QLineEdit::editingFinished, this, [=] {
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
    connect(w, &ComboBoxWidget::dataChanged, this, [=](const QVariant &data) {
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
    connect(w->spinBox(), static_cast<void (DSpinBox::*)(int)>(&DSpinBox::valueChanged), this, [=](const int value) {
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
