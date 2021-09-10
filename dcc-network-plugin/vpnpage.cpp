/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *             listenerri <listenerri@gmail.com>
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

#include "vpnpage.h"
#include "connectionvpneditpage.h"
#include "connectionpageitem.h"
#include "widgets/contentwidget.h"
#include "widgets/switchwidget.h"
#include "widgets/titlelabel.h"
#include "widgets/translucentframe.h"
#include "widgets/optionitem.h"
#include "widgets/nextpagewidget.h"
#include "widgets/loadingnextpagewidget.h"
#include "window/utils.h"
#include "window/gsettingwatcher.h"

#include <DFloatingButton>
#include <DHiDPIHelper>
#include <ddialog.h>
#include <networkmanagerqt/settings.h>
#include <DSpinner>

#include <QDebug>
#include <QList>
#include <QMap>
#include <QVBoxLayout>
#include <QPushButton>
#include <QJsonObject>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QRegularExpression>
#include <QStandardItemModel>

#include <networkcontroller.h>
#include <vpncontroller.h>

DWIDGET_USE_NAMESPACE

using namespace dcc;
using namespace dcc::widgets;
using namespace dde::network;
using namespace NetworkManager;

QString vpnConfigType(const QString &path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly))
        return QString();

    const QString content = f.readAll();
    f.close();

    if (content.contains("openconnect"))
        return "openconnect";
    if (content.contains("l2tp"))
        return "l2tp";
    if (content.startsWith("[main]"))
        return "vpnc";

    return "openvpn";
}

VpnPage::VpnPage(QWidget *parent)
    : QWidget(parent)
    , m_lvprofiles(new DListView(this))
    , m_modelprofiles(new QStandardItemModel(this))
    , m_importFile(new QFileDialog(this))
{
    m_lvprofiles->setAccessibleName("List_vpnList");
    m_lvprofiles->setModel(m_modelprofiles);
    m_lvprofiles->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_lvprofiles->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    m_lvprofiles->setSelectionMode(QAbstractItemView::NoSelection);

    m_importFile->setAccessibleName("VpnPage_importFile");
    m_importFile->setModal(true);
    m_importFile->setNameFilter("*.conf");
    m_importFile->setAcceptMode(QFileDialog::AcceptOpen);
    QStringList directory = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    if (!directory.isEmpty()) {
        m_importFile->setDirectory(directory.first());
    }

    QWidget *widget = new QWidget(this);
    QVBoxLayout *scrollLayout = new QVBoxLayout(widget);

    QLabel *lblTitle = new QLabel(tr("VPN Status"), widget);
    DFontSizeManager::instance()->bind(lblTitle, DFontSizeManager::T5, QFont::DemiBold);
    m_vpnSwitch = new SwitchWidget(widget, lblTitle);
    m_vpnSwitch->switchButton()->setAccessibleName(lblTitle->text());
    //因为swtichbutton内部距离右间距为4,所以这里设置6就可以保证间距为10
    m_vpnSwitch->getMainLayout()->setContentsMargins(10, 0, 6, 0);

    scrollLayout->addWidget(m_vpnSwitch, 0, Qt::AlignTop);
    // 控制lisview字体间距
    QMargins itemMargins(m_lvprofiles->itemMargins());
    itemMargins.setLeft(2);
    m_lvprofiles->setItemMargins(itemMargins);
    scrollLayout->addWidget(m_lvprofiles);
    scrollLayout->setSpacing(10);
    scrollLayout->setContentsMargins(QMargins(10, 0, 10, 0)); //设置左右间距为10

    widget->setLayout(scrollLayout);

    ContentWidget *contentWidget = new ContentWidget(this);
    contentWidget->setAccessibleName("VpnPage_ContentWidget");
    contentWidget->layout()->setMargin(0);
    contentWidget->setContent(widget);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->addWidget(contentWidget);

    QHBoxLayout *buttonsLayout = new QHBoxLayout(this);
    buttonsLayout->setSpacing(30);
    buttonsLayout->addStretch();

    //~ contents_path /network/VPN/Create VPN
    DFloatingButton *createVpnBtn = new DFloatingButton(DStyle::StandardPixmap::SP_IncreaseElement, this);
    createVpnBtn->setMinimumSize(QSize(47, 47));
    createVpnBtn->setToolTip(tr("Create VPN"));
    createVpnBtn->setAccessibleName(tr("Create VPN"));
    GSettingWatcher::instance()->bind("createVpn", createVpnBtn);
    buttonsLayout->addWidget(createVpnBtn);

    //~ contents_path /network/VPN/Import VPN
    DFloatingButton *importVpnBtn = new DFloatingButton("\342\206\223", this);
    importVpnBtn->setMinimumSize(QSize(47, 47));
    importVpnBtn->setToolTip(tr("Import VPN"));
    importVpnBtn->setAccessibleName(tr("Import VPN"));
    GSettingWatcher::instance()->bind("importVpn", importVpnBtn);
    buttonsLayout->addWidget(importVpnBtn);
    buttonsLayout->addStretch();
    mainLayout->addLayout(buttonsLayout);

    setContentsMargins(0, 10, 0, 10);  //设置上下间距为10
    setLayout(mainLayout);

    VPNController *vpnController = NetworkController::instance()->vpnController();

    connect(m_lvprofiles, &DListView::clicked, [ = ] (const QModelIndex &index) {
        QString uuid = index.data(UuidRole).toString();
        vpnController->connectItem(uuid);
    });
    connect(m_vpnSwitch, &SwitchWidget::checkedChanged, [ = ](const bool checked) {
        vpnController->setEnabled(checked);
    });

    connect(createVpnBtn, &QPushButton::clicked, this, &VpnPage::createVPN);
    connect(importVpnBtn, &QPushButton::clicked, this, &VpnPage::importVPN);
    connect(m_importFile, &QFileDialog::finished, this, [ = ] (int result) {
        Q_EMIT requestFrameKeepAutoHide(true);
        if (result == QFileDialog::Accepted) {
            QString file = m_importFile->selectedFiles().first();
            if (file.isEmpty())
                return;

            const auto args = QStringList { "connection", "import", "type", vpnConfigType(file), "file", file };

            QProcess p;
            p.start("nmcli", args);
            p.waitForFinished();
            const auto stat = p.exitCode();
            const QString output = p.readAllStandardOutput();
            QString error = p.readAllStandardError();

            qDebug() << stat << ",output:" << output << ",err:" << error;

            if (stat) {
                const auto ratio = devicePixelRatioF();
                QPixmap icon = QIcon::fromTheme("dialog-error").pixmap(QSize(48, 48) * ratio);
                icon.setDevicePixelRatio(ratio);

                DDialog dialog(this);
                dialog.setTitle(tr("Import Error"));
                dialog.setMessage(tr("File error"));
                dialog.addButton(tr("OK"));
                dialog.setIcon(icon);
                dialog.exec();
                return;
            }

            const QRegularExpression regexp("\\(\\w{8}(-\\w{4}){3}-\\w{12}\\)");
            const auto match = regexp.match(output);

            if (match.hasMatch()) {
                m_editingConnUuid = match.captured();
                m_editingConnUuid.replace("(", "");
                m_editingConnUuid.replace(")", "");
                qDebug() << "editing connection Uuid";
                QTimer::singleShot(10, this, &VpnPage::changeVpnId);
            }
        }
    });

    connect(vpnController, &VPNController::enableChanged, m_vpnSwitch, &SwitchWidget::setChecked);
    connect(vpnController, &VPNController::activeConnectionChanged, this, &VpnPage::onActiveConnsInfoChanged);
    connect(vpnController, &VPNController::itemChanged, this, &VpnPage::updateVpnItems);
    connect(vpnController, &VPNController::itemAdded, [ = ] {
        refreshVpnList(vpnController->items());
    });
    connect(vpnController, &VPNController::itemRemoved, [ = ] {
        refreshVpnList(vpnController->items());
    });

    m_vpnSwitch->setChecked(vpnController->enabled());

    refreshVpnList(vpnController->items());
}

VpnPage::~VpnPage()
{
    if (m_importFile)
        m_importFile->deleteLater();

    GSettingWatcher::instance()->erase("createVpn");
    GSettingWatcher::instance()->erase("importVpn");

    if (!m_editPage.isNull())
        m_editPage->deleteLater();
}

void VpnPage::refreshVpnList(QList<VPNItem *> vpns)
{
    m_modelprofiles->clear();

    for (VPNItem *vpn : vpns) {
        const QString uuid = vpn->connection()->uuid();

        ConnectionPageItem *vpnItem = new ConnectionPageItem(this, m_lvprofiles, vpn->connection());
        vpnItem->setText(vpn->connection()->id());
        vpnItem->setItemData(vpn);
        vpnItem->setData(uuid, UuidRole);

        connect(vpnItem, &ConnectionPageItem::detailClick, [ = ] {
            m_editPage = new ConnectionVpnEditPage(uuid);
            m_editPage->initSettingsWidget();
            connect(m_editPage, &ConnectionVpnEditPage::requestNextPage, this, &VpnPage::requestNextPage);
            connect(m_editPage, &ConnectionVpnEditPage::requestFrameAutoHide, this, &VpnPage::requestFrameKeepAutoHide);
            Q_EMIT requestNextPage(m_editPage);
        });

        m_modelprofiles->appendRow(vpnItem);
    }

    //onActiveConnsInfoChanged(m_model->activeConnInfos());
    m_vpnSwitch->setVisible(m_modelprofiles->rowCount() > 0);

    // 延迟刷新，是为了显示正常
    QTimer::singleShot(100, [ = ] {
        onActiveConnsInfoChanged();
    });
}

void VpnPage::updateVpnItems(const QList<VPNItem *> &vpns)
{
    for (int i = 0; i < m_modelprofiles->rowCount(); i++) {
        ConnectionPageItem *pageItem = static_cast<ConnectionPageItem *>(m_modelprofiles->item(i));
        VPNItem *vpn = static_cast<VPNItem *>(pageItem->itemData());
        if (vpns.contains(vpn))
            pageItem->setText(vpn->connection()->id());
    }
}

void VpnPage::onActiveConnsInfoChanged()
{
    for (int i = 0; i < m_modelprofiles->rowCount(); ++i) {
        ConnectionPageItem *item = static_cast<ConnectionPageItem *>(m_modelprofiles->item(i));
        VPNItem *vpnItem = static_cast<VPNItem *>(item->itemData());
        if (!vpnItem)
            continue;

        item->setConnectionStatus(vpnItem->connectionStatus());
    }
}

void VpnPage::changeVpnId()
{
    NetworkManager::Connection::List connList = listConnections();
    QString importName = "";
    for (const auto &conn : connList) {
        if (conn->settings()->connectionType() == ConnectionSettings::Vpn) {
            if (m_editingConnUuid == conn->uuid()) {
                importName = conn->name();
                break;
            }
        }
    }
    if (importName.isEmpty()) {
        QTimer::singleShot(10, this, &VpnPage::changeVpnId);
        return;
    }

    QString changeName = "";
    bool hasSameName = false;
    for (const auto &conn : connList) {
        const QString vpnName = conn->name();
        const QString vpnUuid = conn->uuid();
        if ((vpnName == importName) && (vpnUuid != m_editingConnUuid)) {
            changeName = importName + "(1)";
            hasSameName = true;
            break;
        }
    }
    if (!hasSameName) {
        return;
    }

    for (int index = 1; ; index++) {
        hasSameName = false;
        for (const auto &conn : connList) {
            QString vpnName = conn->name();
            if (vpnName == changeName) {
                changeName = importName + "(%1)";
                changeName = changeName.arg(index);
                hasSameName = true;
                break;
            }
        }
        if (!hasSameName) {
            break;
        }
    }

    NetworkManager::Connection::Ptr uuidConn = findConnectionByUuid(m_editingConnUuid);
    if (uuidConn) {
        ConnectionSettings::Ptr connSettings = uuidConn->settings();
        connSettings->setId(changeName);
        // update function saves the settings on the hard disk
        QDBusPendingReply<> reply = uuidConn->update(connSettings->toMap());
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug() << "error occurred while updating the connection" << reply.error();
            return;
        }
        qDebug() << "find Connection By Uuid is success";
        return;
    }
}

void VpnPage::importVPN()
{
    Q_EMIT requestFrameKeepAutoHide(false);
    m_importFile->show();
}

void VpnPage::createVPN()
{
    m_editPage = new ConnectionVpnEditPage("", this);
    m_editPage->initSettingsWidgetByType(ConnectionVpnEditPage::VpnType::UNSET);
    connect(m_editPage, &ConnectionVpnEditPage::requestNextPage, this, &VpnPage::requestNextPage);
    connect(m_editPage, &ConnectionVpnEditPage::requestFrameAutoHide, this, &VpnPage::requestFrameKeepAutoHide);
    Q_EMIT requestNextPage(m_editPage);

    //only create New Connection can set "Cancel","Save" button
    //fix: https://pms.uniontech.com/zentao/bug-view-61563.html
    m_editPage->setButtonTupleEnable(true);
}

void VpnPage::jumpPath(const QString &searchPath)
{
    if (searchPath == "Create VPN")
        createVPN();

    if (searchPath == "Import VPN")
        importVPN();
}
