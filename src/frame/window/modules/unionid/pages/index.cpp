/*
 * Copyright (C) 2017 ~ 2020 Deepin Technology Co., Ltd.
 *
 * Author:     liuxueming <liuxueming@uniontech.com>
 *
 * Maintainer: liuxueming <liuxueming@uniontech.com>
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

#include "index.h"
#include "modules/unionid/unionidmodel.h"
#include "widgets/settingsgroup.h"
#include "widgets/switchwidget.h"
#include "avatarwidget.h"
#include "downloadurl.h"
#include "window/utils.h"
#include "newstyle.h"
#include "syncitemwidget.h"
#include "../define.h"
#include "modules/unionid/httpclient.h"
#include "authenticationwindow.h"
#include "../notificationmanager.h"

#include <DWarningButton>
#include <DListView>
#include <DTipLabel>
#include <DCommandLinkButton>
#include <DFontSizeManager>

#include <QJsonObject>
#include <QJsonParseError>
#include <QScrollArea>
#include <QScrollBar>
#include <QScroller>
#include <QLabel>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QDebug>
#include <QDateTime>
#include <QMap>
#include <QDir>
#include <QSplitter>
#include <QDesktopServices>
#include <QDBusInterface>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::unionid;
using namespace dcc::widgets;
using namespace dcc::unionid;
using namespace dcc::cloudsync;

namespace DCC_NAMESPACE {
namespace unionid {

IndexPage::IndexPage(QWidget *parent)
    : LoginedIn(parent)
    , m_mainLayout(new QVBoxLayout)
    , m_listView(new DListView)
    , m_listModel(new QStandardItemModel(this))
{
    m_avatarWidget = new AvatarWidget;
    m_avatarWidget->setFixedSize(80, 80);
    m_avatarWidget->setAvatarPath(AvaterPath,false);

    m_nameLabel = new QLabel;
    QPalette nameLabelPa = m_nameLabel->palette();
    nameLabelPa.setColor(QPalette::Text,textTitleLightColor);
    m_nameLabel->setPalette(nameLabelPa);
    m_nameLabel->setContentsMargins(0,8,0,4);

    m_uidLabel = new QLabel;
    QPalette uidLabelPa = m_uidLabel->palette();
    uidLabelPa.setColor(QPalette::Text,textTipLightColor);
    m_uidLabel->setPalette(uidLabelPa);
    m_uidLabel->setContentsMargins(0,0,0,24);

    m_modifyInfoButton = new QPushButton(tr("Edit Profile"));
    newStyle *modifyInfoStyle = new newStyle(newStyle::TextRim);
    modifyInfoStyle->setTextColor(textTitleLightColor);
    modifyInfoStyle->setBorderColor(buttonNormalBorderLightColor,buttonHoverBorderLightColor,buttonPressBorderLightColor);
    m_modifyInfoButton->setStyle(modifyInfoStyle);
    m_modifyInfoButton->setMinimumSize(72,28);

    m_wxlabel = new QLabel;
    m_wxlabel->setText(tr("Link to WeChat"));
    m_wxlabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_wxNameLabel = new QLabel;
    QPalette pa = m_wxNameLabel->palette();
    m_wxNameLabel->setMinimumWidth(100);
    m_wxNameLabel->setAlignment(Qt::AlignCenter);
    pa.setColor(QPalette::Text,textTipLightColor);
    m_wxNameLabel->setPalette(pa);

    m_modButton = new DCommandLinkButton(tr(""));
    connect(m_modButton,&DCommandLinkButton::clicked,this,&IndexPage::onModButtonClicked);

    QHBoxLayout *bondWXHlayout = new QHBoxLayout;
    bondWXHlayout->addWidget(m_wxlabel);
    bondWXHlayout->addWidget(m_wxNameLabel);
    bondWXHlayout->addStretch();
    bondWXHlayout->addWidget(m_modButton);
    bondWXHlayout->setContentsMargins(0,0,0,0);

    QFrame *line1 = new QFrame;
    line1->setFrameShape(QFrame::HLine);
    line1->setMinimumSize(QSize(400,1));
    line1->setContentsMargins(0,0,0,0);

    QVBoxLayout *bondWXVlayout = new QVBoxLayout;
    bondWXVlayout->addLayout(bondWXHlayout);
    bondWXVlayout->addWidget(line1);
    bondWXVlayout->setContentsMargins(0,0,0,0);

    QWidget *bondWXWidget = new QWidget;
    bondWXWidget->setLayout(bondWXVlayout);
    bondWXWidget->setMinimumSize(QSize(400,56));
    bondWXWidget->setContentsMargins(0,0,0,0);

    QFrame *line2 = new QFrame;
    line2->setFrameShape(QFrame::HLine);
    line2->setMinimumSize(QSize(400,1));
    line2->setContentsMargins(0,0,0,0);
    QPalette line2Pa = line2->palette();
    line2Pa.setColor(QPalette::Light, Qt::green);
    line2->setPalette(line2Pa);

    m_autoSync = new QLabel;
    m_autoSync->setText(tr("Auto Sync"));
    m_autoSync->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_autoSyncSwitch = new DSwitchButton;

    QHBoxLayout *autoSyncHlayout = new QHBoxLayout;
    autoSyncHlayout->addWidget(m_autoSync);
    autoSyncHlayout->addStretch();
    autoSyncHlayout->addWidget(m_autoSyncSwitch);
    autoSyncHlayout->setContentsMargins(0,0,0,0);

    QVBoxLayout *autoSyncVlayout = new QVBoxLayout;
    autoSyncVlayout->addLayout(autoSyncHlayout);
    autoSyncVlayout->addWidget(line2);
    autoSyncVlayout->setContentsMargins(0,0,0,0);

    QWidget *autoSyncWidget = new QWidget;
    autoSyncWidget->setContentsMargins(0,0,0,0);
    autoSyncWidget->setMinimumSize(QSize(400,56));
    autoSyncWidget->setLayout(autoSyncVlayout);

    m_quitButton = new  QPushButton(tr("Sign Out >"));
    newStyle *quitStyle = new newStyle(newStyle::Text);
    quitStyle->setTextColor(textTipLightColor,textTitleLightColor,textTipLightColor);
    m_quitButton->setStyle(quitStyle);

    QHBoxLayout *loginOutHlayout = new QHBoxLayout;
    loginOutHlayout->addStretch();
    loginOutHlayout->addWidget(m_quitButton);
    loginOutHlayout->setContentsMargins(0,0,0,0);

    QWidget *loginOutWidget = new QWidget;
    loginOutWidget->setContentsMargins(0,0,0,0);
    loginOutWidget->setMinimumSize(QSize(410,28));
    loginOutWidget->setLayout(loginOutHlayout);

    m_syncLayout = new QVBoxLayout;
    m_syncLayout->setContentsMargins(0,0,0,0);

    m_syncWidget = new QWidget;
    m_syncWidget->setContentsMargins(0,0,0,0);
    m_syncWidget->setLayout(m_syncLayout);

    QVBoxLayout *areaLayout = new QVBoxLayout;
    areaLayout->setContentsMargins(0,24,0,43);
    areaLayout->addWidget(m_avatarWidget,0, Qt::AlignCenter);
    areaLayout->addWidget(m_nameLabel,0, Qt::AlignCenter);
    areaLayout->addWidget(m_uidLabel,0, Qt::AlignCenter);
    areaLayout->addWidget(m_modifyInfoButton,0, Qt::AlignCenter);
    areaLayout->addWidget(bondWXWidget, 0, Qt::AlignCenter);
    areaLayout->addWidget(autoSyncWidget, 0, Qt::AlignCenter);
    areaLayout->addWidget(m_syncWidget, 0, Qt::AlignCenter);
    areaLayout->addWidget(loginOutWidget, 0, Qt::AlignCenter);
    areaLayout->addStretch();
    areaLayout->setContentsMargins(100,24,100,42);

    QWidget *areaWidget = new QWidget;
    areaWidget->setLayout(areaLayout);
    areaWidget->setContentsMargins(0, 0, 0, 0);

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameStyle(QFrame::NoFrame);
    scrollArea->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setContentsMargins(0, 0, 0, 0);
//    scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContentsOnFirstShow);//暂时没效果,最好还是加上
    scrollArea->setWidget(areaWidget);

    DFontSizeManager::instance()->bind(m_nameLabel, DFontSizeManager::T6,QFont::Medium);
    DFontSizeManager::instance()->bind(m_uidLabel, DFontSizeManager::T7,QFont::Normal);
    DFontSizeManager::instance()->bind(m_modifyInfoButton, DFontSizeManager::T7,QFont::Normal);
    DFontSizeManager::instance()->bind(m_wxlabel, DFontSizeManager::T6,QFont::Normal);
    DFontSizeManager::instance()->bind(m_wxNameLabel, DFontSizeManager::T6,QFont::Normal);
    DFontSizeManager::instance()->bind(m_modButton, DFontSizeManager::T6,QFont::Normal);
    DFontSizeManager::instance()->bind(m_autoSync, DFontSizeManager::T6,QFont::Normal);
    DFontSizeManager::instance()->bind(m_quitButton, DFontSizeManager::T7,QFont::Normal);

    connect(m_autoSyncSwitch, &DSwitchButton::checkedChanged, this, &IndexPage::onSwitchButtoncheckedChanged);
    connect(m_modifyInfoButton, &QPushButton::clicked, this, &IndexPage::onModifyInfo);
//    connect(quitButton, &QPushButton::clicked, this, &IndexPage::requestLogout);
    connect(m_quitButton, &QPushButton::clicked, this, &IndexPage::onQuitButtonClicked);

    m_mainLayout->addWidget(scrollArea);
    m_mainLayout->setContentsMargins(0,0,0,0);
    setLayout(m_mainLayout);

    onThemeTypeChanged(DGuiApplicationHelper::instance()->themeType());

    connect(DGuiApplicationHelper::instance(),&DGuiApplicationHelper::themeTypeChanged,this,&IndexPage::onThemeTypeChanged);

//    m_refreshTimer = new QTimer;
//    connect(m_refreshTimer, &QTimer::timeout, this, &IndexPage::onTokenTimeout);
}

void IndexPage::setModel(UnionidModel *model)
{
    LoginedIn::setModel(model);
//    m_lab->setVisible(!model->getActivation());

    connect(model, &UnionidModel::userInfoChanged, this, &IndexPage::onUserInfoChanged);
    connect(model, &UnionidModel::enableSyncChanged, this, &IndexPage::onChecked);
//    connect(model, &UnionidModel::enableSyncChanged, m_listView, &QListView::setVisible);
//    connect(model, &UnionidModel::enableSyncChanged, m_networkTip, &QLabel::setVisible);
    connect(model, &UnionidModel::syncStateChanged, this, &IndexPage::onStateChanged);
    connect(model, &UnionidModel::moduleSyncStateChanged, this, &IndexPage::onModuleStateChanged);
    connect(model, &UnionidModel::licenseStateChanged, this, [ = ] (const bool &value) {
       m_autoSyncSwitch->setEnabled(value);
        if (m_autoSyncSwitch->isChecked()&& !value) {
            m_autoSyncSwitch->setChecked(false);
            Q_EMIT m_autoSyncSwitch->checkedChanged(m_autoSyncSwitch->isChecked());
        }
        m_lab->setVisible(!value);
    });

    QMap<SyncType, QPair<QString, QString>> moduleTs{
        { SyncType::Network, {("sync_net"), tr("Network Settings") }},
        { SyncType::Sound, {("sync_sound"), tr("Sound Settings") }},
        { SyncType::Mouse, {("sync_mouse"), tr("Mouse Settings") }},
        { SyncType::Update, {("sync_update"), tr("Update Settings") }},
        { SyncType::Dock, {("sync_dock"), tr("Dock") }},
        { SyncType::Launcher, {("sync_launcher"), tr("Launcher") }},
        { SyncType::Wallpaper, {("sync_wallpaper"), tr("Wallpaper") }},
        { SyncType::Theme, {("sync_theme"), tr("Theme") }},
        { SyncType::Power, {("sync_power"), tr("Power Settings") }},
    };

    m_listModel->clear(); // will delete all items

    const std::list<std::pair<SyncType, QStringList>> list = m_model->moduleMap();

    for (auto it = list.cbegin(); it != list.cend(); ++it) {
        syncItemwiget *item = new syncItemwiget(moduleTs[it->first].first,moduleTs[it->first].second,false);
        connect(item,&syncItemwiget::toTellStateChanged,this,&IndexPage::onCheckboxStateChanged);
        item->setSyncType(it->first);
        m_itemMap[it->first] = item->getCheckBox();
        m_syncLayout->addWidget(item);
    }

    std::map<SyncType, bool> moduleState = m_model->moduleSyncState().toStdMap();

    for (auto it = moduleState.cbegin(); it != moduleState.cend(); ++it) {
        onModuleStateChanged(*it);
    };

    onUserInfoChanged(model->userinfo());
    m_autoSyncSwitch->setChecked(model->enableSync() && model->getActivation());

    if (!model->getActivation()) {
        Q_EMIT m_autoSyncSwitch->checkedChanged(m_autoSyncSwitch->isChecked());
    }

    m_autoSyncSwitch->setEnabled(model->getActivation());
//    m_networkTip->setVisible(model->enableSync());
    m_syncWidget->setVisible(model->enableSync());
//    m_listView->setVisible(model->enableSync());
    onStateChanged(model->syncState());
    LoginedIn::setModel(model);
//    connect(model, &UnionidModel::userInfoChanged, this, &IndexPage::onUserInfoChanged);
//    connect(model, &UnionidModel::enableSyncChanged, this, &IndexPage::onChecked);
//    connect(model, &UnionidModel::enableSyncChanged, m_listView, &QListView::setVisible);

    onUserInfoChanged(model->userinfo());
}

void IndexPage::setUserInfo(QString usrInfo)
{
    QByteArray byteJson = usrInfo.toLocal8Bit();
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(byteJson, &jsonError);
    QJsonObject jsonObj = jsonDoc.object();
    QJsonValue jsonValueResult = jsonObj.value("data");

    if (jsonValueResult.isObject()) {
        jsonObj = jsonValueResult.toObject();

        //AT有效期
        jsonValueResult = jsonObj.value("expires_in");
        qInfo() << "expires_in" << jsonValueResult.toInt();
        int expires_in = jsonValueResult.toInt();
//        m_refreshTimer->start(expires_in * 1000);

        jsonValueResult = jsonObj.value("AccessToken");
        QString nResult = jsonValueResult.toString();
        m_accessToken = nResult;
        qInfo() << "m_accessToken" << m_accessToken;

        jsonValueResult = jsonObj.value("RefreshToken");
        nResult = jsonValueResult.toString();
        m_refreshToken = nResult;

        Notificationmanager::instance()->startRefreshToken(m_refreshToken,expires_in * 1000);

        jsonValueResult = jsonObj.value("userNick");
        qInfo() << "userNick" << jsonValueResult.type();
        m_nickName = jsonValueResult.toString();
        m_nameLabel->setText(m_nickName);

        jsonValueResult = jsonObj.value("avatar");
        m_userAvatar = jsonValueResult.toString();
        m_avatarWidget->setAvatarPath(m_userAvatar,true);

        jsonValueResult = jsonObj.value("userName");
        nResult = jsonValueResult.toString();
        m_uidLabel->setText("Union ID：" + nResult);

        jsonValueResult = jsonObj.value("wechatunionid");
        m_wechatunionid = jsonValueResult.toString();
        qInfo() << " wechatunionid" << m_wechatunionid;

        if (m_wechatunionid.isEmpty()) {
            m_modButton->setText(QObject::tr("Link"));
            m_wxNameLabel->setText("");
        } else {
            m_modButton->setText(QObject::tr("Change"));
            QNetworkReply *reply =  HttpClient::instance()->getBindAccountInfo(1, 0, m_wechatunionid);
            connect(reply,&QNetworkReply::finished,this,&IndexPage::onGetBindAccountInfo);
        }
    }
}

void IndexPage::setUserAvatar(QPixmap avatar)
{
    if (avatar.isNull()) {
        m_avatarWidget->setAvatarPath(AvaterPath,false);
    } else {
        m_avatarWidget->setAvater(avatar);
    }
}

void IndexPage::setDefaultInfo()
{
    m_avatarWidget->setAvatarPath(AvaterPath,false);
    m_nameLabel->clear();
    m_uidLabel->clear();
    m_wxNameLabel->clear();
    m_modButton->setText("");
}

void IndexPage::onStateChanged(const std::pair<qint32, QString> &state)
{
    if (!UnionidModel::isSyncStateValid(state)) {
        return;
    }

    SyncState syncState;

    do {
        // check is sync succeed
        if (UnionidModel::isSyncSucceed(state)) {
            syncState = SyncState::Succeed;
            break;
        }

        // check is syncing
        if (UnionidModel::isSyncing(state)) {
            syncState = SyncState::Syncing;
            break;
        }

        // check is sync faild
        if (UnionidModel::isSyncFailed(state)) {
            syncState = SyncState::Failed;
            break;
        }

        Q_UNREACHABLE();

    } while (false);
}

void IndexPage::onModuleStateChanged(std::pair<dcc::cloudsync::SyncType, bool> state)
{

    QCheckBox *item = m_itemMap[state.first];
    Q_ASSERT(item);

    item->setChecked(state.second);
//    qInfo() << "m_itemMap" << state.first << state.second;
//    item->setCheckState(state.second ? Qt::Checked : Qt::Unchecked);
}

void IndexPage::onChecked(bool bIschecked)
{
    m_autoSyncSwitch->setChecked(bIschecked);
}

void IndexPage::onSwitchButtoncheckedChanged(bool bIschecked)
{
    m_syncWidget->setVisible(bIschecked);
    Q_EMIT requestSetAutoSync(bIschecked);
}

void IndexPage::onModifyInfo()
{
    QDesktopServices::openUrl(QUrl("http://login.uniontech.com/", QUrl::TolerantMode));
}

void IndexPage::onThemeTypeChanged(DGuiApplicationHelper::ColorType themeType)
{
    QColor textTitleColor, textTipColor, buttonNormalBorderColor, buttonHoverBorderColor;

    if (themeType == DGuiApplicationHelper::LightType) {
        QColor color(0,0,0);
        textTitleColor = textTitleLightColor;
        textTipColor = textTipLightColor;
        color.setAlphaF(0.15);
        buttonNormalBorderColor = /*buttonNormalBorderLightColor*/color;
        color.setAlphaF(0.25);
        buttonHoverBorderColor = /*buttonHoverBorderLightColor*/color;
    } else if (themeType == DGuiApplicationHelper::DarkType){
        textTitleColor = textTitleDarkColor;
        textTipColor = textTiptDarkColor;
        QColor color(255,255,255);
        color.setAlphaF(0.15);
        buttonNormalBorderColor = /*buttonNormalBorderDarkColor*/color;
        color.setAlphaF(0.25);
        buttonHoverBorderColor = /*buttonHoverBorderDarkColor*/color;
    }

    newStyle *modifyInfoStyle = new newStyle(newStyle::TextRim);
    modifyInfoStyle->setTextColor(textTitleDarkColor);
    modifyInfoStyle->setBorderColor(buttonNormalBorderColor,buttonHoverBorderColor,buttonNormalBorderColor);
    m_modifyInfoButton->setStyle(modifyInfoStyle);

    newStyle *quitStyle = new newStyle(newStyle::Text);
    quitStyle->setTextColor(textTipColor,textTitleColor,textTipColor);
    m_quitButton->setStyle(quitStyle);

    QPalette pa = m_nameLabel->palette();
    pa.setColor(QPalette::Text,textTitleColor);
    m_nameLabel->setPalette(pa);

    pa = m_uidLabel->palette();
    pa.setColor(QPalette::Text,textTipColor);
    m_uidLabel->setPalette(pa);

    pa = m_wxlabel->palette();
    pa.setColor(QPalette::Text,textTitleColor);
    m_wxlabel->setPalette(pa);

    pa = m_wxNameLabel->palette();
    pa.setColor(QPalette::Text,textTipColor);
    m_wxNameLabel->setPalette(pa);

    pa = m_autoSync->palette();
    pa.setColor(QPalette::Text,textTitleColor);
    m_autoSync->setPalette(pa);
}

void IndexPage::onCheckboxStateChanged(bool state, dcc::cloudsync::SyncType syncType)
{
    Q_EMIT requestSetModuleState(std::pair<dcc::cloudsync::SyncType, bool>(syncType,state));
}

void IndexPage::onModButtonClicked()
{
    if (Notificationmanager::instance()->isOnLine()) {
        QNetworkReply *reply =  HttpClient::instance()->getUserInfo(m_accessToken);
        connect(reply,&QNetworkReply::finished,this,&IndexPage::onGetUserInfoResult);
    } else {
        Notificationmanager::instance()->showToast(this,Notificationmanager::NetworkError);
    }
}

void IndexPage::onGetUserInfoResult()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    QString result = HttpClient::instance()->checkReply(reply);

    if (!result.isEmpty()) {
        QByteArray byteJson = result.toLocal8Bit();
        QJsonParseError jsonError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(byteJson, &jsonError);
        QJsonObject jsonObj = jsonDoc.object();
        QJsonValue jsonValueResult = jsonObj.value("phone_number");
        m_phoneNumber = jsonValueResult.toString();

        AuthenticationWindow *authWindow = new AuthenticationWindow;
        connect(authWindow,&AuthenticationWindow::toTellrefreshUserInfo,this,&IndexPage::onTokenTimeout);
        authWindow->setData(m_phoneNumber,m_wechatunionid,m_accessToken,m_userAvatar,m_nameLabel->text());
        authWindow->show();
        m_wechatunionid = "";
    }
}

void IndexPage::onGetBindAccountInfo()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    QString result = HttpClient::instance()->checkReply(reply);

    if (!result.isEmpty()) {

        if (HttpClient::instance()->solveJson(result)) {
            QByteArray byteJson = result.toLocal8Bit();
            QJsonParseError jsonError;
            QJsonDocument jsonDoc = QJsonDocument::fromJson(byteJson, &jsonError);
            QJsonObject jsonObj = jsonDoc.object();
            QJsonValue jsonValueResult = jsonObj.value("data");

            if (jsonValueResult.isObject()) {
                jsonObj = jsonValueResult.toObject();
                jsonValueResult = jsonObj.value("wechatNickName");
                qInfo() << "wechatNickName" << jsonValueResult.toString();
                m_wxNameLabel->setText(jsonValueResult.toString());

                if (m_wechatunionid.isEmpty()) {
                    m_modButton->setText(QObject::tr("Link"));
                } else {
                    m_modButton->setText(QObject::tr("Change"));
                }
            }
        }
    }
}

void IndexPage::onQuitButtonClicked()
{
    QDBusInterface interface("com.deepin.deepinid.Client",
                              "/com/deepin/deepinid/Client",
                              "com.deepin.deepinid.Client");

    QList<QVariant> argumentList;
    argumentList << m_userAvatar;
    argumentList << m_nickName;

    interface.callWithArgumentList(QDBus::NoBlock, "ConfirmLogout", argumentList);

    Q_EMIT requestLogout();
}

void IndexPage::onRefreshAccessToken()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(QObject::sender());
    QString result = HttpClient::instance()->checkReply(reply);
    reply->deleteLater();

    if (HttpClient::instance()->solveJson(result)) {
        Notificationmanager::instance()->setUserInfo(result);
        QByteArray byteJson = result.toLocal8Bit();
        QJsonParseError jsonError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(byteJson, &jsonError);
        QJsonObject jsonObj = jsonDoc.object();
        QJsonValue jsonValueResult = jsonObj.value("data");

        if (jsonValueResult.isObject()) {
            jsonObj = jsonValueResult.toObject();
            jsonValueResult = jsonObj.value("AccessToken");
            m_accessToken = jsonValueResult.toString();

            jsonValueResult = jsonObj.value("RefreshToken");
            m_refreshToken = jsonValueResult.toString();

            jsonValueResult = jsonObj.value("expires_in");
            int expires_in = jsonValueResult.toInt();
            qInfo() << "expires_in" << jsonValueResult.toInt();

            Notificationmanager::instance()->startRefreshToken(m_refreshToken,expires_in * 1000);

            jsonValueResult = jsonObj.value("wechatunionid");
            m_wechatunionid = jsonValueResult.toString();

            QNetworkReply *reply =  HttpClient::instance()->getBindAccountInfo(1, 0, m_wechatunionid);
            connect(reply,&QNetworkReply::finished,this,&IndexPage::onGetBindAccountInfo);
        }
    }
}

void IndexPage::onTokenTimeout()
{
    QNetworkReply *reply = HttpClient::instance()->refreshAccessToken(HttpClient::instance()->getClientId(),m_refreshToken);
    connect(reply,&QNetworkReply::finished,this,&IndexPage::onRefreshAccessToken);
}

IndexPage::~IndexPage()
{
    qInfo() << "IndexPage::~IndexPage()***********";
}

}
}
