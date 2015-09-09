#include "createuserpanel.h"

CreateUserPanel::CreateUserPanel(QWidget *parent) : QWidget(parent)
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);

    initDbusData();
    if (m_account && m_account->isValid()){
        initHeader();
        initInputLline();
        initConfirmLine();
        initInfoLine();

        m_layout->addStretch();
    }
}

void CreateUserPanel::initDbusData()
{
    m_account = new DBusAccount(this);
    connect(m_account, &DBusAccount::UserAdded, this, &CreateUserPanel::onUserAdded);
    if (m_account->isValid())
        m_randIcon = m_account->RandUserIcon().value();
}

void CreateUserPanel::initHeader()
{
    QLabel *headerLabel = new QLabel(tr("Create New User"));
    headerLabel->setObjectName("CreateHeaderLabel");
    headerLabel->setFixedHeight(DUI::EXPAND_HEADER_HEIGHT);
    headerLabel->setContentsMargins(DUI::HEADER_LEFT_MARGIN, 0 , 0, 0);
    headerLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    m_layout->addWidget(headerLabel);
    DSeparatorHorizontal *s = new DSeparatorHorizontal;
    m_layout->addWidget(s);
}

void CreateUserPanel::initInfoLine()
{
    QLabel *infoFrame = new QLabel;
    infoFrame->setObjectName("CreateInfoLabel");
    infoFrame->setFixedHeight(100);
    QHBoxLayout *hLayout = new QHBoxLayout(infoFrame);
    hLayout->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->setSpacing(0);
    QVBoxLayout *vLayout  = new QVBoxLayout;
    vLayout->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(0);

    m_avatar = new UserAvatar;
    m_avatar->setAvatarSize(UserAvatar::AvatarSmallSize);
    m_avatar->setFixedSize(ICON_SIZE, ICON_SIZE);
    m_avatar->setIcon(m_randIcon);

    m_newNameLabel = new QLabel(tr("New User"));
    m_newNameLabel->setObjectName("NewNameLabel");
    m_newNameLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    QLabel *newTypeLabel = new QLabel(tr("Normal User"));
    newTypeLabel->setObjectName("NewTypeLabel");
    newTypeLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    connect(m_accountType, &AccountTypeLine::typeChanged, [=](int type){
        switch (type) {
        case 1:
            newTypeLabel->setText(tr("Administor"));
            break;
        default:
            newTypeLabel->setText(tr("Normal User"));
            break;
        }
    });

    vLayout->addWidget(m_newNameLabel);
    vLayout->addWidget(newTypeLabel);

    hLayout->addSpacing(DUI::HEADER_LEFT_MARGIN);
    hLayout->addWidget(m_avatar);
    hLayout->addSpacing(DUI::HEADER_LEFT_MARGIN);
    hLayout->addLayout(vLayout);
    hLayout->addStretch(1);

    DSeparatorHorizontal *s = new DSeparatorHorizontal();
    m_layout->insertWidget(2, s);
    m_layout->insertWidget(2, infoFrame);
}

void CreateUserPanel::initInputLline()
{
    m_nameLine = new InputLine();
    m_passwdNew = new PasswdLine();
    m_passwdRepeat = new PasswdLine();
    m_accountType = new AccountTypeLine();
    m_autoLogin = new SwitchLine();
    connect(m_nameLine, &InputLine::textChanged, this, &CreateUserPanel::onNameChanged);
    connect(m_passwdNew, &PasswdLine::textChanged, this, &CreateUserPanel::onPasswdChanged);
    connect(m_passwdRepeat, &PasswdLine::textChanged, this, &CreateUserPanel::onPasswdRepeatChanged);
    connect(m_nameLine, &InputLine::focusChanged, this, &CreateUserPanel::onNameFocusChanged);
    connect(m_passwdNew, &PasswdLine::focusChanged, this, &CreateUserPanel::onPasswdFocusChanged);
    connect(m_passwdRepeat, &PasswdLine::focusChanged, this, &CreateUserPanel::onPasswdRepeatFocusChanged);

    QFont f = m_nameLine->lineEdit()->font();
    f.setCapitalization(QFont::AllLowercase);
    m_nameLine->lineEdit()->setFont(f);

    m_nameLine->setTitle(tr("User Name"));
    m_passwdNew->setTitle(tr("Password"));
    m_passwdRepeat->setTitle(tr("Repeat Password"));
    m_accountType->setTitle(tr("Account Type"));
    m_autoLogin->setTitle(tr("Auto Login"));

    DSeparatorHorizontal *s1 = new DSeparatorHorizontal();
    DSeparatorHorizontal *s2 = new DSeparatorHorizontal();
    DSeparatorHorizontal *s3 = new DSeparatorHorizontal();
    m_layout->addWidget(m_nameLine);
    m_layout->addWidget(s1);
    m_layout->addWidget(m_passwdNew);
    m_layout->addWidget(s2);
    m_layout->addWidget(m_passwdRepeat);
    m_layout->addWidget(s3);
    m_layout->addWidget(m_accountType);
    m_layout->addWidget(m_autoLogin);

    QTimer::singleShot(200, this, SLOT(updateLineStyle()));
}

void CreateUserPanel::initConfirmLine()
{
    m_confirmLine = new ConfirmButtonLine();
    connect(m_confirmLine, &ConfirmButtonLine::cancel, this, &CreateUserPanel::onCancel);
    connect(m_confirmLine, &ConfirmButtonLine::confirm, this, &CreateUserPanel::onConfirm);

    DSeparatorHorizontal *s4 = new DSeparatorHorizontal();
    m_layout->addWidget(m_confirmLine);
    m_layout->addWidget(s4);
}

bool CreateUserPanel::validate()
{
    QDBusPendingReply<bool, QString, int> reply = m_account->IsUsernameValid(m_nameLine->text().toLower());
    bool nameValid = reply.argumentAt(0).isValid() ? reply.argumentAt(0).toBool() : false;
    QString warningMsg = reply.argumentAt(1).isValid() ? reply.argumentAt(1).toString() : "";
    if (!nameValid){
        m_nameLine->showWarning(warningMsg);
        return false;
    }

    if (m_passwdNew->text().isEmpty()){
        m_passwdNew->showWarning(tr("Password can not be empty."));
        return false;
    }

    if (m_passwdRepeat->text().isEmpty()){
        m_passwdRepeat->showWarning(tr("Password can not be empty."));
    }

    if (m_passwdRepeat->text() != m_passwdNew->text()){
        m_passwdRepeat->showWarning(tr("The two passwords do not match."));
        return false;
    }

    return true;
}

void CreateUserPanel::updateLineStyle()
{
    //For Style sheet
    //DUI with a higher priority
    QString backgrounStyle = QString("DUI--DBaseLine { background-color: %1}").arg(lineBackgroundColor());
    m_nameLine->setStyleSheet(m_nameLine->styleSheet() + backgrounStyle);
    m_passwdNew->setStyleSheet(m_passwdNew->styleSheet() + backgrounStyle);
    m_passwdRepeat->setStyleSheet(m_passwdRepeat->styleSheet() + backgrounStyle);
    m_accountType->setStyleSheet(m_accountType->styleSheet() + backgrounStyle);
    m_autoLogin->setStyleSheet(m_autoLogin->styleSheet() + backgrounStyle);
    m_confirmLine->setStyleSheet(m_confirmLine->styleSheet() + backgrounStyle);
}

void CreateUserPanel::resetData()
{
    m_oldName = "";
    m_nameLine->setText("");
    m_nameLine->hideWarning();
    m_passwdNew->setText("");
    m_passwdNew->hideWarning();
    m_passwdRepeat->setText("");
    m_passwdRepeat->hideWarning();
}

void CreateUserPanel::onCancel()
{
    emit createCancel();
    resetData();
}

void CreateUserPanel::onConfirm()
{
    if (validate()){
        m_account->CreateUser(m_nameLine->text().toLower(), "", m_accountType->currentIndex());

        emit createConfirm();
        resetData();
    }
}

void CreateUserPanel::onUserAdded(const QString &path)
{
    DBusAccountUser *user = new DBusAccountUser(path, this);
    if (user->isValid()){
        if (!m_randIcon.isEmpty())
            user->SetIconFile(m_randIcon);
        if (!m_passwdNew->text().isEmpty() && m_passwdNew->text() == m_passwdRepeat->text())
            user->SetPassword(m_passwdNew->text());
        user->SetAutomaticLogin(m_autoLogin->check());

        //clear up(update) data
        m_nameLine->lineEdit()->setText("");
        m_passwdNew->passwordEdit()->setPassword("");
        m_passwdRepeat->passwordEdit()->setPassword("");
        m_accountType->setType(0);
        m_autoLogin->setCheck(false);
        m_randIcon = m_account->RandUserIcon().value();
        m_avatar->setIcon(m_randIcon);
    }
}

void CreateUserPanel::onNameFocusChanged(bool focus)
{
    if (focus){
        m_nameLine->hideWarning();
        m_passwdNew->hideWarning();
        m_passwdRepeat->hideWarning();
    }
    else if (m_nameLine->text().isEmpty())
        m_nameLine->showWarning(tr("Username can not be empty."));
}

void CreateUserPanel::onPasswdFocusChanged(bool focus)
{
    if (focus){
        m_passwdNew->hideWarning();
        m_passwdRepeat->hideWarning();

        if (m_nameLine->text().isEmpty())
            m_nameLine->showWarning(tr("Username can not be empty."));
        else if (!m_passwdRepeat->text().isEmpty() && m_passwdRepeat->text() != m_passwdNew->text())
            m_passwdRepeat->showWarning(tr("The two passwords do not match."));
    }
    else if (m_passwdNew->text().isEmpty() && !m_nameLine->text().isEmpty())
        m_passwdNew->showWarning("Password can not be empty.");
}

void CreateUserPanel::onPasswdRepeatFocusChanged(bool focus)
{
    if (focus){
        m_passwdNew->hideWarning();
        m_passwdRepeat->hideWarning();

        if (m_nameLine->text().isEmpty())
            m_nameLine->showWarning(tr("Username can not be empty."));
    }
    else if (!m_passwdNew->text().isEmpty() && m_passwdRepeat->text().isEmpty())
        m_passwdRepeat->showWarning(tr("The two passwords do not match."));
}

void CreateUserPanel::onNameChanged(const QString &name)
{
    m_nameLine->hideWarning();
    if (!name.isEmpty()){
        QDBusPendingReply<bool, QString, int> reply = m_account->IsUsernameValid(name.toLower());
        bool nameValid = reply.argumentAt(0).isValid() ? reply.argumentAt(0).toBool() : false;
        QString warningMsg = reply.argumentAt(1).isValid() ? reply.argumentAt(1).toString() : "";
        int validCode = reply.argumentAt(2).isValid() ? reply.argumentAt(2).toInt() : -1;
        if (nameValid){
            m_oldName = name;
            m_newNameLabel->setText(name.toLower());
        }
        else{
            if (validCode == 4 || validCode == 5)//4:NameExist,5:SystemUsed
                m_oldName = name;
            else{
                m_nameLine->setText(m_oldName.toLower());
            }
            m_newNameLabel->setText(m_oldName.toLower());
            m_nameLine->showWarning(warningMsg);
        }
    }
    else
        m_newNameLabel->setText(tr("New User"));
}

void CreateUserPanel::onPasswdChanged(const QString &)
{
    m_passwdNew->hideWarning();
    m_passwdRepeat->hideWarning();
    if (!m_passwdRepeat->text().isEmpty() && m_passwdNew->text() != m_passwdRepeat->text())
        m_passwdRepeat->showWarning(tr("The two passwords do not match."));
}

void CreateUserPanel::onPasswdRepeatChanged(const QString &passwd)
{
    m_passwdRepeat->hideWarning();
    if (!m_passwdRepeat->text().isEmpty() && m_passwdNew->text().indexOf(passwd, 0) != 0)
        m_passwdRepeat->showWarning(tr("The two passwords do not match."));
}

QString CreateUserPanel::lineBackgroundColor() const
{
    return m_lineBackgroundColor;
}

void CreateUserPanel::setLineBackgroundColor(const QString &lineBackgroundColor)
{
    m_lineBackgroundColor = lineBackgroundColor;
}




