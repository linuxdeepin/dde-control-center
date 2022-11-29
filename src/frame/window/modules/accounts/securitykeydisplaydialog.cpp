// SPDX-FileCopyrightText: 2021 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "securitykeydisplaydialog.h"
#include "window/dconfigwatcher.h"
#include "window/utils.h"
#include "widgets/utils.h"

#include <QEvent>
#include <QVBoxLayout>
#include <QDebug>
#include <QPixmap>
#include <QMouseEvent>
#include <QClipboard>
#include <QApplication>
#include <QRandomGenerator>

const QString RandomData = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
const int RandomDataSize = RandomData.length();
const int RandomDataLength = 24;
const QString CopyIconPath = QString(":/accounts/themes/common/icons/security_key_copy.svg");

SecurityKeyDisplayDialog::SecurityKeyDisplayDialog(dcc::accounts::AccountsWorker *work, DDialog *parent)
    : DDialog(parent)
    , m_securityKeyDisplayTxt(nullptr)
    , m_copyIconLabel(nullptr)
    , m_currentAccount("")
    , m_work(work)
{
    initWidget();
    initData();
}

void SecurityKeyDisplayDialog::initWidget()
{
    addButtons(QStringList() << tr("Cancel") << tr("Confirm"));
    setIcon(QIcon::fromTheme("dialog-warning"));

    auto isWayland = qEnvironmentVariable("XDG_SESSION_TYPE").contains("wayland");
    if (isWayland) {
        setWindowFlags(Qt::NoDropShadowWindowHint | Qt::Window | Qt::CustomizeWindowHint);
        create();
        setWindowFlags(Qt::NoDropShadowWindowHint | Qt::Window  | Qt::CustomizeWindowHint);
    } else {
        setWindowFlags(Qt::NoDropShadowWindowHint | Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint);
        setWindowFlags(Qt::Tool | Qt::NoDropShadowWindowHint | Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint);
    }
    installEventFilter(this);
    setFixedSize(530, 272);
    setOnButtonClickedClose(true);
    setCloseButtonVisible(true);

    QWidget *widget = new QWidget;
    addContent(widget);

    DLabel *txt = new DLabel(tr("An account security key is a code that can be used to log in to the system if you forgot your password. Please copy the code and save it in a safe place."));
    txt->setAlignment(Qt::AlignHCenter);
    txt->setWordWrap(true);

    m_securityKeyDisplayTxt = new DLabel;
    QHBoxLayout *securityKeyLayout = new QHBoxLayout;
    securityKeyLayout->addWidget(m_securityKeyDisplayTxt);

    m_copyIconLabel = new DLabel;
    QPixmap pic(CopyIconPath);
    pic.setDevicePixelRatio(devicePixelRatioF());
    m_copyIconLabel->setPixmap(pic);
    securityKeyLayout->addWidget(m_copyIconLabel);
    securityKeyLayout->setAlignment(Qt::AlignCenter);
    m_copyIconLabel->installEventFilter(this);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(txt);
    layout->addSpacing(5);
    layout->addLayout(securityKeyLayout);
    widget->setLayout(layout);
}

void SecurityKeyDisplayDialog::initData()
{
    connect(this, &DDialog::buttonClicked, this, [this](int index, const QString & text) {
        qDebug() << " [initData] buttonClicked index : " << index << text;
        Q_EMIT notifySaveSecurityKey(index);
    });

    connect(this, &DDialog::closed, this, [this]() {
        qDebug() << " [initData] closed.";
        Q_EMIT notifySaveSecurityKey(false);
    });

}

void SecurityKeyDisplayDialog::updateSecurityKey()
{
    m_securityKeyDisplayTxt->setText(DCC_NAMESPACE::getSecurityKeyDisplayData(getRandomString(RandomDataLength)));

    QFont tf(m_securityKeyDisplayTxt->text().at(0));
    tf.setPointSize(16);
    tf.setWeight(QFont::DemiBold);
    m_securityKeyDisplayTxt->setFont(tf);
    m_securityKeyDisplayTxt->setAlignment(Qt::AlignHCenter);
}

bool SecurityKeyDisplayDialog::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_copyIconLabel && event->type() == QEvent::MouseButtonRelease && static_cast<QMouseEvent *>(event)->button() == Qt::MouseButton::LeftButton) {
        copySecurityKeyToClip();
    }
    return DDialog::eventFilter(obj, event);
}

/* function 获取随机数密钥
 * length 密钥长度
 * return 密钥
*/
QString SecurityKeyDisplayDialog::getRandomString(int length) const
{
    QString ret = "";
    for (int i = 0; i < length; i++) {
        // bounded -> [0, RandomDataSize)
        ret += RandomData[QRandomGenerator::global()->bounded(0, RandomDataSize)];
    }
    return ret;
}

void SecurityKeyDisplayDialog::copySecurityKeyToClip()
{
    QClipboard *clip = QApplication::clipboard();
    if (clip && m_securityKeyDisplayTxt) {
        clip->setText(m_securityKeyDisplayTxt->text());
    } else {
        qWarning() << Q_FUNC_INFO << " get clipboard failed.";
    }
}

void SecurityKeyDisplayDialog::saveSecurityKey(QString currentAccount)
{
    const QString securityKeyDisplayTxt = m_securityKeyDisplayTxt->text().remove(QRegExp("\\-"));
    if (securityKeyDisplayTxt == "") {
        qWarning() << " [saveSecurityKey] securityKeyDisplayTxt is NULL.";
        return;
    }
    if (QString::compare(m_currentAccount, currentAccount) != 0) {
        m_currentAccount = currentAccount;
    }
    m_work->setSecurityKey(cryptUserPassword(securityKeyDisplayTxt));
}

void SecurityKeyDisplayDialog::clearSecurityKey()
{
    m_securityKeyDisplayTxt->setText("");
    m_work->deleteSecretKey();
}

void SecurityKeyDisplayDialog::setCurrentAccount(QString account)
{
    if (QString::compare(m_currentAccount, account) != 0) {
        m_currentAccount = account;
    }
}
