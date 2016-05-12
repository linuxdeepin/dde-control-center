/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "accountmainwidget.h"

AccountMainWidget::AccountMainWidget(QWidget *parent) : QFrame(parent)
{
    qWarning() << "Account: init module now...";
    D_THEME_INIT_WIDGET(AccountMainWidget);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    m_stackWidget = new QStackedWidget();

    DSeparatorHorizontal *sh = new DSeparatorHorizontal(this);
    m_mainLayout->addWidget(sh);

    initDBusAccount();
    initHeader();
    initListPanel();
    initCreatePanel();

    m_mainLayout->addWidget(m_stackWidget);
    qWarning() << "Account: init module completed!";
}

void AccountMainWidget::preDestroy()
{
    m_createPanel->preDestroy();
    m_listPanel->preDestroy();
}

void AccountMainWidget::initHeader()
{
    m_header = new ModuleHeader(tr("User Accounts"), false, this);
    m_mainLayout->addWidget(m_header);
    DSeparatorHorizontal *separator = new DSeparatorHorizontal;
    m_mainLayout->addWidget(separator);


    initHeaderStackWidget();

    m_header->setRightContent(m_headerStackWidget);

    //ControlButton//////////////////////////////////////////
    QWidget *headerButtonContent = new QWidget();

    DBusSessionManager sessionManager;
    QString currentUserPath = m_account->FindUserById(sessionManager.currentUid()).value();
    DBusAccountUser dau(currentUserPath);
    bool adminCurrentLogin = dau.accountType() == 1;
    if (!adminCurrentLogin) {
        headerButtonContent->setFixedHeight(0);
    }

    QHBoxLayout *buttonLayout = new QHBoxLayout(headerButtonContent);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(0);
    buttonLayout->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    m_deleteUserButton = new GeneralRemoveButton;
    if (m_account && m_account->userList().count() <= 1) {
        m_deleteUserButton->setVisible(false);
        m_deleteUserButton->setEnabled(false);
    }

    m_addUserButton = new GeneralAddButton;
    buttonLayout->addWidget(m_deleteUserButton);
    buttonLayout->addSpacing(DTK_WIDGET_NAMESPACE::BUTTON_MARGIN);
    buttonLayout->addWidget(m_addUserButton);

    //user list label//////////////////////////////////////////
    DTextButton *listButton = new DTextButton(tr("User List"));
    listButton->setFixedHeight(DTK_WIDGET_NAMESPACE::BUTTON_HEIGHT);
    connect(listButton, &DTextButton::clicked, [ = ] {
        m_headerStackWidget->setVisible(true);
        m_headerStackWidget->setCurrentIndex(0);
        m_state = StateNormal;
        emit showForNormal();
    });
    QFrame *lbFrame = new QFrame;
    QHBoxLayout *lbLayout = new QHBoxLayout(lbFrame);
    lbLayout->setContentsMargins(0, 0, 0, 0);
    lbLayout->setAlignment(Qt::AlignVCenter);
    lbLayout->addWidget(listButton);

    m_headerStackWidget->addWidget(headerButtonContent);
    m_headerStackWidget->addWidget(lbFrame);

    m_buttonToolTip = new DynamicLabel(m_header);
    m_buttonToolTip->stackUnder(m_headerStackWidget);

    connect(this, &AccountMainWidget::cancelDelete, [ = ] {
        if (m_deleteUserButton)
        {
            m_deleteUserButton->setChecked(false);

            m_state = StateNormal;
        }
    });

    connect(m_deleteUserButton, &GeneralRemoveButton::stateChanged, this, &AccountMainWidget::onDeleteButtonStateChanged);
    connect(m_deleteUserButton, &GeneralRemoveButton::mouseEnter, this, &AccountMainWidget::onDeleteButtonMouseEntered);
    connect(m_deleteUserButton, &GeneralRemoveButton::mouseLeave, m_buttonToolTip, &DynamicLabel::hideLabel);
    connect(m_addUserButton, &GeneralAddButton::clicked, this, &AccountMainWidget::onAddButtonClicked);
    connect(m_addUserButton, &GeneralAddButton::mouseEnter, this, &AccountMainWidget::onAddButtonMouseEntered);
    connect(m_addUserButton, &GeneralAddButton::mouseLeave, m_buttonToolTip, &DynamicLabel::hideLabel);
    connect(this, &AccountMainWidget::stateChanged, this, [ = ](PanelState state) {
        if (state == StateDeleting) {
            m_addUserButton->setEnabled(false);
        } else {
            m_addUserButton->setEnabled(true);
        }
    });
}

void AccountMainWidget::initListPanel()
{
    m_listPanel = new UserListPanel();
    m_listScrollArea = new QScrollArea();
    m_stackWidget->addWidget(m_listScrollArea);

    m_listScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listScrollArea->setWidgetResizable(true);
    connect(this, &AccountMainWidget::requestDelete, m_listPanel, &UserListPanel::requestDelete);
    connect(this, &AccountMainWidget::hideForSetting, m_listPanel, &UserListPanel::hideForSetting);
    connect(this, &AccountMainWidget::showForNormal, m_listPanel, &UserListPanel::showForNormal);
    connect(m_listPanel, &UserListPanel::changeToSetting, [ = ](bool setting) {
        if (setting) {
            m_headerStackWidget->setVisible(true);
            m_headerStackWidget->setCurrentIndex(1);

            m_state = StateSetting;
            emit hideForSetting();
        } else {
            m_headerStackWidget->setVisible(true);
            m_headerStackWidget->setCurrentIndex(0);

            m_state = StateNormal;
            emit showForNormal();
        }
    });
    connect(m_listPanel, &UserListPanel::cancelDelete, [ = ] {
        emit cancelDelete();
        emit requestDelete(false);
        setPanelState(StateNormal);
    });
    m_listScrollArea->setWidget(m_listPanel);
}

void AccountMainWidget::initCreatePanel()
{
    m_createPanel = new CreateUserPanel();
    connect(m_createPanel, &CreateUserPanel::createCancel, [ = ] {
        setPanelState(StateNormal);
    });
    connect(m_createPanel, &CreateUserPanel::createConfirm, [ = ] {
        setPanelState(StateNormal);
    });

    m_stackWidget->addWidget(m_createPanel);
}

void AccountMainWidget::initHeaderStackWidget()
{
    m_headerStackWidget = new QStackedWidget();

    connect(this, &AccountMainWidget::stateChanged, [ = ](PanelState state) {
        switch (state) {
        case StateNormal:
            m_stackWidget->setCurrentIndex(0);
            m_headerStackWidget->setVisible(true);
            m_headerStackWidget->setCurrentIndex(0);
            break;
        case StateCreating:
            m_stackWidget->setCurrentIndex(1);
            m_headerStackWidget->setVisible(false);
            break;
        case StateDeleting:
            m_stackWidget->setCurrentIndex(0);
            m_headerStackWidget->setVisible(true);
            m_headerStackWidget->setCurrentIndex(0);
            break;
        case StateSetting:
            m_stackWidget->setCurrentIndex(0);
            m_headerStackWidget->setVisible(true);
            m_headerStackWidget->setCurrentIndex(1);
            break;
        default:
            break;
        }
    });
}

void AccountMainWidget::initDBusAccount()
{
    m_account = new DBusAccount(this);
    connect(m_account, &DBusAccount::UserListChanged, [ = ] {
        if (m_account->userList().count() <= 1)
        {
            m_deleteUserButton->setVisible(false);
            m_deleteUserButton->setEnabled(false);
        } else {
            m_deleteUserButton->setVisible(true);
            m_deleteUserButton->setEnabled(true);
        }
    });
}

void AccountMainWidget::setPanelState(AccountMainWidget::PanelState state)
{
    m_state = state;

    emit stateChanged(state);
}

void AccountMainWidget::onAddButtonClicked()
{
    setPanelState(StateCreating);
    m_buttonToolTip->hideLabel();
}

void AccountMainWidget::onAddButtonMouseEntered()
{
    m_buttonToolTip->setText(tr("Add Account"));
    QPoint tp = m_account->userList().count() <= 1 ? m_addUserButton->mapTo(this, QPoint(0, 0)) : m_deleteUserButton->mapTo(this, QPoint(0, 0));
    //the x or width value is valid after all component ready,infact it only need move once
    m_buttonToolTip->move(tp.x() - m_buttonToolTip->width() - DTK_WIDGET_NAMESPACE::TEXT_RIGHT_MARGIN,
                          (m_header->height() - m_buttonToolTip->height()) / 2);

    m_buttonToolTip->showLabel();
}

void AccountMainWidget::onDeleteButtonStateChanged()
{
    DImageButton::State buttonState = m_deleteUserButton->getState();
    if (buttonState == DImageButton::Hover || buttonState == DImageButton::Press) {
        return;
    }

    switch (buttonState) {
    case DImageButton::Checked:
        if (m_state == StateDeleting) {
            break;
        }
        setPanelState(StateDeleting);
        emit requestDelete(true);
        break;
    default:
        if (m_state == StateNormal) {
            break;
        }
        setPanelState(StateNormal);
        emit requestDelete(false);
        break;
    }
    m_buttonToolTip->hideLabel();
}

void AccountMainWidget::onDeleteButtonMouseEntered()
{
    m_buttonToolTip->setText(tr("Delete Account"));
    //the x or width value is valid after all component ready,infact it only need move once
    m_buttonToolTip->move(m_deleteUserButton->mapTo(this, QPoint(0, 0)).x() - m_buttonToolTip->width() - DTK_WIDGET_NAMESPACE::TEXT_RIGHT_MARGIN,
                          (m_header->height() - m_buttonToolTip->height()) / 2);

    m_buttonToolTip->showLabel();
}

