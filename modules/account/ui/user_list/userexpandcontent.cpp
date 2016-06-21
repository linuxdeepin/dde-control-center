/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QTimer>
#include <QApplication>
#include "dbus/dbusaccount.h"
#include "dbus/dbussessionmanager.h"
#include "userexpandcontent.h"
#include <sys/types.h>
#include <unistd.h>

UserExpandContent::UserExpandContent(const QString &userPath, QWidget *parent)
    : QWidget(parent),m_userPath(userPath)
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setAlignment(Qt::AlignHCenter);
    m_mainLayout->setSpacing(0);

    QTimer *initTimer = new QTimer(this);
    connect(initTimer, SIGNAL(timeout()), this, SLOT(initDBusData()));
    initTimer->start(1000);
}

UserExpandContent::~UserExpandContent()
{

}

void UserExpandContent::onRequestPreDestroy()
{
    if (m_passwordFrame)
        m_passwordFrame->preDestroy();
    changeControlCenterHideable(true);
}

void UserExpandContent::initDBusData()
{
    QTimer *timer = qobject_cast<QTimer *>(sender());
    if (timer) {
        timer->stop();
        timer->deleteLater();

        //get dbus data
        DBusAccount * da = new DBusAccount(this);
        connect(da, &DBusAccount::Success, [=](uint id, const QString &) {
            if (uint(getpid()) == id) {
                changeControlCenterHideable(true);
            }
        });

        if (!m_accountUser)
            m_accountUser = new DBusAccountUser(m_userPath, this);

        if (da->isValid() && m_accountUser->isValid()) {
            DBusSessionManager sessionManager;
            QString currentUserPath = da->FindUserById(sessionManager.currentUid()).value();
            m_isCurrentUser = m_userPath == currentUserPath;

            initAvatarPanel();
            initAutoLogin();
            initUserEnable();
            initPassword();

            m_mainLayout->addStretch(1);

            onAccountEnableChanged(!m_accountUser->locked());
            updateSize();
            qWarning() << "Account: init user's dbus-data completed!" << m_userPath;
        }
        else
            qWarning() << "Account: dbus's data invalid!" << m_userPath;
    }
}

void UserExpandContent::changeControlCenterHideable(bool hideable)
{
    qDebug() << "change hideable to " << hideable;

    if (hideable) {
        //500毫秒延时是为了防止某些性能比较好的机器上dbus数据处理比较快，
        //引起的返回比较快导致窗口焦点切换还没完成就设置控制中心为“可隐藏”状态
        //这是比较折中的处理方案
        //fixme
        QTimer *t = new QTimer(this);
        t->setSingleShot(true);
        connect(t, &QTimer::timeout, this, [=] {
//            this->window()->setProperty("autoHide", true);
            QMetaObject::invokeMethod(this->window(), "setAutoHide", Qt::QueuedConnection, Q_ARG(bool, true));
            sender()->deleteLater();
        });
        t->start(200);
    }
    else {
        qApp->processEvents();
        this->window()->setProperty("autoHide", false);
    }
}

void UserExpandContent::initAvatarPanel()
{
    m_allAvatarGrid = new AvatarGrid(AvatarGrid::NormalGrid, m_userPath, this);

    connect(m_allAvatarGrid, &AvatarGrid::avatarSelected, this, &UserExpandContent::onAvatarSelected);

    m_stackWidget = new QStackedWidget(this);
    connect(m_stackWidget, &QStackedWidget::currentChanged, [=](int){
        m_allAvatarGrid->setAvatars(m_accountUser->iconList() << ADD_AVATAR_ICON);

        updatemAvatarGridSize();

        if (m_autoLoginLine)    //after initialization
            updateSize();
    });
    connect(m_accountUser, &DBusAccountUser::IconListChanged, [=]{
        m_allAvatarGrid->setAvatars(m_accountUser->iconList() << ADD_AVATAR_ICON);
        updatemAvatarGridSize();
        updateSize(true);
    });
    connect(m_accountUser, &DBusAccountUser::HistoryIconsChanged, [=] {
        updatemAvatarGridSize();
        updateSize(true);
    });

    m_stackWidget->addWidget(m_allAvatarGrid);
    m_stackWidget->setCurrentIndex(1);

    m_mainLayout->addWidget(m_stackWidget, 0, Qt::AlignHCenter);
    m_mainLayout->addWidget(new DSeparatorHorizontal);
}

void UserExpandContent::initAutoLogin()
{
    m_autoLoginLine = new AccountSwitchLine();
    m_autoLoginLine->setTitle(tr("Auto-login"));
    m_autoLoginLine->setCheck(m_accountUser->automaticLogin());
    connect(m_autoLoginLine, &AccountSwitchLine::checkChanged, [=](bool s){
        if (m_accountUser->automaticLogin() == s)//just record the valid value(configure file just has single line record,set invalid value will cause error)
            return;

        changeControlCenterHideable(false);
        QDBusPendingReply<bool> reply = m_accountUser->SetAutomaticLogin(s);
        reply.waitForFinished();
        if (reply.error().isValid()) {
            //reset state
            m_autoLoginLine->setCheck(m_accountUser->automaticLogin());
            changeControlCenterHideable(true);
            qWarning() << "Account: set account automatic login error: " << reply.error();
        }
    });
    connect(m_accountUser, &DBusAccountUser::AutomaticLoginChanged, [=]{
        m_autoLoginLine->setCheck(m_accountUser->automaticLogin());
    });

    m_mainLayout->addWidget(m_autoLoginLine);
}

void UserExpandContent::initUserEnable()
{
    m_lockLine = new AccountSwitchLine();
    m_lockLine->setTitle(tr("Enable"));
    m_lockLine->setCheck(!m_accountUser->locked());
    connect(m_lockLine, &AccountSwitchLine::checkChanged, [=](bool s){
        if (!m_accountUser->locked() == s)
            return;

        changeControlCenterHideable(false);
        QDBusPendingReply<bool> reply = m_accountUser->SetLocked(!s);
        reply.waitForFinished();
        if (reply.error().isValid()) {
            //reset state
            m_lockLine->setCheck(!m_accountUser->locked());
            changeControlCenterHideable(true);
            qWarning()<<"Account: set account lock error: " << reply.error();
        }
    });
    connect(m_accountUser, &DBusAccountUser::LockedChanged, [=]{
        m_lockLine->setCheck(!m_accountUser->locked());

        onAccountEnableChanged(!m_accountUser->locked());
    });

    m_mainLayout->addWidget(m_lockLine);

    if (m_isCurrentUser) {
        m_lockLine->setFixedHeight(0);
        m_lockLine->hide();
    }
}

void UserExpandContent::initPassword()
{
    m_passwordFrame = new PasswordFrame();
    connect(m_passwordFrame, &PasswordFrame::sizeChanged, [=]{
        updateSize();
    });
    connect(m_passwordFrame, &PasswordFrame::changePassword, [=](QString password){
        changeControlCenterHideable(false);
        QDBusPendingReply<bool> reply = m_accountUser->SetPassword(password);
        reply.waitForFinished();
        if (reply.error().isValid()) {
            changeControlCenterHideable(true);
            qWarning() << "Account: set password error: " << reply.error();
        }
    });

    m_mainLayout->addWidget(m_passwordFrame);
}

void UserExpandContent::onAvatarSelected(const QString &avatar)
{
//    this->window()->setProperty("autoHide", false);
    changeControlCenterHideable(false);
    QString fileName = "";
    if (avatar == ADD_AVATAR_ICON){
        QFileDialog dl(this);
        dl.setDirectory(QDir::homePath());
        dl.setWindowTitle(tr("Choose a new picture for your Avatar"));
        dl.setNameFilter(tr("Image files (*.jpg *.png *.jpeg)"));

        QRect rec = QApplication::desktop()->screenGeometry();
        dl.move(rec.x() + (rec.width() - dl.width()) / 2, rec.y() + (rec.height() - dl.height()) / 2);
        if (dl.exec() == QDialog::Accepted) {
            fileName = dl.selectedFiles().first();
        }
    }
    else
        fileName = avatar;

    if (!fileName.isEmpty()) {
        changeControlCenterHideable(false);
        QDBusPendingReply<bool> reply = m_accountUser->SetIconFile(fileName);
        reply.waitForFinished();
        if (reply.error().isValid()) {
            changeControlCenterHideable(true);
            qWarning()<<"Account: set icon file error: " << reply.error();
        }
    }

    changeControlCenterHideable(true);
}

void UserExpandContent::onAccountEnableChanged(bool enabled)
{
    if (enabled) {
        updatemAvatarGridSize();
        m_autoLoginLine->setFixedHeight(DTK_WIDGET_NAMESPACE::CONTENT_HEADER_HEIGHT);
        m_passwordFrame->setFixedHeight(DTK_WIDGET_NAMESPACE::CONTENT_HEADER_HEIGHT);
    }
    else {
        m_stackWidget->setFixedHeight(0);
        m_autoLoginLine->setFixedHeight(0);
        m_passwordFrame->reset();
        m_passwordFrame->setFixedHeight(0);
    }

    updateSize(true);
}

void UserExpandContent::updateSize(bool note)
{
    int totalHeight = 0;
    totalHeight += m_stackWidget->height();
    totalHeight += m_passwordFrame->height();
    totalHeight += m_autoLoginLine->height();
    totalHeight += m_lockLine->height();

    setFixedHeight(totalHeight);

    if (note)
        emit sizeChanged();
}

void UserExpandContent::updatemAvatarGridSize()
{
    m_stackWidget->setFixedSize(m_allAvatarGrid->size());
}


