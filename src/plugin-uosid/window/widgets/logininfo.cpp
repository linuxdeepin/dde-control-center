/*
 * Copyright (C) 2016 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     duanhongyu <duanhongyu@uniontech.com>

 * Maintainer: duanhongyu <duanhongyu@uniontech.com>
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

#include "syncmodel.h"
#include "downloadurl.h"
#include "avatarwidget.h"
#include "logininfo.h"
#include <ddbussender.h>

#include <DFloatingButton>
#include <DFontSizeManager>
#include <DListView>
#include <DLabel>
#include <DLineEdit>
#include <DToolButton>
#include <DApplicationHelper>
#include <DDesktopServices>

#include <QEvent>
#include <QTimer>
#include <QBoxLayout>
#include <QObject>
#include <QWidget>
#include <QDir>
#include <QLabel>
#include <QDebug>
#include <QPushButton>
#include <QDesktopServices>
#include <QDBusPendingReply>

DWIDGET_USE_NAMESPACE

const static QString NicknameInvalid = QStringLiteral("7515");
const static QString NicknameNULL = QStringLiteral("7501");
const static QString OtherInvalid = QStringLiteral("7500");

LoginInfoPage::LoginInfoPage(QWidget *parent)
    : QWidget (parent)
    , m_mainLayout(new QVBoxLayout)
    , m_model(nullptr)
    , m_downloader(nullptr)
    , m_avatarPath(QString("%1/.cache/deepin/dde-control-center/sync").arg(getenv("HOME")))
    , m_avatar(new AvatarWidget(this))
    , m_fristLogin(true)
    , m_username(new DLabel(this))
    , m_editNameBtn(new DToolButton(this))
    , m_inputLineEdit(new DLineEdit(this))
    , m_listView(new DListView)
    , m_listModel(new QStandardItemModel(this))
    , m_logoutBtn(new QPushButton (this))
    , m_editInfoBtn(new QPushButton(this))
{
    initUI();
    initConnection();
}

LoginInfoPage::~LoginInfoPage()
{
    if (m_downloader != nullptr)
        m_downloader->deleteLater();
}

void LoginInfoPage::setModel(SyncModel *model)
{
    m_model = model;
    m_userFullName = m_model->userinfo()["Nickname"].toString();
    m_listView->setModel(m_listModel);
    connect(m_model, &SyncModel::userInfoChanged, this, &LoginInfoPage::onUserInfoChanged);
    connect(m_model, &SyncModel::resetUserNameError, this, &LoginInfoPage::onResetError, Qt::QueuedConnection);

    onUserInfoChanged(m_model->userinfo());
}

void LoginInfoPage::initUI()
{
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);
    setFocusPolicy(Qt::FocusPolicy::ClickFocus);

    m_avatar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    m_avatar->setFixedSize(QSize(100, 100));
    m_username->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_editNameBtn->setIcon(QIcon::fromTheme("dcc_edit"));
    m_editNameBtn->setIconSize(QSize(12, 12));

    m_inputLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_inputLineEdit->setVisible(false);
    m_inputLineEdit->lineEdit()->setFrame(false);
    m_inputLineEdit->lineEdit()->installEventFilter(this);

    DFontSizeManager::instance()->bind(m_username, DFontSizeManager::T5);
    DFontSizeManager::instance()->bind(m_inputLineEdit, DFontSizeManager::T5);

    // 头像名称布局
    QVBoxLayout *avatarLayout = new QVBoxLayout;
    avatarLayout->setContentsMargins(10, 0, 10, 0);
    avatarLayout->setSpacing(10);
    avatarLayout->addWidget(m_avatar, 0, Qt::AlignHCenter);
    avatarLayout->setSpacing(20);

    // 名称编辑区域设置固定
    QVBoxLayout *inputLineLayout = new QVBoxLayout;
    QWidget *inputLine = new QWidget;
    inputLine->setFixedHeight(70);
    inputLine->setLayout(inputLineLayout);

    inputLineLayout->addWidget(m_username, 0, Qt::AlignHCenter);
    inputLineLayout->addWidget(m_inputLineEdit);
    m_inputLineEdit->setVisible(false);
    inputLineLayout->setSpacing(10);
    inputLineLayout->addWidget(m_editNameBtn, 0, Qt::AlignHCenter);
    inputLineLayout->addStretch();
    avatarLayout->addWidget(inputLine);

    // 用户信息
    QHBoxLayout *listLayout = new QHBoxLayout;
    listLayout->setContentsMargins(10, 0, 10, 0);
    m_mainLayout->setSpacing(0);
    m_listView->setBackgroundType(DStyledItemDelegate::BackgroundType::NoBackground);
    m_listView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listView->setSelectionMode(QListView::SelectionMode::NoSelection);
    m_listView->setEditTriggers(DListView::NoEditTriggers);
    m_listView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listView->setFrameShape(DListView::NoFrame);
    m_listView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContentsOnFirstShow);

    m_listView->setViewportMargins(0, 0, 0, 0);
    m_listView->setItemSpacing(10);
    listLayout->addWidget(m_listView, 0, Qt::AlignHCenter);

    // 底部按钮布局
    m_editInfoBtn->setToolTip(tr("Edit account"));
    m_editInfoBtn->setIcon(QIcon::fromTheme("dcc_sync_Setting"));
    m_editInfoBtn->setIconSize(QSize(16, 16));
    m_editInfoBtn->setFixedSize(36,36);
    m_logoutBtn->setToolTip(tr("Sign out"));
    m_logoutBtn->setIcon(QIcon::fromTheme("dcc_sync_out"));
    m_logoutBtn->setIconSize(QSize(16, 16));
    m_logoutBtn->setFixedSize(36,36);
    QHBoxLayout *buttLayout = new QHBoxLayout;
    buttLayout->setAlignment(Qt::AlignHCenter);
    buttLayout->setContentsMargins(10, 10, 10, 10);
    buttLayout->addWidget(m_editInfoBtn, 0, Qt::AlignHCenter);
    buttLayout->addSpacing(10);
    buttLayout->addWidget(m_logoutBtn, 0, Qt::AlignHCenter);
    m_mainLayout->addSpacing(60);
    m_mainLayout->addLayout(avatarLayout);
    m_mainLayout->addSpacing(10);
    m_mainLayout->addLayout(listLayout);
    m_mainLayout->addStretch();
    m_mainLayout->addLayout(buttLayout);
    setLayout(m_mainLayout);
}

void LoginInfoPage::initConnection()
{
    connect(m_inputLineEdit, &DLineEdit::textChanged, this, [ = ](const QString &userFullName) {
        m_inputLineEdit->setAlert(false);
        if (userFullName.isEmpty() || userFullName.size() > 32) {
            m_inputLineEdit->setAlert(true);
            m_inputLineEdit->showAlertMessage(tr("The nickname must be 1~32 characters long"), m_inputLineEdit, 2000);
        }
    });

    connect(m_inputLineEdit, &DLineEdit::textEdited, this, [ = ](const QString &userFullName) {
        QString fullName = userFullName;
        fullName.remove(":");
        if (fullName != userFullName) {
            m_inputLineEdit->setText(fullName);
        }
        if (fullName.size() > 32) {
            m_inputLineEdit->setAlert(true);
            m_inputLineEdit->showAlertMessage(tr("The full name is too long"), this);
            DDesktopServices::playSystemSoundEffect(DDesktopServices::SSE_Error);
        }
    });

    connect(m_inputLineEdit, &DLineEdit::editingFinished, this, [ = ] {
        QString userFullName = m_inputLineEdit->lineEdit()->text();
        onEditingFinished(userFullName);
    });

    //点击用户全名编辑按钮
    connect(m_editNameBtn, &DIconButton::clicked, this, [ = ]() {
        m_username->setVisible(false);
        m_editNameBtn->setVisible(false);
        m_inputLineEdit->setVisible(true);
        m_inputLineEdit->setAlert(false);
        m_inputLineEdit->setText(m_userFullName);
        m_inputLineEdit->hideAlertMessage();
        m_inputLineEdit->lineEdit()->setFocus();
    });

    connect(m_logoutBtn, &DFloatingButton::clicked, this, &LoginInfoPage::requestLogoutUser);
    connect(m_editInfoBtn, &DFloatingButton::clicked, [this](){
        QString url = loadCodeURL();
        QUrl::toPercentEncoding(url);
        QDesktopServices::openUrl(QUrl(url));
    });

    connect(Dtk::Gui::DGuiApplicationHelper::instance(), &Dtk::Gui::DGuiApplicationHelper::themeTypeChanged,
        this, [this](Dtk::Gui::DGuiApplicationHelper::ColorType themeType) {
        QString theme = (themeType == DGuiApplicationHelper::DarkType) ? "_dark" : QString();
        for (int i=0;i<m_listModel->rowCount();i++){
            QStandardItem *item =m_listModel->item(i);
            item->setIcon(QIcon::fromTheme(item->data().toString()+theme).pixmap(16, QIcon::Disabled, QIcon::Off));
        }
    });
}

void LoginInfoPage::onUserInfoChanged(const QVariantMap &infos)
{
    m_userFullName = m_model->userinfo()["Nickname"].toString();
    const bool isLogind = !infos["Username"].toString().isEmpty();
    const QString region = infos["Region"].toString();
    QString profile_image = infos.value("ProfileImage").toString();

    if (!isLogind) {
        m_fristLogin = true;
        return;
    }

    if (m_fristLogin && isLogind) {
        QTimer::singleShot(5000, this, [this]() {
            Q_EMIT requestPullMessage();
        });

        m_fristLogin = false;
    }
    onAvatarChanged(profile_image);
    m_username->setText(handleNameTooLong(m_model->userDisplayName()).toHtmlEscaped());

    QString phone = infos["Phone"].toString();
    QString mail = infos["Email"].toString();
    QString wechat = infos["WechatNickname"].toString();
    QString regionDate = (region == "CN") ? tr("Mainland China") : tr("Other regions");

    qDebug() << "tonken Info : " << phone << mail << wechat;
    QList<QPair<QString, QString>> infoTs {
        {"dcc_sync_phone", phone},
        {"dcc_sync_mail", mail},
        {"dcc_sync_wechat", wechat},
        {"dcc_sync_place", regionDate},
    };
    onUserInfoListChanged(infoTs);
}

bool LoginInfoPage::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_inputLineEdit->lineEdit() && event->type() == QEvent::MouseButtonPress) {
        m_inputLineEdit->hideAlertMessage();
        m_inputLineEdit->lineEdit()->setFocus();
    }
    return false;
}

// 处理编辑输入完成后的逻辑
void LoginInfoPage::onEditingFinished(const QString &userFullName)
{
    QString fullName = userFullName;
    m_inputLineEdit->lineEdit()->clearFocus();
    m_inputLineEdit->setVisible(false);
    m_username->setVisible(true);
    m_editNameBtn->setVisible(true);
    if (userFullName.isEmpty() || userFullName.size() > 32) {
        m_inputLineEdit->setAlert(false);
        m_inputLineEdit->hideAlertMessage();
        return;
    }

    m_username->setText(handleNameTooLong(userFullName).toHtmlEscaped());
    Q_EMIT requestSetFullname(fullName);
}

void LoginInfoPage::onAvatarChanged(const QString &avaPath)
{
    QString profile_image = avaPath;
    QString avatarPath = QString("%1%2").arg(m_avatarPath).arg(m_model->userDisplayName());
    QDir dir;
    dir.mkpath(avatarPath);
    qDebug() << " ProfileImage = " << profile_image << ", avatarPath " << avatarPath;
    if (profile_image.isEmpty())
        return;

    if (m_downloader == nullptr)
        m_downloader = new DownloadUrl;

    connect(m_downloader, &DownloadUrl::fileDownloaded, this, &LoginInfoPage::setAvatarPath, Qt::UniqueConnection);
    m_downloader->downloadFileFromURL(profile_image, avatarPath);
}

void LoginInfoPage::onUserInfoListChanged(const QList<QPair<QString, QString>> &moduleTs)
{
    m_listModel->clear();
    QString theme = (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::DarkType) ? "_dark" : QString();
    for (auto it = moduleTs.cbegin(); it != moduleTs.cend(); ++it) {
        QString itemIcon = it->first;
        QString itemText = it->second;
        DStandardItem *item = new DStandardItem;

        item->setText(itemText);
        item->setData(itemIcon);
        item->setFontSize(DFontSizeManager::T9);
        item->setTextColorRole(DPalette::TextTips);
        item->setIcon(QIcon::fromTheme(item->data().toString()+theme).pixmap(16, QIcon::Disabled, QIcon::Off));

        if (!itemText.isEmpty())
            m_listModel->appendRow(item);
    }
    m_listView->setModel(m_listModel);
}

void LoginInfoPage::setAvatarPath(const QString &avatarPath)
{
    qDebug() << "downloaded filename = " << avatarPath;
    m_avatar->setAvatarPath(avatarPath);
}

void LoginInfoPage::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    m_username->setText(handleNameTooLong(m_userFullName, this->width()).toHtmlEscaped());
}

QString LoginInfoPage::handleNameTooLong(const QString &fullName, int base)
{
    int widthBase = 24;
    if (base)
        widthBase = base / 10;
    QString name = fullName;
    for (int i = 1; i < fullName; ++i) {
        if (fullName.left(i).toLocal8Bit().size() > widthBase) {
            name = fullName.left(i - 1) + QString("...");
            break;
        }
    }
    return name;
}

void LoginInfoPage::onResetError(const QString &error)
{
    qDebug() << "ResetPasswd error: " << error;
    if (error.contains(NicknameInvalid)) {
        m_inputLineEdit->showAlertMessage(tr("Invalid nickname, please enter a new one"), this);
        m_username->setText(handleNameTooLong(m_userFullName).toHtmlEscaped());
    } else if (error.contains(NicknameNULL)) {
        m_inputLineEdit->showAlertMessage(tr("The nickname cannot contain special characters ('<>&\") and whitespaces"), this);
        m_username->setText(handleNameTooLong(m_userFullName).toHtmlEscaped());
    }
}

QString LoginInfoPage::loadCodeURL()
{
    auto func_getToken = [this] {
        QDBusPendingReply<QString> retToken = DDBusSender()
                .service("com.deepin.sync.Daemon")
                .interface("com.deepin.utcloud.Daemon")
                .path("/com/deepin/utcloud/Daemon")
                .method("UnionLoginToken")
                .call();
        retToken.waitForFinished();
        QString token = retToken.value();
        if (token.isEmpty())
            qDebug() << retToken.error().message();
        return token;
    };

    QString oauthURI = "https://login.uniontech.com";

    if (!qEnvironmentVariableIsEmpty("DEEPINID_OAUTH_URI")) {
        oauthURI = qgetenv("DEEPINID_OAUTH_URI");
    }

    QString url = oauthURI += QString("/oauth2/authorize/registerlogin?autoLoginKey=%1").arg(func_getToken());
    return url;
}
